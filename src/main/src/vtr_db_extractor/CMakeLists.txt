cmake_minimum_required(VERSION 3.17)
project(vtr_db_extractor LANGUAGES CXX CUDA)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include("${CMAKE_CURRENT_LIST_DIR}/../vtr_common/vtr_include.cmake")

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)

find_package(Boost REQUIRED COMPONENTS system thread chrono timer)


find_package(Eigen3 REQUIRED)
find_package(vtr_common REQUIRED)

find_package(vtr_common_msgs REQUIRED)
find_package(vtr_tactic_msgs REQUIRED)
find_package(vtr_pose_graph_msgs REQUIRED)
find_package(vtr_vision_msgs)
find_package(SQLite3 REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(rosbag2_cpp REQUIRED)
find_package(OpenCV REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(geometry_msgs REQUIRED)


#message(STATUS "vtr_common include dirs: ${vtr_common_INCLUDE_DIRS}")
add_executable(lie_converter src/convert_lie_from_file.cpp)
add_executable(graph_odom_extractor src/extract_odom_from_bag.cpp)
add_executable(graph_vertex_extractor src/extract_vertex_from_bag.cpp)
add_executable(graph_image_extractor src/extract_image_from_bag.cpp)
add_executable(graph_loc_offset_extractor src/extract_loc_result_from_bag.cpp)
add_executable(graph_loc_status_extractor src/extract_loc_status_from_bag.cpp)
add_executable(graph_edge_extractor src/extract_edge_from_bag.cpp)
add_executable(graph_landmark_extractor src/extract_landmarks_from_bag.cpp)
add_executable(graph_descriptor_extractor src/extract_descriptors_from_bag.cpp)
add_executable(graph_keypoint_extractor src/extract_keypoints_from_bag.cpp)
add_executable(graph_landmark_matches_extractor src/extract_matches_from_bag.cpp)
add_library(utils src/utils.cpp)

ament_target_dependencies(lie_converter
  rclcpp
  vtr_common
  vtr_tactic_msgs
  Eigen3
)

ament_target_dependencies(graph_odom_extractor
  rclcpp
  vtr_common
  vtr_tactic_msgs
  Eigen3
  rosbag2_cpp
  SQLite3
  yaml-cpp
)

ament_target_dependencies(graph_vertex_extractor
  rclcpp
  vtr_common
  vtr_pose_graph_msgs
  Eigen3
  rosbag2_cpp
  SQLite3
  yaml-cpp
)


ament_target_dependencies(graph_image_extractor
  rclcpp
  vtr_common
  vtr_vision_msgs
  rosbag2_cpp
  SQLite3
  yaml-cpp
  OpenCV
  cv_bridge
)

ament_target_dependencies(graph_loc_offset_extractor
  rclcpp
  vtr_common
  vtr_tactic_msgs
  Eigen3
  rosbag2_cpp
  SQLite3
  yaml-cpp
)

ament_target_dependencies(graph_loc_status_extractor
  rclcpp
  vtr_common
  vtr_tactic_msgs
  vtr_vision_msgs
  Eigen3
  rosbag2_cpp
  SQLite3
  yaml-cpp
)

ament_target_dependencies(graph_edge_extractor
  rclcpp
  vtr_common
  vtr_tactic_msgs
  Eigen3
  rosbag2_cpp
  SQLite3
  yaml-cpp
  vtr_pose_graph_msgs
)

ament_target_dependencies(graph_landmark_extractor
  rclcpp
  vtr_common
  vtr_vision_msgs
  Eigen3
  rosbag2_cpp
  SQLite3
  yaml-cpp
)

ament_target_dependencies(graph_descriptor_extractor
  rclcpp
  vtr_common
  vtr_vision_msgs
  Eigen3
  rosbag2_cpp
  SQLite3
  yaml-cpp
)

ament_target_dependencies(graph_keypoint_extractor
  rclcpp
  vtr_common
  vtr_vision_msgs
  Eigen3
  rosbag2_cpp
  SQLite3
  yaml-cpp
  geometry_msgs
)

ament_target_dependencies(graph_landmark_matches_extractor
  rclcpp
  vtr_common
  vtr_vision_msgs
  Eigen3
  rosbag2_cpp
  SQLite3
  yaml-cpp
  geometry_msgs
)

ament_target_dependencies(utils
  rclcpp
  vtr_common
  vtr_vision_msgs
  vtr_tactic_msgs
  vtr_pose_graph_msgs
  Eigen3
  rosbag2_cpp
  SQLite3
  yaml-cpp
  OpenCV
  cv_bridge
)

target_include_directories(lie_converter PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

target_include_directories(graph_odom_extractor PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

target_include_directories(graph_vertex_extractor PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

target_include_directories(graph_image_extractor PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

target_include_directories(graph_loc_status_extractor PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

target_include_directories(graph_loc_offset_extractor PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

target_include_directories(graph_edge_extractor PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

target_include_directories(graph_landmark_extractor PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

target_include_directories(graph_descriptor_extractor PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

target_include_directories(graph_keypoint_extractor PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

target_include_directories(graph_landmark_matches_extractor PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

target_include_directories(utils PUBLIC
  include/
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

target_link_libraries(graph_landmark_extractor  utils)
target_link_libraries(graph_descriptor_extractor  utils)
target_link_libraries(graph_keypoint_extractor  utils)
target_link_libraries(graph_edge_extractor  utils)
target_link_libraries(graph_odom_extractor  utils)
target_link_libraries(graph_vertex_extractor  utils)
target_link_libraries(graph_loc_offset_extractor  utils)
target_link_libraries(graph_loc_status_extractor  utils)
target_link_libraries(graph_image_extractor  utils)
target_link_libraries(graph_landmark_matches_extractor  utils)
# target_link_libraries(lie_converter  vtr_common vtr_common_msgs lgmath)
# target_include_directories(lie_converter  PUBLIC  ${vtr_common_INCLUDE_DIRS}
#   ${vtr_common_msgs_INCLUDE_DIRS}
#   ${lgmath_INCLUDE_DIRS}
# )

install(
  DIRECTORY include/
  DESTINATION include
)

install(
  TARGETS utils lie_converter graph_odom_extractor graph_vertex_extractor graph_image_extractor 
  graph_loc_offset_extractor graph_edge_extractor graph_landmark_extractor graph_keypoint_extractor 
  graph_descriptor_extractor graph_loc_status_extractor graph_landmark_matches_extractor
  EXPORT export_${PROJECT_NAME}
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION lib/${PROJECT_NAME}
  INCLUDES DESTINATION include
  )

ament_package()