
#include "map/map_manager.hpp"

#include <glog/logging.h>

#include "common/math_base/slam_transform.hpp"
#include "common/data_struct/keyframe.hpp"
#include "common/data_struct/our_cloud_downsample.hpp"

namespace cvte_lidar_slam {

std::shared_ptr<MapManager> MapManager::ptr_instance_ = nullptr;
std::mutex MapManager::instance_mutex_;

std::shared_ptr<MapManager> MapManager::getInstance() {
  if (nullptr == ptr_instance_) {
    std::lock_guard<std::mutex> lock(instance_mutex_);
    if (nullptr == ptr_instance_) {
      // ptr_instance_ = std::make_shared<MapManager>();
      ptr_instance_.reset(new MapManager);
    }
  }
  return ptr_instance_;
}

MapManager::MapManager() {
  ptr_state_machine_ = SlamStateMachine::getInstance();
  surround_frame_.reset(new laserCloud());
  ds_surround_frame_.reset(new laserCloud());
  frame_pose_tree_.reset(new laserCloud());
  ds_global_frame_.reset(new laserCloud());
  reserved_global_frame_.reset(new laserCloud());
  all_surf_cloud_.reset(new laserCloud());
  all_corner_cloud_.reset(new laserCloud());
  ptr_kdtree_frame_.reset(new pcl::KdTreeFLANN<PointType>());
}

MapManager::~MapManager() {
  l_ptr_keyframe_.clear();
  keyframe_database_.clear();
}

void MapManager::initParameters(const BackendConfig &config) {
  config_ = config;
  ptr_cloud_downsampler_ = std::make_shared<CloudDownsample>();
  ptr_cloud_downsampler_->setLeafSize(config_.leaf_size, config_.leaf_size,
                                      config_.leaf_size);
  ptr_surroundcloud_downsampler_ = std::make_shared<CloudDownsample>();
  ptr_surroundcloud_downsampler_->setLeafSize(
      config_.leaf_size, config_.leaf_size, config_.leaf_size);
  ptr_viewcloud_downsampler_ = std::make_shared<CloudDownsample>();
  ptr_viewcloud_downsampler_->setLeafSize(config_.leaf_size, config_.leaf_size,
                                          config_.leaf_size);
  global_frame_pose_.setLeafSize(0.8, 0.8, 0.8);
  global_frame_pose_for_map_.setLeafSize(0.4, 0.4, 0.4);
  global_cloud_.setLeafSize(0.4, 0.4, 0.4);
  latest_frame_id_ = 0;
  l_ptr_keyframe_.clear();
  keyframe_database_.clear();
  um_keyframes_info_.clear();
}

void MapManager::Reset() {
  std::unique_lock<std::mutex> lock(mutex_);
  l_ptr_keyframe_.clear();
  keyframe_database_.clear();
  um_keyframes_info_.clear();
  std::map<size_t, std::shared_ptr<KeyFrame>>().swap(keyframe_database_);
  frame_pose_tree_->clear();
  ptr_cloud_downsampler_->Clear();
  latest_frame_id_ = 0;
}

bool MapManager::saveKeyFrame(const std::string &file,
                              const std::string &temp_file) {
  laserCloud::Ptr ds_keyframe_for_map(new laserCloud());
  laserCloud::Ptr our_ds_keyframe_pose(new laserCloud());
  if (!ptr_cloud_downsampler_->setOutputCloud(our_ds_keyframe_pose)) {
    return false;
  }
  int pointcloud_number = our_ds_keyframe_pose->points.size();
  if (0 == pointcloud_number) {
    return false;
  }

  laserCloud::Ptr all_corner_cloud(new laserCloud());
  laserCloud::Ptr all_surf_cloud(new laserCloud());

  LOG(ERROR) << "\nsaving keyframe...... ";
  size_t keyframe_num = 0;
  for (int i = 0; i < pointcloud_number; ++i) {
    size_t frame_id = (size_t) our_ds_keyframe_pose->points[i].intensity;
    if (keyframe_database_.find(frame_id) != keyframe_database_.end()) {
      auto temp_frame = keyframe_database_.find(frame_id)->second;
      Mat34d keyframe_pose = temp_frame->getPose();
      if (!temp_frame->isActived()) {
        if (!activeKeyFrame(temp_file, temp_frame)) {
          LOG(ERROR) << "Active frame failed";
          continue;
        }
      }
      laserCloud::Ptr surroundingCloudCorner =
          Transformbox::transformPointCloud(keyframe_pose,
                                            temp_frame->corner_cloud_);
      laserCloud::Ptr surroundingCloudSurf = Transformbox::transformPointCloud(
          keyframe_pose, temp_frame->surf_cloud_);
      temp_frame->corner_cloud_->clear();
      temp_frame->surf_cloud_->clear();

      if (surroundingCloudCorner->points.empty() ||
          surroundingCloudSurf->points.empty()) {
        continue;
      }

      std::string corner_file =
          file + "corner" + std::to_string(frame_id) + ".pcd";
      std::string surf_file = file + "surf" + std::to_string(frame_id) + ".pcd";

      std::cout.width(3);  // i的输出为3位宽
      std::cout << (100 * (i + 1) / pointcloud_number) << "%";
      std::cout << "\b\b\b\b";  //回删三个字符，使数字在原地变化

      FramePosition pos;
      pos = our_ds_keyframe_pose->points[i];
      pos.intensity = frame_id;

      ds_keyframe_for_map->points.push_back(pos);
      keyframe_num++;

      *all_corner_cloud += *surroundingCloudCorner;
      *all_surf_cloud += *surroundingCloudSurf;

      pcl::io::savePCDFileBinaryCompressed(corner_file,
                                           *surroundingCloudCorner);
      pcl::io::savePCDFileBinaryCompressed(surf_file, *surroundingCloudSurf);
    }
  }
  if (keyframe_num <= 0) {
    LOG(ERROR) << "There's no any keyframe in map";
    return false;
  }
  std::string pose_file = file + "keyframe_pos.pcd";
  ds_keyframe_for_map->width = 1;
  ds_keyframe_for_map->height = keyframe_num;
  pcl::io::savePCDFileBinaryCompressed(pose_file, *ds_keyframe_for_map);

  global_cloud_.setInputCloud(all_corner_cloud);
  global_cloud_.filter(*all_corner_cloud_);
  if (all_corner_cloud_->points.empty()) {
    LOG(ERROR) << "Corner map is empty";
    return false;
  }
  pcl::io::savePCDFileBinaryCompressed(file + "all_corner_cloud.pcd",
                                       *all_corner_cloud_);

  global_cloud_.setInputCloud(all_surf_cloud);
  global_cloud_.filter(*all_surf_cloud_);
  if (all_surf_cloud_->points.empty()) {
    LOG(ERROR) << "Surf map is empty";
    return false;
  }
  pcl::io::savePCDFileBinaryCompressed(file + "all_surf_cloud.pcd",
                                       *all_surf_cloud_);

  std::cout << "\n\n";
  return true;
}

bool MapManager::saveAllKeyFrame(const std::string &file,
                                 const std::string &temp_file) {
  const size_t &pointcloud_number = keyframe_database_.size();
  if (0 == pointcloud_number) {
    return false;
  }
  std::ofstream keyframe_info_file;
  laserCloud::Ptr all_corner_cloud(new laserCloud());
  laserCloud::Ptr all_surf_cloud(new laserCloud());
  laserCloud::Ptr keyframe_for_map(new laserCloud());
  const std::string &info_file_name = file + "keyframe_info.txt";
  keyframe_info_file.open(info_file_name, std::ios::out | std::ios::trunc);
  keyframe_info_file << std::fixed;
  if (!keyframe_info_file.is_open()) {
    LOG(ERROR) << " Can not open file";
    return false;
  }
  keyframe_info_file << "#format: frame_id tx ty tz qw qx qy qz pose_cov "
                        "gps_flag gx gy gz g_cov"
                     << std::endl;

  LOG(ERROR) << "\nsaving keyframe...... ";
  size_t keyframe_num = 0;
  for (auto kf_iter = keyframe_database_.begin();
       kf_iter != keyframe_database_.end(); kf_iter++) {
    std::shared_ptr<KeyFrame> ptr_keyframe = kf_iter->second;
    const Mat34d &keyframe_pose = ptr_keyframe->getPose();
    const size_t &frame_id = ptr_keyframe->index_;
    if (!ptr_keyframe->isActived()) {
      if (!activeKeyFrame(temp_file, ptr_keyframe)) {
        LOG(ERROR) << "Active frame failed";
        continue;
      }
    }
    laserCloud::Ptr surroundingCloudCorner = Transformbox::transformPointCloud(
        keyframe_pose, ptr_keyframe->corner_cloud_);
    laserCloud::Ptr surroundingCloudSurf = Transformbox::transformPointCloud(
        keyframe_pose, ptr_keyframe->surf_cloud_);

    if (surroundingCloudCorner->points.empty() ||
        surroundingCloudSurf->points.empty()) {
      continue;
    }

    std::string corner_file =
        file + "corner" + std::to_string(frame_id) + ".pcd";
    std::string surf_file = file + "surf" + std::to_string(frame_id) + ".pcd";

    std::cout.width(3);  // i的输出为3位宽
    std::cout << (100 * (keyframe_num + 1) / pointcloud_number) << "%";
    std::cout << "\b\b\b\b";  //回删三个字符，使数字在原地变化

    FramePosition pos;
    pos.x = keyframe_pose(0, 3);
    pos.y = keyframe_pose(1, 3);
    pos.z = keyframe_pose(2, 3);
    pos.intensity = frame_id;

    keyframe_for_map->points.push_back(pos);
    keyframe_num++;

    *all_corner_cloud += *surroundingCloudCorner;
    *all_surf_cloud += *surroundingCloudSurf;

    pcl::io::savePCDFileBinaryCompressed(corner_file,
                                         *ptr_keyframe->corner_cloud_);
    pcl::io::savePCDFileBinaryCompressed(surf_file, *ptr_keyframe->surf_cloud_);
    const double &pose_cov = ptr_keyframe->map_cov_(3, 3);
    const bool &gps_flag = ptr_keyframe->has_gps_;
    Vec3d gps_pos(0.0, 0.0, 0.0);
    double gps_cov = 100.0;
    if (gps_flag) {
      gps_pos = ptr_keyframe->gps_pos_;
      gps_cov = ptr_keyframe->gps_cov_;
    }
    Eigen::Quaterniond quat(keyframe_pose.block<3, 3>(0, 0));
    quat.normalize();
    keyframe_info_file << std::setprecision(0) << frame_id << ' '
                       << std::setprecision(6) << pos.x << ' ' << pos.y << ' '
                       << pos.z << ' ' << quat.x() << ' ' << quat.y() << ' '
                       << quat.z() << ' ' << quat.w() << ' ' << pose_cov << ' '
                       << gps_flag << ' ' << gps_pos[0] << ' ' << gps_pos[1]
                       << ' ' << gps_pos[2] << ' ' << gps_cov << std::endl;
    ptr_keyframe->corner_cloud_->clear();
    ptr_keyframe->surf_cloud_->clear();
    ptr_keyframe->corner_cloud_->points.shrink_to_fit();
    ptr_keyframe->surf_cloud_->points.shrink_to_fit();
  }
  keyframe_info_file.close();
  if (keyframe_num <= 0) {
    LOG(ERROR) << "There's no any keyframe in map";
    return false;
  }
  std::string pose_file = file + "keyframe_pos.pcd";
  keyframe_for_map->width = 1;
  keyframe_for_map->height = keyframe_num;
  pcl::io::savePCDFileBinaryCompressed(pose_file, *keyframe_for_map);

  global_cloud_.setInputCloud(all_corner_cloud);
  global_cloud_.filter(*all_corner_cloud_);
  if (all_corner_cloud_->points.empty()) {
    LOG(ERROR) << "Corner map is empty";
    return false;
  }
  pcl::io::savePCDFileBinaryCompressed(file + "all_corner_cloud.pcd",
                                       *all_corner_cloud_);

  global_cloud_.setInputCloud(all_surf_cloud);
  global_cloud_.filter(*all_surf_cloud_);
  if (all_surf_cloud_->points.empty()) {
    LOG(ERROR) << "Surf map is empty";
    return false;
  }
  pcl::io::savePCDFileBinaryCompressed(file + "all_surf_cloud.pcd",
                                       *all_surf_cloud_);

  std::cout << "\n\n";
  return true;
}

bool MapManager::loadLocalKeyFrame(const std::string &file, FramePosition pos) {
  std::string map_file = file;
  if (map_file.back() != '/') {
    map_file.push_back('/');
  }
  if (!loadKeyFrameInfo(map_file)) {
    LOG(ERROR) << "Failed to load keyframe info";
    return false;
  }
  std::string pose_file = map_file + "keyframe_pos.pcd";
  if (pcl::io::loadPCDFile<PointType>(pose_file, *frame_pose_tree_) == -1) {
    LOG(ERROR) << "keyframe_pos is empty";
    return false;
  }
  if (frame_pose_tree_->size() != um_keyframes_info_.size()) {
    LOG(ERROR) << "Keyframes size wrong";
    return false;
  }
  if (pcl::io::loadPCDFile<PointType>(file + "all_corner_cloud.pcd",
                                      *all_corner_cloud_) == -1) {
    LOG(ERROR) << "all_corner_cloud is empty";
    return false;
  }
  if (pcl::io::loadPCDFile<PointType>(file + "all_surf_cloud.pcd",
                                      *all_surf_cloud_) == -1) {
    LOG(ERROR) << "all_surf_cloud is empty";
    return false;
  }
  int pointcloud_number = frame_pose_tree_->points.size();
  ptr_kdtree_frame_->setInputCloud(frame_pose_tree_);
  std::cout << "\nloading keyframe...... " << pointcloud_number << std::endl;
  for (int i = 0; i < pointcloud_number; ++i) {
    FramePosition point = frame_pose_tree_->points[i];
    double distance = std::sqrt((point.x - pos.x) * (point.x - pos.x) +
                                (point.y - pos.y) * (point.y - pos.y) +
                                (point.z - pos.z) * (point.z - pos.z));
    if (std::fabs(distance) < config_.surround_search_radius) {
      size_t frame_id = (size_t) frame_pose_tree_->points[i].intensity;
      laserCloud::Ptr corner_cloud(new laserCloud());
      laserCloud::Ptr surf_cloud(new laserCloud());

      std::string corner_file =
          file + "corner" + std::to_string(frame_id) + ".pcd";
      std::string surf_file = file + "surf" + std::to_string(frame_id) + ".pcd";

      std::cout.width(3);  // i的输出为3位宽
      std::cout << (100 * (i + 1) / pointcloud_number) << "%";
      std::cout << "\b\b\b\b";  //回删三个字符，使数字在原地变化

      pcl::io::loadPCDFile<PointType>(corner_file, *corner_cloud);
      pcl::io::loadPCDFile<PointType>(surf_file, *surf_cloud);
      auto keyframe_info_iter = um_keyframes_info_.find(frame_id);
      if (keyframe_info_iter == um_keyframes_info_.end()) {
        LOG(ERROR) << "Wrong frame id: " << frame_id;
        continue;
      }
      auto keyframe = std::make_shared<KeyFrame>(keyframe_info_iter->second,
                                                 corner_cloud, surf_cloud);
      this->Add(keyframe);
    } else {
      reserved_global_frame_->points.push_back(point);
    }
  }

  std::cout << "\n\n";
  return true;
}

bool MapManager::loadReservedKeyFrame(const std::string &file) {
  if (reserved_global_frame_->points.empty()) {
    return false;
  }
  int pointcloud_number = reserved_global_frame_->points.size();
  std::cout << "\nloading reserved global keyframe...... " << pointcloud_number
            << std::endl;

  for (int i = 0; i < pointcloud_number; ++i) {
    size_t frame_id = (size_t) reserved_global_frame_->points[i].intensity;
    laserCloud::Ptr corner_cloud(new laserCloud());
    laserCloud::Ptr surf_cloud(new laserCloud());
    std::string corner_file =
        file + "corner" + std::to_string(frame_id) + ".pcd";
    std::string surf_file = file + "surf" + std::to_string(frame_id) + ".pcd";

    std::cout.width(3);  // i的输出为3位宽
    std::cout << (100 * (i + 1) / pointcloud_number) << "%";
    std::cout << "\b\b\b\b";  //回删三个字符，使数字在原地变化

    pcl::io::loadPCDFile<PointType>(corner_file, *corner_cloud);
    pcl::io::loadPCDFile<PointType>(surf_file, *surf_cloud);
    auto keyframe_info_iter = um_keyframes_info_.find(frame_id);
    if (keyframe_info_iter == um_keyframes_info_.end()) {
      LOG(ERROR) << "Wrong frame id: " << frame_id;
      continue;
    }
    auto keyframe = std::make_shared<KeyFrame>(keyframe_info_iter->second,
                                               corner_cloud, surf_cloud);
    // TODO: l_ptr_keyframe_的数据结构检查一下，两处都在使用，需要明确区分
    l_ptr_keyframe_.emplace_back(keyframe);
  }

  std::cout << "\n\n";
  return true;
}

bool MapManager::addBufferToMap() {
  // std::unique_lock<std::mutex> lock(mutex_);
  if (l_ptr_keyframe_.empty()) {
    return false;
  }
  for (const auto iter : l_ptr_keyframe_) { this->Add(iter); }
  return true;
}

bool MapManager::loadKeyFrame(const std::string &file) {
  LOG(ERROR) << "Don't use this function now";
  return false;
  std::string pose_file = file + "keyframe_pos.pcd";
  if (pcl::io::loadPCDFile<PointType>(pose_file, *frame_pose_tree_) ==
      -1)  //* load the file
  {
    return false;
  }
  int pointcloud_number = frame_pose_tree_->points.size();
  std::cout << "\n keyframe count: " << pointcloud_number << std::endl;
  std::cout << "loading keyframe...... ";

  for (int i = 0; i < pointcloud_number; ++i) {
    size_t frame_id = (size_t) frame_pose_tree_->points[i].intensity;
    laserCloud::Ptr corner_cloud(new laserCloud());
    laserCloud::Ptr surf_cloud(new laserCloud());
    std::string corner_file =
        file + "corner" + std::to_string(frame_id) + ".pcd";
    std::string surf_file = file + "surf" + std::to_string(frame_id) + ".pcd";

    std::cout.width(3);  // i的输出为3位宽
    std::cout << (100 * (i + 1) / pointcloud_number) << "%";
    std::cout << "\b\b\b\b";  //回删三个字符，使数字在原地变化

    pcl::io::loadPCDFile<PointType>(corner_file, *corner_cloud);
    pcl::io::loadPCDFile<PointType>(surf_file, *surf_cloud);
    FramePosition pos = frame_pose_tree_->points[i];
    auto keyframe = std::make_shared<KeyFrame>(pos, corner_cloud, surf_cloud);
    this->Add(keyframe);
  }

  std::cout << "\n\n";
  return true;
}

bool MapManager::loadKeyFrameInfo(const std::string &file) {
  std::ifstream keyframe_info_file;
  const std::string &info_data_dir = file + "keyframe_info.txt";
  keyframe_info_file.open(info_data_dir);
  if (!keyframe_info_file.is_open()) {
    LOG(ERROR) << " Can not open file";
    return false;
  }
  um_keyframes_info_.clear();
  std::unordered_map<size_t, KeyFrameInfo>().swap(um_keyframes_info_);
  // malloc_trim(0);
  while (!keyframe_info_file.eof()) {
    KeyFrameInfo keyframe_info;
    double pose[7] = {0.};
    size_t frame_id;
    double pose_cov;
    Mat34d pose3d;
    std::string s;
    std::getline(keyframe_info_file, s);
    if (s.front() == '#' || s.empty()) {
      continue;
    }
    std::stringstream ss;
    ss << s;
    ss >> frame_id;

    for (uint i = 0; i < 7; ++i) { ss >> pose[i]; }
    Eigen::Quaterniond Quat(pose[6], pose[3], pose[4], pose[5]);
    Eigen::Map<Eigen::Vector3d> Trans(pose);
    Quat.normalize();
    pose3d.block<3, 3>(0, 0) = Quat.toRotationMatrix();
    pose3d.block<3, 1>(0, 3) = Trans;
    ss >> pose_cov;
    uint gps_flags = 0;
    ss >> gps_flags;
    double gps_info[4] = {0.};
    for (uint i = 0; i < 4; ++i) { ss >> gps_info[i]; }
    Eigen::Map<Vec3d> gps_pos(gps_info);
    if (gps_flags > 0) {
      keyframe_info =
          KeyFrameInfo(frame_id, pose3d, pose_cov, true, gps_pos, gps_info[3]);

    } else {
      keyframe_info = KeyFrameInfo(frame_id, pose3d, pose_cov);
    }
    um_keyframes_info_.insert(std::make_pair(frame_id, keyframe_info));
    // if(!v_keyframes_info_.empty()) {
    //   auto keyframe_info = v_keyframes_info_.back();
    //   std::cout << std::setprecision(0) << keyframe_info.frame_id << ' '
    //             << std::setprecision(6) << std::endl;
    //   std::cout << std::setprecision(6) << '\n' << keyframe_info.pose <<
    //   '\n'; std::cout << std::setprecision(6) << '\n' <<
    //   keyframe_info.pose_cov << '\n'; std::cout << std::setprecision(6) <<
    //   keyframe_info.has_gps << '\n' << keyframe_info.gps_pos.transpose() <<
    //   '\n'; std::cout << std::setprecision(6) << '\n' <<
    //   keyframe_info.gps_cov << '\n';
    // }
  }
  keyframe_info_file.close();
  return true;
}

void MapManager::Update() {
  std::unique_lock<std::mutex> lock(mutex_);
  FramePosition pos;
  frame_pose_tree_->points.clear();
  ptr_cloud_downsampler_->Clear();
  for (auto kf_iter = keyframe_database_.begin();
       kf_iter != keyframe_database_.end(); kf_iter++) {
    std::shared_ptr<KeyFrame> ptr_keyframe = kf_iter->second;
    Mat34d keyframe_pose = ptr_keyframe->getPose();
    pos.x = keyframe_pose(0, 3);
    pos.y = keyframe_pose(1, 3);
    pos.z = keyframe_pose(2, 3);
    pos.intensity = ptr_keyframe->index_;
    frame_pose_tree_->points.push_back(pos);
    ptr_cloud_downsampler_->InsertPoint(pos);
  }
  ptr_kdtree_frame_->setInputCloud(frame_pose_tree_);
}

void MapManager::Add(std::shared_ptr<KeyFrame> ptr_keyframe) {
  std::unique_lock<std::mutex> lock(mutex_);
  FramePosition pos;
  Mat34d keyframe_pose = ptr_keyframe->getPose();
  pos.x = keyframe_pose(0, 3);
  pos.y = keyframe_pose(1, 3);
  pos.z = keyframe_pose(2, 3);
  pos.intensity = ptr_keyframe->index_;
  if (AD_MODE::AD_MAPPING == ptr_state_machine_->getCurrentMode()) {
    frame_pose_tree_->points.push_back(pos);
    ptr_cloud_downsampler_->InsertPoint(pos);
    ptr_kdtree_frame_->setInputCloud(frame_pose_tree_);
    if (l_ptr_keyframe_.size() > config_.surround_search_num) {
      l_ptr_keyframe_.pop_front();
    }
    l_ptr_keyframe_.push_back(ptr_keyframe);
  }

  if (keyframe_database_.find(ptr_keyframe->index_) ==
      keyframe_database_.end()) {
    keyframe_database_.insert(std::pair<size_t, std::shared_ptr<KeyFrame>>(
        ptr_keyframe->index_, ptr_keyframe));
  }
}

std::vector<std::shared_ptr<KeyFrame>> MapManager::DetectCandidatesByTime(
    const unsigned int number) {
  std::unique_lock<std::mutex> lock(mutex_);
  std::vector<std::shared_ptr<KeyFrame>> v_keyframe;  /// <

  v_keyframe.clear();
  for (auto it = l_ptr_keyframe_.rbegin(); it != l_ptr_keyframe_.rend(); ++it) {
    if (v_keyframe.size() < number) {
      v_keyframe.push_back(*it);
    } else {
      break;
    }
  }
  return v_keyframe;
}

std::vector<std::shared_ptr<KeyFrame>> MapManager::getAllKeyFrames() {
  std::unique_lock<std::mutex> lock(mutex_);
  std::vector<std::shared_ptr<KeyFrame>> v_keyframe;
  v_keyframe.clear();
  v_keyframe.reserve(keyframe_database_.size());
  for (const auto &it : keyframe_database_) {
    v_keyframe.emplace_back(it.second);
  }
  return v_keyframe;
}

std::vector<std::shared_ptr<KeyFrame>> MapManager::DetectCandidatesByIndex(
    const int index, const int number) {
  std::unique_lock<std::mutex> lock(mutex_);
  std::vector<std::shared_ptr<KeyFrame>> v_keyframe;  /// <
  std::deque<std::shared_ptr<KeyFrame>>
      q_keyframe;  // TODO:这里不应该使用临时变量，会增加计算量
  v_keyframe.clear();
  if (keyframe_database_.empty()) {
    return v_keyframe;
  }

  for (int i = index - number; i <= index + number; ++i) {
    auto keyframe_iter = keyframe_database_.find(i);
    if (keyframe_iter != keyframe_database_.end()) {
      v_keyframe.emplace_back(keyframe_iter->second);
    }
  }

  return v_keyframe;
}

std::vector<std::shared_ptr<KeyFrame>> MapManager::DetectCandidatesByDistance(
    const FramePosition &pos, const double radius) {
  std::unique_lock<std::mutex> lock(mutex_);
  std::vector<std::shared_ptr<KeyFrame>> v_keyframe;  /// <
  v_keyframe.clear();
  if (keyframe_database_.empty()) {
    return v_keyframe;
  }
  surround_frame_->clear();
  ds_surround_frame_->clear();
  ptr_surroundcloud_downsampler_->Clear();

  std::vector<int> pointSearchInd;  // TODO:事先resize 提高效率
  std::vector<float> pointSearchSqDis;
  ptr_kdtree_frame_->radiusSearch(pos, radius, pointSearchInd, pointSearchSqDis,
                                  0);
  for (unsigned int i = 0; i < pointSearchInd.size(); ++i) {
    surround_frame_->points.push_back(
        frame_pose_tree_->points[pointSearchInd[i]]);
  }
  ptr_surroundcloud_downsampler_->InsertCloud(surround_frame_);
  if (!ptr_surroundcloud_downsampler_->setOutputCloud(ds_surround_frame_)) {
    return v_keyframe;
  }

  unsigned int ds_surround_frame_num = ds_surround_frame_->points.size();
  for (unsigned int i = 0; i < ds_surround_frame_num; ++i) {
    size_t frame_id = (size_t) ds_surround_frame_->points[i].intensity;
    if (keyframe_database_.find(frame_id) != keyframe_database_.end()) {
      v_keyframe.emplace_back(keyframe_database_.find(frame_id)->second);
    }
  }
  return v_keyframe;
}

std::vector<std::shared_ptr<KeyFrame>>
MapManager::DetectCandidatesByHorizontalDistance(const FramePosition &pos,
                                                 const double radius) {
  std::unique_lock<std::mutex> lock(mutex_);
  std::vector<std::shared_ptr<KeyFrame>> v_keyframe;  /// <
  v_keyframe.clear();
  if (keyframe_database_.empty()) {
    return v_keyframe;
  }
  surround_frame_->clear();
  ds_surround_frame_->clear();
  ptr_surroundcloud_downsampler_->Clear();

  for (unsigned int i = 0; i < frame_pose_tree_->points.size(); ++i) {
    double distance = sqrt((pos.x - frame_pose_tree_->points[i].x) *
                               (pos.x - frame_pose_tree_->points[i].x) +
                           (pos.y - frame_pose_tree_->points[i].y) *
                               (pos.y - frame_pose_tree_->points[i].y));
    if (radius > distance) {
      surround_frame_->points.push_back(frame_pose_tree_->points[i]);
    }
  }
  ptr_surroundcloud_downsampler_->InsertCloud(surround_frame_);
  if (!ptr_surroundcloud_downsampler_->setOutputCloud(ds_surround_frame_)) {
    return v_keyframe;
  }
  unsigned int ds_surround_frame_num = ds_surround_frame_->points.size();

  for (unsigned int i = 0; i < ds_surround_frame_num; ++i) {
    size_t frame_id = (size_t) ds_surround_frame_->points[i].intensity;
    // std::cout << "frame index " << frame_id << std::endl;
    if (keyframe_database_.find(frame_id) != keyframe_database_.end()) {
      v_keyframe.emplace_back(keyframe_database_.find(frame_id)->second);
    }
  }
  return v_keyframe;
}

std::vector<size_t>
MapManager::DetectCandidatesIdByHorizontalDistance(const FramePosition &pos,
                                                  const double radius) {
  std::unique_lock<std::mutex> lock(mutex_);
  std::vector<size_t> v_frame_id;  /// <
  std::vector<FramsDis> v_frame_dis;
  v_frame_dis.clear();
  v_frame_id.clear();
  if (keyframe_database_.empty()) {
    return v_frame_id;
  }
  surround_frame_->clear();
  ds_surround_frame_->clear();
  ptr_surroundcloud_downsampler_->Clear();

  std::vector<int> pointSearchInd;  // TODO:事先resize 提高效率
  std::vector<float> pointSearchSqDis;

  for (unsigned int i = 0; i < frame_pose_tree_->points.size(); ++i) {
    double distance = sqrt((pos.x - frame_pose_tree_->points[i].x) *
                               (pos.x - frame_pose_tree_->points[i].x) +
                           (pos.y - frame_pose_tree_->points[i].y) *
                               (pos.y - frame_pose_tree_->points[i].y));
    if (radius > distance) {
      surround_frame_->points.push_back(frame_pose_tree_->points[i]);
    }
  }
  ptr_surroundcloud_downsampler_->InsertCloud(surround_frame_);
  if (!ptr_surroundcloud_downsampler_->setOutputCloud(ds_surround_frame_)) {
    return v_frame_id;
  }
  unsigned int ds_surround_frame_num = ds_surround_frame_->points.size();

  for (unsigned int i = 0; i < ds_surround_frame_num; ++i) {
    size_t frame_id =
        (size_t)ds_surround_frame_->points[i].intensity;
    double distance = sqrt((pos.x - ds_surround_frame_->points[i].x) *
                              (pos.x - ds_surround_frame_->points[i].x) +
                           (pos.y - ds_surround_frame_->points[i].y) *
                               (pos.y - ds_surround_frame_->points[i].y));
    if (keyframe_database_.find(frame_id) != keyframe_database_.end()) {
      FramsDis frame_dis;
      frame_dis.distance = distance;
      frame_dis.frame_id = frame_id;
      v_frame_dis.emplace_back(frame_dis);
    }
  }

  sort(v_frame_dis.begin(), v_frame_dis.end(), MapManager::compFrameByDis);
  for(int i = 0;i < v_frame_dis.size();i++){
    v_frame_id.emplace_back(v_frame_dis[i].frame_id);
  }
  return v_frame_id;
}

// TODO:尝试采用增量更新，或者有优化时再跟新
laserCloud::Ptr MapManager::getGlobalCloud() {
  {
    std::unique_lock<std::mutex> lock(mutex_);
    ds_global_frame_->clear();
    ptr_viewcloud_downsampler_->Clear();
    ptr_viewcloud_downsampler_->InsertCloud(frame_pose_tree_);
    ptr_viewcloud_downsampler_->setOutputCloud(ds_global_frame_);
  }
  laserCloud::Ptr global_cloud(new laserCloud());
  laserCloud::Ptr ds_global_cloud(new laserCloud());

  int ds_global_frame_frame_num = ds_global_frame_->points.size();
  for (int i = 0; i < ds_global_frame_frame_num; ++i) {
    size_t frame_id = (size_t) ds_global_frame_->points[i].intensity;
    if (keyframe_database_.find(frame_id) != keyframe_database_.end()) {
      auto temp_frame = keyframe_database_.find(frame_id)->second;
      if (!temp_frame->isActived()) {
        continue;
      }
      Mat34d keyframe_pose = temp_frame->getPose();
      laserCloud::Ptr surroundingCloudCorner =
          Transformbox::transformPointCloud(keyframe_pose,
                                            temp_frame->corner_cloud_);
      laserCloud::Ptr surroundingCloudSurf = Transformbox::transformPointCloud(
          keyframe_pose, temp_frame->surf_cloud_);
      *global_cloud += *surroundingCloudCorner;
      *global_cloud += *surroundingCloudSurf;
    }
  }
  global_cloud_.setInputCloud(global_cloud);
  global_cloud_.filter(*ds_global_cloud);
  return ds_global_cloud;
}

laserCloud::Ptr MapManager::getSurroundCloud(const FramePosition &pos,
                                             const double radius) {
  std::unique_lock<std::mutex> lock(mutex_);
  laserCloud::Ptr surround_cloud(new laserCloud());
  laserCloud::Ptr ds_surround_cloud(new laserCloud());
  std::vector<int> pointSearchInd;  // TODO:事先resize 提高效率
  std::vector<float> pointSearchSqDis;
  ptr_kdtree_frame_->radiusSearch(pos, radius, pointSearchInd, pointSearchSqDis,
                                  0);

  for (unsigned int i = 0; i < pointSearchInd.size(); ++i) {
    surround_frame_->points.push_back(
        frame_pose_tree_->points[pointSearchInd[i]]);
    size_t frame_id =
        (size_t) frame_pose_tree_->points[pointSearchInd[i]].intensity;
    if (keyframe_database_.find(frame_id) != keyframe_database_.end()) {
      auto temp_frame = keyframe_database_.find(frame_id)->second;
      if (!temp_frame->isActived()) {
        continue;
      }
      Mat34d keyframe_pose = temp_frame->getPose();
      laserCloud::Ptr surroundingCloudCorner =
          Transformbox::transformPointCloud(keyframe_pose,
                                            temp_frame->corner_cloud_);
      laserCloud::Ptr surroundingCloudSurf = Transformbox::transformPointCloud(
          keyframe_pose, temp_frame->surf_cloud_);
      *surround_cloud += *surroundingCloudCorner;
      *surround_cloud += *surroundingCloudSurf;
    }
  }
  global_cloud_.setInputCloud(surround_cloud);
  global_cloud_.filter(*ds_surround_cloud);
  return ds_surround_cloud;
}

laserCloud::Ptr MapManager::getKeyPoses() {
  std::unique_lock<std::mutex> lock(mutex_);
  laserCloud::Ptr pose_cloud(new laserCloud());
  *pose_cloud = *frame_pose_tree_;
  return pose_cloud;
}

laserCloud::Ptr MapManager::getCornerMapCloud() {
  std::unique_lock<std::mutex> lock(mutex_);
  laserCloud::Ptr cloud(new laserCloud());
  *cloud = *all_corner_cloud_;
  return cloud;
}

laserCloud::Ptr MapManager::getSurfMapCloud() {
  std::unique_lock<std::mutex> lock(mutex_);
  laserCloud::Ptr cloud(new laserCloud());
  *cloud = *all_surf_cloud_;
  return cloud;
}

// FIXME:这个函数有bug
int MapManager::getLatestFrameID() {
  std::unique_lock<std::mutex> lock(mutex_);
  if (frame_pose_tree_->points.empty()) {
    return -1;
  }
  return (size_t) frame_pose_tree_->points[frame_pose_tree_->points.size() - 1]
      .intensity;
}

bool MapManager::hasKeyFrame() {
  if (frame_pose_tree_->points.size() > 0) {
    return true;
  } else {
    return false;
  }
}

bool MapManager::activeKeyFrame(const std::string file_name,
                                std::shared_ptr<KeyFrame> ptr_keyframe) {
  if (nullptr == ptr_keyframe) {
    return false;
  }
  std::string corner_file =
      file_name + "corner" + std::to_string(ptr_keyframe->index_) + ".pcd";
  std::string surf_file =
      file_name + "surf" + std::to_string(ptr_keyframe->index_) + ".pcd";
  if ((access(corner_file.c_str(), 0) != 0) ||
      (access(surf_file.c_str(), 0) != 0)) {
    LOG(ERROR) << file_name << " Cloud is not exist!!";
    return false;
  }
  if (pcl::io::loadPCDFile<PointType>(corner_file,
                                      *ptr_keyframe->corner_cloud_) == -1) {
    LOG(ERROR) << "corner cloud is empty";
    return false;
  }
  if (pcl::io::loadPCDFile<PointType>(surf_file, *ptr_keyframe->surf_cloud_) ==
      -1) {
    LOG(ERROR) << "surf_cloud is empty";
    return false;
  }
  ptr_keyframe->setActivedSatus(true);
  return true;
}

bool MapManager::unactiveKeyFrame(const std::string file_name,
                                  std::shared_ptr<KeyFrame> ptr_keyframe) {
  if (nullptr == ptr_keyframe) {
    return false;
  }
  std::string corner_file =
      file_name + "corner" + std::to_string(ptr_keyframe->index_) + ".pcd";
  std::string surf_file =
      file_name + "surf" + std::to_string(ptr_keyframe->index_) + ".pcd";
  if ((access(corner_file.c_str(), 0) == -1) ||
      (access(surf_file.c_str(), 0) == -1)) {
    if (pcl::io::savePCDFileBinaryCompressed<PointType>(
            corner_file, *ptr_keyframe->corner_cloud_) == -1) {
      LOG(ERROR) << "corner cloud save failed";
      return false;
    }
    if (pcl::io::savePCDFileBinaryCompressed<PointType>(
            surf_file, *ptr_keyframe->surf_cloud_) == -1) {
      LOG(ERROR) << "surf_cloud is save failed";
      return false;
    }
  }
  ptr_keyframe->setActivedSatus(false);
  ptr_keyframe->corner_cloud_->clear();
  ptr_keyframe->surf_cloud_->clear();
  return true;
}

std::shared_ptr<KeyFrame> MapManager::getKeyFrameFromID(const size_t index) {
  auto keyframe_iter = keyframe_database_.find(index);
  if (keyframe_iter != keyframe_database_.end()) {
    return keyframe_iter->second;
  }
  return nullptr;
}

std::vector<std::pair<size_t, Mat34d>> MapManager::getKeyFrameDataPose() {
  std::vector<std::pair<size_t, Mat34d>> keyframe_pose_v;
  std::unique_lock<std::mutex> lock(mutex_);
  for (auto kf_iter = keyframe_database_.begin();
       kf_iter != keyframe_database_.end(); kf_iter++) {
    std::shared_ptr<KeyFrame> ptr_keyframe = kf_iter->second;
    std::pair<size_t, Mat34d> keyframe_pose;
    keyframe_pose.first = ptr_keyframe->index_;
    keyframe_pose.second = ptr_keyframe->getPose();
    keyframe_pose_v.push_back(keyframe_pose);
  }
  return keyframe_pose_v;
}

bool MapManager::getPoseFromID(const size_t frame_id, Mat34d &pose34d) {
  auto keyframe_iter = keyframe_database_.find(frame_id);
  if (keyframe_iter != keyframe_database_.end()) {
    std::shared_ptr<KeyFrame> ptr_keyframe = keyframe_iter->second;
    pose34d = ptr_keyframe->getPose();
    return true;
  } else {
    return false;
  }
}

bool MapManager::getOdomPoseFromID(const size_t frame_id, Mat34d &pose34d) {
  auto keyframe_iter = keyframe_database_.find(frame_id);
  if (keyframe_iter != keyframe_database_.end()) {
    std::shared_ptr<KeyFrame> ptr_keyframe = keyframe_iter->second;
    pose34d = ptr_keyframe->getWheelPose();
    return true;
  } else {
    return false;
  }
}

bool MapManager::getPositionFromId(const size_t frame_id, Vec3d &pos) {
  auto keyframe_iter = keyframe_database_.find(frame_id);
  if (keyframe_iter != keyframe_database_.end()) {
    std::shared_ptr<KeyFrame> ptr_keyframe = keyframe_iter->second;
    FramePosition pose = ptr_keyframe->getPosition();

    pos(0) = pose.x;
    pos(1) = pose.y;
    pos(2) = pose.z;

    return true;
  } else {
    LOG(WARNING) << "not find keyframe: " << frame_id;
    return false;
  }
}

}  // namespace cvte_lidar_slam