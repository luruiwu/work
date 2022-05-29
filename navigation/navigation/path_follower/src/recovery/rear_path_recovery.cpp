#include <glog/logging.h>
#include <algorithm>
#include "robot_info.hpp"
#include "recovery/rear_path_recovery.hpp"
#include "speed_decision_base.hpp"

namespace CVTE_BABOT {
RearPathRecovery::RearPathRecovery() {
  recover_v_ = 0.2;
  max_recover_distance_ = 1.0;
  max_recover_distance_config_ = 1.0;
}

void RearPathRecovery::recoverClear() {
  LOG(INFO) << "recover clear";
  recover_v_ = 0.2;
  if (RobotInfo::getPtrInstance()->getMissionType() == MISSIONTYPE::ELVATOR) {
    max_recover_distance_ = 1.5;
  } else {
    max_recover_distance_ = max_recover_distance_config_;
  }

  if (max_recover_distance_ > 1.5) {
    max_recover_distance_ = 1.5;
  }

  recover_start_pose_ = RobotInfo::getPtrInstance()->getCurrentPose();
  recover_start_temp_pose_ = recover_start_pose_;
  // easy_recover_flag = false;

  state_ = RecoveryState::GO_DEFAULT;
  last_state_ = RecoveryState::GO_DEFAULT;
  last_rotate_state_ = RecoveryState::GO_DEFAULT;
}

void RearPathRecovery::changeStateTo(RecoveryState traget_state) {
  if (traget_state != state_) {
    LOG(INFO) << "change state from " << state_ << " to " << traget_state;
    last_state_ = state_;
    state_ = traget_state;
    if (state_ < GO_DEFAULT) {
      if (state_ == RecoveryState::GO_RETREAT ||
          state_ == RecoveryState::GO_ADVANCE) {
        recover_start_temp_pose_ =
            RobotInfo::getPtrInstance()->getCurrentPose();
      }
      if (state_ == RecoveryState::GO_RIGHT ||
          state_ == RecoveryState::GO_LEFT) {
        // 记录旋转过的位置
        last_rotate_state_ = state_;
      }
    }
  }
}

/*===========================后退相关==================================*/
bool RearPathRecovery::checkIfNeedToRecover() {
  // 计算旋转矩阵
  auto cur_pose = RobotInfo::getPtrInstance()->getCurrentPose();
  Eigen::Matrix<double, 2, 3> T_wb;
  T_wb << cos(cur_pose.getYaw()), -sin(cur_pose.getYaw()), cur_pose.getX(),
      sin(cur_pose.getYaw()), cos(cur_pose.getYaw()), cur_pose.getY();

  // 获取各块的障碍因子
  std::vector<float> factor_gather(RecoveryState::GO_DEFAULT, 254.0);
  factor_gather[RecoveryState::GO_RIGHT] =
      SpeedDecisionBase::getInstance()->getRecoverFactor(
          RecoveDir::RECOVER_RIGHT, &T_wb);
  factor_gather[RecoveryState::GO_LEFT] =
      SpeedDecisionBase::getInstance()->getRecoverFactor(
          RecoveDir::RECOVER_LEFT, &T_wb);
  factor_gather[RecoveryState::GO_ADVANCE] =
      SpeedDecisionBase::getInstance()->getRecoverFactor(
          RecoveDir::RECOVER_FRONT, &T_wb);
  factor_gather[RecoveryState::GO_RETREAT] =
      SpeedDecisionBase::getInstance()->getRecoverFactor(
          RecoveDir::RECOVER_BACK, &T_wb);
  float right_arc_factor =
      SpeedDecisionBase::getInstance()->getArcRecoverFactor(
          RecoveDir::RECOVER_RIGHT, &T_wb);
  float left_arc_factor = SpeedDecisionBase::getInstance()->getArcRecoverFactor(
      RecoveDir::RECOVER_LEFT, &T_wb);

  LOG(INFO) << "is factor front: " << factor_gather[RecoveryState::GO_ADVANCE]
            << ", is factor back: " << factor_gather[RecoveryState::GO_RETREAT]
            << ", is factor left: " << factor_gather[RecoveryState::GO_LEFT]
            << ", is factor right: " << factor_gather[RecoveryState::GO_RIGHT];
  LOG(INFO) << "right_arc_factor " << right_arc_factor << " left_arc_factor "
            << left_arc_factor;

  // 判断机器是否脱困成功
  if (SpeedDecisionBase::getInstance()->checkOutofRecovery()) {
    LOG(WARNING) << "robot out of obs ";
    changeStateTo(RecoveryState::GO_DEFAULT);
    return false;
  }

  // 放大后退的代价
  float *back = factor_gather.data() + RecoveryState::GO_RETREAT;
  if (*back < 253.0) {
    *back = *back + 70;
    *back = *back > 252.0 ? 252.0 : *back;
  }

  // 根据运动方向屏蔽某些方向
  switch (state_) {
    case RecoveryState::GO_RETREAT: {
      factor_gather[RecoveryState::GO_ADVANCE] = 254;
      if (cur_pose.distanceTo(recover_start_temp_pose_) < 0.3) {
        // 未长距离后退时  若已发生过旋转 屏蔽旋转
        if (last_state_ == RecoveryState::GO_LEFT) {
          LOG(INFO) << "back ignore right";
          factor_gather[RecoveryState::GO_RIGHT] = 254;
        } else if (last_state_ == RecoveryState::GO_RIGHT) {
          LOG(INFO) << "back ignore left";
          factor_gather[RecoveryState::GO_LEFT] = 254;
        }
      }
    } break;
    case RecoveryState::GO_ADVANCE: {
      factor_gather[RecoveryState::GO_RETREAT] = 254;
      if (cur_pose.distanceTo(recover_start_temp_pose_) < 0.5) {
        // 未长距离前进时  若已发生过旋转 屏蔽旋转
        if (last_state_ == RecoveryState::GO_LEFT) {
          LOG(INFO) << "front ignore right";
          factor_gather[RecoveryState::GO_RIGHT] = 254;
        } else {
          if (last_state_ == RecoveryState::GO_RIGHT) {
            LOG(INFO) << "front ignore left";
            factor_gather[RecoveryState::GO_LEFT] = 254;
          }
        }
      }
    } break;
    case RecoveryState::GO_LEFT: {
      factor_gather[RecoveryState::GO_RIGHT] = 254;
    } break;
    case RecoveryState::GO_RIGHT: {
      factor_gather[RecoveryState::GO_LEFT] = 254;
    } break;
    case RecoveryState::GO_DEFAULT: {
      // 初始化状态 不屏蔽方向
    } break;
    case RecoveryState::GO_ARC_RIGHT: {
      LOG(INFO) << "state is GO_ARC_RIGHT";
    } break;
    case RecoveryState::GO_ARC_LEFT: {
      LOG(INFO) << "state is GO_ARC_LEFT";
    } break;
    default: {
      LOG(INFO) << "state is RECOVERYERROR";
      // 发生错误 机器被困住 机器朝最小方向运动
      std::vector<float>::iterator smallest =
          std::min_element(std::begin(factor_gather), std::end(factor_gather));
      int dir = std::distance(std::begin(factor_gather), smallest);

      // // 最小代价的框内 致命框没有东西
      // if (*smallest < 253.0 && easy_recover_flag == false) {
      //   if (last_rotate_state_ == RecoveryState::GO_RIGHT &&
      //       dir == RecoveryState::GO_LEFT) {
      //     LOG(WARNING) << "easy mode begin";
      //     easy_recover_flag = true;
      //   }
      //   if (last_rotate_state_ == RecoveryState::GO_LEFT &&
      //       dir == RecoveryState::GO_RIGHT) {
      //     LOG(WARNING) << "easy mode begin";
      //     easy_recover_flag = true;
      //   }
      // }
    } break;
  }

  std::vector<float>::iterator smallest =
      std::min_element(std::begin(factor_gather), std::end(factor_gather));
  if (*smallest >= 253.0) {
    // 机器已经被困住 尝试画弧
    if ((right_arc_factor < 253 || left_arc_factor < 253) &&
        RobotInfo::getPtrInstance()->getMissionType() != MISSIONTYPE::ELVATOR) {
      if (right_arc_factor < left_arc_factor) {
        changeStateTo(RecoveryState::GO_ARC_RIGHT);
        LOG(WARNING) << "arc right recover";
      } else {
        changeStateTo(RecoveryState::GO_ARC_LEFT);
        LOG(WARNING) << "arc left recover";
      }
      return true;
    }
    LOG(ERROR) << "robot is trapped";
    changeStateTo(RecoveryState::RECOVERYERROR);
    return true;
  }

  // 选择因子最小的方向
  int dir = std::distance(std::begin(factor_gather), smallest);
  if (dir >= RecoveryState::GO_DEFAULT) {
    LOG(ERROR) << "std error";
    changeStateTo(RecoveryState::RECOVERYERROR);
  } else {
    if ((state_ == RecoveryState::GO_ARC_LEFT && left_arc_factor < 253.0) ||
        (state_ == RecoveryState::GO_ARC_RIGHT && right_arc_factor < 253.0)) {
      if (*smallest > 220) {
        LOG(INFO) << "still arc recover";
        return true;
      }
    }
    bool can_change = true;
    for (int i = 0; i < factor_gather.size(); i++) {
      if (fabs(factor_gather[i] - *smallest) < 0.5) {
        if (i == state_) {
          // 本次遍历内有多个代价值相等的区域，保持上一次的移动方向
          LOG(INFO) << "use last statem ";
          can_change = false;
        }
      }
    }
    if (can_change) {
      changeStateTo((RecoveryState)(dir));
    }
  }

  return true;
}

RecoveryState RearPathRecovery::calculateRecoverVelocity(double &recover_v,
                                                         double &recover_w) {
  // 判断后面是否有障碍,倒退距离过长不再倒退
  Pose2d current_pose = RobotInfo::getPtrInstance()->getCurrentPose();
  double recover_distance = current_pose.distanceTo(recover_start_pose_);

  if (recover_distance > max_recover_distance_ &&
      RecoveryState::GO_RETREAT == state_) {
    LOG_EVERY_N(INFO, 3) << "recover_distance is max " << recover_distance
                         << "(max: " << max_recover_distance_ << ")";
    recover_v = 0.00;
    recover_w = 0.0;
    return RECOVERYERROR;
  } else if (RecoveryState::GO_RETREAT == state_) {
    LOG_EVERY_N(INFO, 3) << "GO RETREAT to recover";
    recover_v = -recover_v_;
    recover_w = 0.0;
    return RECOVERYSUCCESS;
  } else if (RecoveryState::GO_ADVANCE == state_) {
    LOG_EVERY_N(INFO, 3) << "GO ADVANCE to recover";
    recover_v = recover_v_;
    recover_w = 0.0;
    return RECOVERYSUCCESS;
  } else if (RecoveryState::GO_LEFT == state_) {
    LOG_EVERY_N(INFO, 3) << "GO LEFT to recover";
    recover_v = 0.0;
    recover_w = recover_w_;
    return RECOVERYSUCCESS;
  } else if (RecoveryState::GO_RIGHT == state_) {
    LOG_EVERY_N(INFO, 3) << "GO RIGHT to recover";
    recover_v = 0.0;
    recover_w = -recover_w_;
    return RECOVERYSUCCESS;
  } else if (RecoveryState::GO_ARC_RIGHT == state_) {
    LOG_EVERY_N(INFO, 3) << "GO ARC_RIGHT to recover";
    recover_v = recover_v_ / 2;
    recover_w = -recover_w_ / 2;
    return RECOVERYSUCCESS;
  } else if (RecoveryState::GO_ARC_LEFT == state_) {
    LOG_EVERY_N(INFO, 3) << "GO ARC_LEFT to recover";
    recover_v = recover_v_ / 2;
    recover_w = recover_w_ / 2;
    return RECOVERYSUCCESS;
  } else {
    if (RecoveryState::GO_DEFAULT == state_) {
      if (last_state_ == RecoveryState::GO_DEFAULT) {
        LOG_EVERY_N(INFO, 3) << "state init";
      } else {
        LOG_EVERY_N(INFO, 3) << "revocer finish";
      }
      recover_v = 0.00;
      recover_w = 0.0;
    } else {
      // 获取各块的障碍因子
      LOG(INFO) << ", recover fail !";
      auto cur_pose = RobotInfo::getPtrInstance()->getCurrentPose();
      Eigen::Matrix<double, 2, 3> T_wb;
      std::vector<float> factor_gather(RecoveryState::GO_DEFAULT, 254.0);

      T_wb << cos(cur_pose.getYaw()), -sin(cur_pose.getYaw()), cur_pose.getX(),
          sin(cur_pose.getYaw()), cos(cur_pose.getYaw()), cur_pose.getY();

      factor_gather[RecoveryState::GO_RIGHT] =
          SpeedDecisionBase::getInstance()->getRecoverFactor(
              RecoveDir::RECOVER_RIGHT, &T_wb);
      factor_gather[RecoveryState::GO_LEFT] =
          SpeedDecisionBase::getInstance()->getRecoverFactor(
              RecoveDir::RECOVER_LEFT, &T_wb);
      factor_gather[RecoveryState::GO_ADVANCE] =
          SpeedDecisionBase::getInstance()->getRecoverFactor(
              RecoveDir::RECOVER_FRONT, &T_wb);
      factor_gather[RecoveryState::GO_RETREAT] =
          SpeedDecisionBase::getInstance()->getRecoverFactor(
              RecoveDir::RECOVER_BACK, &T_wb);

      LOG_EVERY_N(WARNING, 3)
          << "last state: " << static_cast<int>(last_state_)
          << ", obs_fron: " << factor_gather[RecoveryState::GO_ADVANCE]
          << ", obs_back: " << factor_gather[RecoveryState::GO_RETREAT]
          << ", obs left: " << factor_gather[RecoveryState::GO_LEFT]
          << ", obs right: " << factor_gather[RecoveryState::GO_RIGHT]
          << ", recover fail !";
    }
    recover_v = 0;
    recover_w = 0;
    return RECOVERYERROR;
  }
}
}  // namespace CVTE_BABOT
