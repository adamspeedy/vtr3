cmake_minimum_required(VERSION 3.16)
project(vtr_scene_graph_interface)

include("${CMAKE_CURRENT_LIST_DIR}/../vtr_common/vtr_include.cmake")


find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(vtr_navigation_msgs REQUIRED)
find_package(vtr_pose_graph_msgs REQUIRED)
find_package(vtr_common_msgs REQUIRED)
find_package(vtr_common REQUIRED)
find_package(rosbag2_cpp REQUIRED)
find_package(SQLite3 REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(Eigen3 REQUIRED)

add_executable(add_goal src/add_goal.cpp)
add_executable(repeat_command src/repeat_command.cpp)
add_library(utils src/utils.cpp)


ament_target_dependencies(add_goal rclcpp std_msgs vtr_navigation_msgs vtr_pose_graph_msgs)
ament_target_dependencies(repeat_command rclcpp std_msgs vtr_navigation_msgs)

ament_target_dependencies(utils
  rclcpp
  vtr_common
  vtr_pose_graph_msgs
  Eigen3
  rosbag2_cpp
  SQLite3
  yaml-cpp
)

target_include_directories(add_goal PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
target_include_directories(utils PUBLIC
  include/
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)


target_link_libraries(add_goal  utils)


install(
  DIRECTORY include/
  DESTINATION include
)


install(TARGETS
  add_goal repeat_command utils
  DESTINATION lib/${PROJECT_NAME})
# install(
#   TARGETS
#     ${PROJECT_NAME}
#   LIBRARY DESTINATION lib
#   ARCHIVE DESTINATION lib
#   RUNTIME DESTINATION lib/${PROJECT_NAME}
#   INCLUDES DESTINATION include
# )

ament_package()