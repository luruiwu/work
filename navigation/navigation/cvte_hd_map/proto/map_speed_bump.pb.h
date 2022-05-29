// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: map_speed_bump.proto

#ifndef PROTOBUF_INCLUDED_map_5fspeed_5fbump_2eproto
#define PROTOBUF_INCLUDED_map_5fspeed_5fbump_2eproto

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 3006001
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 3006001 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/inlined_string_field.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/unknown_field_set.h>
#include "map_id.pb.h"
#include "map_geometry.pb.h"
// @@protoc_insertion_point(includes)
#define PROTOBUF_INTERNAL_EXPORT_protobuf_map_5fspeed_5fbump_2eproto 

namespace protobuf_map_5fspeed_5fbump_2eproto {
// Internal implementation detail -- do not use these members.
struct TableStruct {
  static const ::google::protobuf::internal::ParseTableField entries[];
  static const ::google::protobuf::internal::AuxillaryParseTableField aux[];
  static const ::google::protobuf::internal::ParseTable schema[1];
  static const ::google::protobuf::internal::FieldMetadata field_metadata[];
  static const ::google::protobuf::internal::SerializationTable serialization_table[];
  static const ::google::protobuf::uint32 offsets[];
};
void AddDescriptors();
}  // namespace protobuf_map_5fspeed_5fbump_2eproto
namespace cvte {
namespace hdmap {
class SpeedBump;
class SpeedBumpDefaultTypeInternal;
extern SpeedBumpDefaultTypeInternal _SpeedBump_default_instance_;
}  // namespace hdmap
}  // namespace cvte
namespace google {
namespace protobuf {
template<> ::cvte::hdmap::SpeedBump* Arena::CreateMaybeMessage<::cvte::hdmap::SpeedBump>(Arena*);
}  // namespace protobuf
}  // namespace google
namespace cvte {
namespace hdmap {

// ===================================================================

class SpeedBump : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:cvte.hdmap.SpeedBump) */ {
 public:
  SpeedBump();
  virtual ~SpeedBump();

  SpeedBump(const SpeedBump& from);

