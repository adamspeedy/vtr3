cmake_minimum_required(VERSION 3.8)
project(vtr_vision)

# if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
#   add_compile_options(-Wall -Wextra -Wpedantic)
# endif()



# Common setup for vtr packages
include("${CMAKE_CURRENT_LIST_DIR}/../vtr_common/vtr_include.cmake")


# find dependencies
find_package(ament_cmake REQUIRED)
# uncomment the following section in order to fill in
# further dependencies manually.

find_package(OpenCV REQUIRED)
find_package(cv_bridge REQUIRED)

#find_package(Torch REQUIRED)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

find_package(lgmath REQUIRED)
find_package(steam REQUIRED)

find_package(vtr_common REQUIRED)
find_package(vtr_logging REQUIRED)
find_package(vtr_vision_msgs REQUIRED)
find_package(vtr_common_msgs REQUIRED)
find_package(vtr_storage REQUIRED)
find_package(vtr_tactic REQUIRED)

if(DEFINED VTR_ENABLE_VISION)

find_package(gpusurf REQUIRED) # currently assuming this is required

#file(GLOB_RECURSE SRC src/*.cpp)
# file(GLOB_RECURSE SRC
#   src/visualize.cpp
#   src/utils.cpp
#   src/pipeline.cpp
#   src/image_conversions.cpp
#   src/steam_extensions/*.cpp
#   src/sensors/*.cpp
#   src/outliers/sampler/*.cpp
#   src/outliers/ransac/*.cpp
#   src/modules/ransac/*.cpp
#   src/modules/preprocessing/*.cpp
#   src/modules/optimization/*.cpp
#   src/modules/odometry/*.cpp
#   src/modules/localization/*.cpp
#   src/modules/bundle_adjustment/*.cpp
#   src/messages/*.cpp
#   src/geometry/*.cpp
#   src/features/extractor/*.cpp
#   src/features/matcher/*.cpp
#)


if(NOT DEFINED ITERATION_BUILD_VISION)
  set(ITERATION_BUILD_VISION 11)  # Default value
endif()

if(${ITERATION_BUILD_VISION} EQUAL 1)
  message(STATUS "Iteration 1")
  file(GLOB_RECURSE SRC
  src/visualize.cpp
  src/utils.cpp
  src/pipeline.cpp
  )
endif()

