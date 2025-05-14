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
find_package(SQLite3 REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(rosbag2_cpp REQUIRED)


#message(STATUS "vtr_common include dirs: ${vtr_common_INCLUDE_DIRS}")
add_executable(lie_converter src/convert_lie_from_file.cpp)
add_executable(bag_extractor src/convert_lie_from_bag.cpp)

ament_target_dependencies(lie_converter
  rclcpp
  vtr_common
  vtr_tactic_msgs
  Eigen3
)

ament_target_dependencies(bag_extractor
  rclcpp
  vtr_common
  vtr_tactic_msgs
  Eigen3
  rosbag2_cpp
  SQLite3
  yaml-cpp
)

target_include_directories(lie_converter PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

target_include_directories(bag_extractor PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)


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
  TARGETS lie_converter bag_extractor
  EXPORT export_${PROJECT_NAME}
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION lib/${PROJECT_NAME}
  INCLUDES DESTINATION include
  )

ament_package()