  inline SpeedBump& operator=(const SpeedBump& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  SpeedBump(SpeedBump&& from) noexcept
    : SpeedBump() {
    *this = ::std::move(from);
  }

  inline SpeedBump& operator=(SpeedBump&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _internal_metadata_.unknown_fields();
  }
  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return _internal_metadata_.mutable_unknown_fields();
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const SpeedBump& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const SpeedBump* internal_default_instance() {
    return reinterpret_cast<const SpeedBump*>(
               &_SpeedBump_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  void Swap(SpeedBump* other);
  friend void swap(SpeedBump& a, SpeedBump& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline SpeedBump* New() const final {
    return CreateMaybeMessage<SpeedBump>(NULL);
  }

  SpeedBump* New(::google::protobuf::Arena* arena) const final {
    return CreateMaybeMessage<SpeedBump>(arena);
  }
  void CopyFrom(const ::google::protobuf::Message& from) final;
  void MergeFrom(const ::google::protobuf::Message& from) final;
  void CopyFrom(const SpeedBump& from);
  void MergeFrom(const SpeedBump& from);
  void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) final;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const final;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(SpeedBump* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // repeated .cvte.hdmap.Id overlap_id = 7;
  int overlap_id_size() const;
  void clear_overlap_id();
  static const int kOverlapIdFieldNumber = 7;
  ::cvte::hdmap::Id* mutable_overlap_id(int index);
  ::google::protobuf::RepeatedPtrField< ::cvte::hdmap::Id >*
      mutable_overlap_id();
  const ::cvte::hdmap::Id& overlap_id(int index) const;
  ::cvte::hdmap::Id* add_overlap_id();
  const ::google::protobuf::RepeatedPtrField< ::cvte::hdmap::Id >&
      overlap_id() const;

  // repeated .cvte.hdmap.Curve position = 8;
  int position_size() const;
  void clear_position();
  static const int kPositionFieldNumber = 8;
  ::cvte::hdmap::Curve* mutable_position(int index);
  ::google::protobuf::RepeatedPtrField< ::cvte::hdmap::Curve >*
      mutable_position();
  const ::cvte::hdmap::Curve& position(int index) const;
  ::cvte::hdmap::Curve* add_position();
  const ::google::protobuf::RepeatedPtrField< ::cvte::hdmap::Curve >&
      position() const;

  // optional string material = 4;
  bool has_material() const;
  void clear_material();
  static const int kMaterialFieldNumber = 4;
  const ::std::string& material() const;
  void set_material(const ::std::string& value);
  #if LANG_CXX11
  void set_material(::std::string&& value);
  #endif
  void set_material(const char* value);
  void set_material(const char* value, size_t size);
  ::std::string* mutable_material();
  ::std::string* release_material();
  void set_allocated_material(::std::string* material);

  // optional string color = 5;
  bool has_color() const;
  void clear_color();
  static const int kColorFieldNumber = 5;
  const ::std::string& color() const;
  void set_color(const ::std::string& value);
  #if LANG_CXX11
  void set_color(::std::string&& value);
  #endif
  void set_color(const char* value);
  void set_color(const char* value, size_t size);
  ::std::string* mutable_color();
  ::std::string* release_color();
  void set_allocated_color(::std::string* color);

  // optional string static_area = 6;
  bool has_static_area() const;
  void clear_static_area();
  static const int kStaticAreaFieldNumber = 6;
  const ::std::string& static_area() const;
  void set_static_area(const ::std::string& value);
  #if LANG_CXX11
  void set_static_area(::std::string&& value);
  #endif
  void set_static_area(const char* value);
  void set_static_area(const char* value, size_t size);
  ::std::string* mutable_static_area();
  ::std::string* release_static_area();
  void set_allocated_static_area(::std::string* static_area);

  // optional .cvte.hdmap.Id id = 1;
  bool has_id() const;
  void clear_id();
  static const int kIdFieldNumber = 1;
  private:
  const ::cvte::hdmap::Id& _internal_id() const;
  public:
  const ::cvte::hdmap::Id& id() const;
  ::cvte::hdmap::Id* release_id();
  ::cvte::hdmap::Id* mutable_id();
  void set_allocated_id(::cvte::hdmap::Id* id);

  // optional .cvte.hdmap.Polygon polygon = 2;
  bool has_polygon() const;
  void clear_polygon();
  static const int kPolygonFieldNumber = 2;
  private:
  const ::cvte::hdmap::Polygon& _internal_polygon() const;
  public:
  const ::cvte::hdmap::Polygon& polygon() const;
  ::cvte::hdmap::Polygon* release_polygon();
  ::cvte::hdmap::Polygon* mutable_polygon();
  void set_allocated_polygon(::cvte::hdmap::Polygon* polygon);

  // optional double speed = 3;
  bool has_speed() const;
  void clear_speed();
  static const int kSpeedFieldNumber = 3;
  double speed() const;
  void set_speed(double value);

  // @@protoc_insertion_point(class_scope:cvte.hdmap.SpeedBump)
 private:
  void set_has_id();
  void clear_has_id();
  void set_has_polygon();
  void clear_has_polygon();
  void set_has_speed();
  void clear_has_speed();
  void set_has_material();
  void clear_has_material();
  void set_has_color();
  void clear_has_color();
  void set_has_static_area();
  void clear_has_static_area();

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::internal::HasBits<1> _has_bits_;
  mutable ::google::protobuf::internal::CachedSize _cached_size_;
  ::google::protobuf::RepeatedPtrField< ::cvte::hdmap::Id > overlap_id_;
  ::google::protobuf::RepeatedPtrField< ::cvte::hdmap::Curve > position_;
  ::google::protobuf::internal::ArenaStringPtr material_;
  ::google::protobuf::internal::ArenaStringPtr color_;
  ::google::protobuf::internal::ArenaStringPtr static_area_;
  ::cvte::hdmap::Id* id_;
  ::cvte::hdmap::Polygon* polygon_;
  double speed_;
  friend struct ::protobuf_map_5fspeed_5fbump_2eproto::TableStruct;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// SpeedBump

// optional .cvte.hdmap.Id id = 1;
inline bool SpeedBump::has_id() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void SpeedBump::set_has_id() {
  _has_bits_[0] |= 0x00000008u;
}
inline void SpeedBump::clear_has_id() {
  _has_bits_[0] &= ~0x00000008u;
}
inline const ::cvte::hdmap::Id& SpeedBump::_internal_id() const {
  return *id_;
}
inline const ::cvte::hdmap::Id& SpeedBump::id() const {
  const ::cvte::hdmap::Id* p = id_;
  // @@protoc_insertion_point(field_get:cvte.hdmap.SpeedBump.id)
  return p != NULL ? *p : *reinterpret_cast<const ::cvte::hdmap::Id*>(
      &::cvte::hdmap::_Id_default_instance_);
}
inline ::cvte::hdmap::Id* SpeedBump::release_id() {
  // @@protoc_insertion_point(field_release:cvte.hdmap.SpeedBump.id)
  clear_has_id();
  ::cvte::hdmap::Id* temp = id_;
  id_ = NULL;
  return temp;
}
inline ::cvte::hdmap::Id* SpeedBump::mutable_id() {
  set_has_id();
  if (id_ == NULL) {
    auto* p = CreateMaybeMessage<::cvte::hdmap::Id>(GetArenaNoVirtual());
    id_ = p;
  }
  // @@protoc_insertion_point(field_mutable:cvte.hdmap.SpeedBump.id)
  return id_;
}
inline void SpeedBump::set_allocated_id(::cvte::hdmap::Id* id) {
  ::google::protobuf::Arena* message_arena = GetArenaNoVirtual();
  if (message_arena == NULL) {
    delete reinterpret_cast< ::google::protobuf::MessageLite*>(id_);
  }
  if (id) {
    ::google::protobuf::Arena* submessage_arena = NULL;
    if (message_arena != submessage_arena) {
      id = ::google::protobuf::internal::GetOwnedMessage(
          message_arena, id, submessage_arena);
    }
    set_has_id();
  } else {
    clear_has_id();
  }
  id_ = id;
  // @@protoc_insertion_point(field_set_allocated:cvte.hdmap.SpeedBump.id)
}

// optional .cvte.hdmap.Polygon polygon = 2;
inline bool SpeedBump::has_polygon() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}
inline void SpeedBump::set_has_polygon() {
  _has_bits_[0] |= 0x00000010u;
}
inline void SpeedBump::clear_has_polygon() {
  _has_bits_[0] &= ~0x00000010u;
}
inline const ::cvte::hdmap::Polygon& SpeedBump::_internal_polygon() const {
  return *polygon_;
}
inline const ::cvte::hdmap::Polygon& SpeedBump::polygon() const {
  const ::cvte::hdmap::Polygon* p = polygon_;
  // @@protoc_insertion_point(field_get:cvte.hdmap.SpeedBump.polygon)
  return p != NULL ? *p : *reinterpret_cast<const ::cvte::hdmap::Polygon*>(
      &::cvte::hdmap::_Polygon_default_instance_);
}
inline ::cvte::hdmap::Polygon* SpeedBump::release_polygon() {
  // @@protoc_insertion_point(field_release:cvte.hdmap.SpeedBump.polygon)
  clear_has_polygon();
  ::cvte::hdmap::Polygon* temp = polygon_;
  polygon_ = NULL;
  return temp;
}
inline ::cvte::hdmap::Polygon* SpeedBump::mutable_polygon() {
  set_has_polygon();
  if (polygon_ == NULL) {
    auto* p = CreateMaybeMessage<::cvte::hdmap::Polygon>(GetArenaNoVirtual());
    polygon_ = p;
  }
  // @@protoc_insertion_point(field_mutable:cvte.hdmap.SpeedBump.polygon)
  return polygon_;
}
inline void SpeedBump::set_allocated_polygon(::cvte::hdmap::Polygon* polygon) {
  ::google::protobuf::Arena* message_arena = GetArenaNoVirtual();
  if (message_arena == NULL) {
    delete reinterpret_cast< ::google::protobuf::MessageLite*>(polygon_);
  }
  if (polygon) {
    ::google::protobuf::Arena* submessage_arena = NULL;
    if (message_arena != submessage_arena) {
      polygon = ::google::protobuf::internal::GetOwnedMessage(
          message_arena, polygon, submessage_arena);
    }
    set_has_polygon();
  } else {
    clear_has_polygon();
  }
  polygon_ = polygon;
  // @@protoc_insertion_point(field_set_allocated:cvte.hdmap.SpeedBump.polygon)
}

// optional double speed = 3;
inline bool SpeedBump::has_speed() const {
  return (_has_bits_[0] & 0x00000020u) != 0;
}
inline void SpeedBump::set_has_speed() {
  _has_bits_[0] |= 0x00000020u;
}
inline void SpeedBump::clear_has_speed() {
  _has_bits_[0] &= ~0x00000020u;
}
inline void SpeedBump::clear_speed() {
  speed_ = 0;
  clear_has_speed();
}
inline double SpeedBump::speed() const {
  // @@protoc_insertion_point(field_get:cvte.hdmap.SpeedBump.speed)
  return speed_;
}
inline void SpeedBump::set_speed(double value) {
  set_has_speed();
  speed_ = value;
  // @@protoc_insertion_point(field_set:cvte.hdmap.SpeedBump.speed)
}

// optional string material = 4;
inline bool SpeedBump::has_material() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void SpeedBump::set_has_material() {
  _has_bits_[0] |= 0x00000001u;
}
inline void SpeedBump::clear_has_material() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void SpeedBump::clear_material() {
  material_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  clear_has_material();
}
inline const ::std::string& SpeedBump::material() const {
  // @@protoc_insertion_point(field_get:cvte.hdmap.SpeedBump.material)
  return material_.GetNoArena();
}
inline void SpeedBump::set_material(const ::std::string& value) {
  set_has_material();
  material_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:cvte.hdmap.SpeedBump.material)
}
#if LANG_CXX11
inline void SpeedBump::set_material(::std::string&& value) {
  set_has_material();
  material_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:cvte.hdmap.SpeedBump.material)
}
#endif
inline void SpeedBump::set_material(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  set_has_material();
  material_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:cvte.hdmap.SpeedBump.material)
}
inline void SpeedBump::set_material(const char* value, size_t size) {
  set_has_material();
  material_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:cvte.hdmap.SpeedBump.material)
}
inline ::std::string* SpeedBump::mutable_material() {
  set_has_material();
  // @@protoc_insertion_point(field_mutable:cvte.hdmap.SpeedBump.material)
  return material_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* SpeedBump::release_material() {
  // @@protoc_insertion_point(field_release:cvte.hdmap.SpeedBump.material)
  if (!has_material()) {
    return NULL;
  }
  clear_has_material();
  return material_.ReleaseNonDefaultNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void SpeedBump::set_allocated_material(::std::string* material) {
  if (material != NULL) {
    set_has_material();
  } else {
    clear_has_material();
  }
  material_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), material);
  // @@protoc_insertion_point(field_set_allocated:cvte.hdmap.SpeedBump.material)
}

// optional string color = 5;
inline bool SpeedBump::has_color() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void SpeedBump::set_has_color() {
  _has_bits_[0] |= 0x00000002u;
}
inline void SpeedBump::clear_has_color() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void SpeedBump::clear_color() {
  color_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  clear_has_color();
}
inline const ::std::string& SpeedBump::color() const {
  // @@protoc_insertion_point(field_get:cvte.hdmap.SpeedBump.color)
  return color_.GetNoArena();
}
inline void SpeedBump::set_color(const ::std::string& value) {
  set_has_color();
  color_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:cvte.hdmap.SpeedBump.color)
}
#if LANG_CXX11
inline void SpeedBump::set_color(::std::string&& value) {
  set_has_color();
  color_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:cvte.hdmap.SpeedBump.color)
}
#endif
inline void SpeedBump::set_color(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  set_has_color();
  color_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:cvte.hdmap.SpeedBump.color)
}
inline void SpeedBump::set_color(const char* value, size_t size) {
  set_has_color();
  color_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:cvte.hdmap.SpeedBump.color)
}
inline ::std::string* SpeedBump::mutable_color() {
  set_has_color();
  // @@protoc_insertion_point(field_mutable:cvte.hdmap.SpeedBump.color)
  return color_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* SpeedBump::release_color() {
  // @@protoc_insertion_point(field_release:cvte.hdmap.SpeedBump.color)
  if (!has_color()) {
    return NULL;
  }
  clear_has_color();
  return color_.ReleaseNonDefaultNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void SpeedBump::set_allocated_color(::std::string* color) {
  if (color != NULL) {
    set_has_color();
  } else {
    clear_has_color();
  }
  color_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), color);
  // @@protoc_insertion_point(field_set_allocated:cvte.hdmap.SpeedBump.color)
}