if(${ITERATION_BUILD_VISION} EQUAL 2)
  message(STATUS "Iteration 2")
  file(GLOB_RECURSE SRC
  src/visualize.cpp
  src/utils.cpp
  src/pipeline.cpp
  src/image_conversions.cpp
  src/steam_extensions/*.cpp
  )
endif()


if(${ITERATION_BUILD_VISION} EQUAL 3)
  message(STATUS "Iteration 3")
  file(GLOB_RECURSE SRC
  src/visualize.cpp
  src/utils.cpp
  src/pipeline.cpp
  src/image_conversions.cpp
  src/steam_extensions/*.cpp
  src/sensors/*.cpp
  src/outliers/sampler/*.cpp
  )
endif()
if(${ITERATION_BUILD_VISION} EQUAL 4)
  message(STATUS "Iteration 4")
  file(GLOB_RECURSE SRC
  src/visualize.cpp
  src/utils.cpp
  src/pipeline.cpp
  src/image_conversions.cpp
  src/steam_extensions/*.cpp
  src/sensors/*.cpp
  src/outliers/sampler/*.cpp
  src/outliers/ransac/*.cpp
  src/modules/ransac/*.cpp
  )
endif()
if(${ITERATION_BUILD_VISION} EQUAL 5)
  message(STATUS "Iteration 5")
  file(GLOB_RECURSE SRC
  src/visualize.cpp
  src/utils.cpp
  src/pipeline.cpp
  src/image_conversions.cpp
  src/steam_extensions/*.cpp
  src/sensors/*.cpp
  src/outliers/sampler/*.cpp
  src/outliers/ransac/*.cpp
  src/modules/ransac/*.cpp
  src/modules/preprocessing/*.cpp
  )
endif()
if(${ITERATION_BUILD_VISION} EQUAL 6)
  message(STATUS "Iteration 6")
  file(GLOB_RECURSE SRC
  src/visualize.cpp
  src/utils.cpp
  src/pipeline.cpp
  src/image_conversions.cpp
  src/steam_extensions/*.cpp
  src/sensors/*.cpp
  src/outliers/sampler/*.cpp
  src/outliers/ransac/*.cpp
  src/modules/ransac/*.cpp
  src/modules/preprocessing/*.cpp
  src/modules/optimization/*.cpp
  )
endif()
if(${ITERATION_BUILD_VISION} EQUAL 7)
  message(STATUS "Iteration 7")
  file(GLOB_RECURSE SRC
  src/visualize.cpp
  src/utils.cpp
  src/pipeline.cpp
  src/image_conversions.cpp
  src/steam_extensions/*.cpp
  src/sensors/*.cpp
  src/outliers/sampler/*.cpp
  src/outliers/ransac/*.cpp
  src/modules/ransac/*.cpp
  src/modules/preprocessing/*.cpp
  src/modules/optimization/*.cpp
  src/modules/odometry/*.cpp
  )
endif()

if(${ITERATION_BUILD_VISION} EQUAL 8)
  message(STATUS "Iteration 8")
  file(GLOB_RECURSE SRC
  src/visualize.cpp
  src/utils.cpp
  src/pipeline.cpp
  src/image_conversions.cpp
  src/steam_extensions/*.cpp
  src/sensors/*.cpp
  src/outliers/sampler/*.cpp
  src/outliers/ransac/*.cpp
  src/modules/ransac/*.cpp
  src/modules/preprocessing/*.cpp
  src/modules/optimization/*.cpp
  src/modules/odometry/*.cpp
  src/modules/localization/*.cpp
  )
endif()

if(${ITERATION_BUILD_VISION} EQUAL 9)
  message(STATUS "Iteration 9")
  file(GLOB_RECURSE SRC
  src/visualize.cpp
  src/utils.cpp
  src/pipeline.cpp
  src/image_conversions.cpp
  src/steam_extensions/*.cpp
  src/sensors/*.cpp
  src/outliers/sampler/*.cpp
  src/outliers/ransac/*.cpp
  src/modules/ransac/*.cpp
  src/modules/preprocessing/*.cpp
  src/modules/optimization/*.cpp
  src/modules/odometry/*.cpp
  src/modules/localization/*.cpp
  src/modules/bundle_adjustment/*.cpp
  )
endif()

if(${ITERATION_BUILD_VISION} EQUAL 10)
  message(STATUS "Iteration 10")
  file(GLOB_RECURSE SRC
  src/visualize.cpp
  src/utils.cpp
  src/pipeline.cpp
  src/image_conversions.cpp
  src/steam_extensions/*.cpp
  src/sensors/*.cpp
  src/outliers/sampler/*.cpp
  src/outliers/ransac/*.cpp
  src/modules/ransac/*.cpp
  src/modules/preprocessing/*.cpp
  src/modules/optimization/*.cpp
  src/modules/odometry/*.cpp
  src/modules/localization/*.cpp
  src/modules/bundle_adjustment/*.cpp
  src/messages/*.cpp
  src/geometry/*.cpp
  )
endif() 
if(${ITERATION_BUILD_VISION} EQUAL 11)
  message(STATUS "Iteration 11")
  file(GLOB_RECURSE SRC
  src/visualize.cpp
  src/utils.cpp
  src/pipeline.cpp
  src/image_conversions.cpp
  src/steam_extensions/*.cpp
  src/sensors/*.cpp
  src/outliers/sampler/*.cpp
  src/outliers/ransac/*.cpp
  src/modules/ransac/*.cpp
  src/modules/preprocessing/*.cpp
  src/modules/optimization/*.cpp
  src/modules/odometry/*.cpp
  src/modules/localization/*.cpp
  src/modules/bundle_adjustment/*.cpp
  src/messages/*.cpp
  src/geometry/*.cpp
  src/features/extractor/*.cpp
  src/features/matcher/*.cpp
  )
endif() 

add_library(${PROJECT_NAME} ${SRC})

#target_link_libraries(${PROJECT_NAME} ${TORCH_LIBRARIES})
#set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)

ament_target_dependencies(${PROJECT_NAME}
  OpenCV
  cv_bridge
  gpusurf lgmath steam
  vtr_common vtr_logging vtr_vision_msgs vtr_storage vtr_tactic vtr_common_msgs
)
target_include_directories(${PROJECT_NAME}
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
  #${TORCH_INCLUDE_DIRS})

ament_export_targets(export_${PROJECT_NAME} HAS_LIBRARY_TARGET)
ament_export_dependencies(
  cv_bridge gpusurf lgmath steam
  vtr_common vtr_logging vtr_vision_msgs vtr_storage vtr_tactic vtr_common_msgs
  #Torch
)

#ament_export_include_directories(${TORCH_INCLUDE_DIRS})
#ament_export_libraries(${TORCH_LIBRARIES})


install(
  DIRECTORY include/
  DESTINATION include
)

install(
  TARGETS
    ${PROJECT_NAME}
  EXPORT export_${PROJECT_NAME}
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION lib/${PROJECT_NAME}
  INCLUDES DESTINATION include
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

endif() #VTR_ENABLE_VISION
ament_package()