// optional string static_area = 6;
inline bool SpeedBump::has_static_area() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void SpeedBump::set_has_static_area() {
  _has_bits_[0] |= 0x00000004u;
}
inline void SpeedBump::clear_has_static_area() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void SpeedBump::clear_static_area() {
  static_area_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  clear_has_static_area();
}
inline const ::std::string& SpeedBump::static_area() const {
  // @@protoc_insertion_point(field_get:cvte.hdmap.SpeedBump.static_area)
  return static_area_.GetNoArena();
}
inline void SpeedBump::set_static_area(const ::std::string& value) {
  set_has_static_area();
  static_area_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:cvte.hdmap.SpeedBump.static_area)
}
#if LANG_CXX11
inline void SpeedBump::set_static_area(::std::string&& value) {
  set_has_static_area();
  static_area_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:cvte.hdmap.SpeedBump.static_area)
}
#endif
inline void SpeedBump::set_static_area(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  set_has_static_area();
  static_area_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:cvte.hdmap.SpeedBump.static_area)
}
inline void SpeedBump::set_static_area(const char* value, size_t size) {
  set_has_static_area();
  static_area_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:cvte.hdmap.SpeedBump.static_area)
}
inline ::std::string* SpeedBump::mutable_static_area() {
  set_has_static_area();
  // @@protoc_insertion_point(field_mutable:cvte.hdmap.SpeedBump.static_area)
  return static_area_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* SpeedBump::release_static_area() {
  // @@protoc_insertion_point(field_release:cvte.hdmap.SpeedBump.static_area)
  if (!has_static_area()) {
    return NULL;
  }
  clear_has_static_area();
  return static_area_.ReleaseNonDefaultNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void SpeedBump::set_allocated_static_area(::std::string* static_area) {
  if (static_area != NULL) {
    set_has_static_area();
  } else {
    clear_has_static_area();
  }
  static_area_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), static_area);
  // @@protoc_insertion_point(field_set_allocated:cvte.hdmap.SpeedBump.static_area)
}

// repeated .cvte.hdmap.Id overlap_id = 7;
inline int SpeedBump::overlap_id_size() const {
  return overlap_id_.size();
}
inline ::cvte::hdmap::Id* SpeedBump::mutable_overlap_id(int index) {
  // @@protoc_insertion_point(field_mutable:cvte.hdmap.SpeedBump.overlap_id)
  return overlap_id_.Mutable(index);
}
inline ::google::protobuf::RepeatedPtrField< ::cvte::hdmap::Id >*
SpeedBump::mutable_overlap_id() {
  // @@protoc_insertion_point(field_mutable_list:cvte.hdmap.SpeedBump.overlap_id)
  return &overlap_id_;
}
inline const ::cvte::hdmap::Id& SpeedBump::overlap_id(int index) const {
  // @@protoc_insertion_point(field_get:cvte.hdmap.SpeedBump.overlap_id)
  return overlap_id_.Get(index);
}
inline ::cvte::hdmap::Id* SpeedBump::add_overlap_id() {
  // @@protoc_insertion_point(field_add:cvte.hdmap.SpeedBump.overlap_id)
  return overlap_id_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::cvte::hdmap::Id >&
SpeedBump::overlap_id() const {
  // @@protoc_insertion_point(field_list:cvte.hdmap.SpeedBump.overlap_id)
  return overlap_id_;
}

// repeated .cvte.hdmap.Curve position = 8;
inline int SpeedBump::position_size() const {
  return position_.size();
}
inline ::cvte::hdmap::Curve* SpeedBump::mutable_position(int index) {
  // @@protoc_insertion_point(field_mutable:cvte.hdmap.SpeedBump.position)
  return position_.Mutable(index);
}
inline ::google::protobuf::RepeatedPtrField< ::cvte::hdmap::Curve >*
SpeedBump::mutable_position() {
  // @@protoc_insertion_point(field_mutable_list:cvte.hdmap.SpeedBump.position)
  return &position_;
}
inline const ::cvte::hdmap::Curve& SpeedBump::position(int index) const {
  // @@protoc_insertion_point(field_get:cvte.hdmap.SpeedBump.position)
  return position_.Get(index);
}
inline ::cvte::hdmap::Curve* SpeedBump::add_position() {
  // @@protoc_insertion_point(field_add:cvte.hdmap.SpeedBump.position)
  return position_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::cvte::hdmap::Curve >&
SpeedBump::position() const {
  // @@protoc_insertion_point(field_list:cvte.hdmap.SpeedBump.position)
  return position_;
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)

}  // namespace hdmap
}  // namespace cvte

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_INCLUDED_map_5fspeed_5fbump_2eproto