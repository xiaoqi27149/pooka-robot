cmake_minimum_required(VERSION 3.8)
project(rkros_rtsp_demo)

# set c++ version
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find depends
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(rkros_rockit REQUIRED)
find_package(rclcpp_components REQUIRED)

set(RTSP_LIBRARY ${CMAKE_CURRENT_SOURCE_DIR}/lib/librtsp.a)

# depends list
set(COMMON_DEPENDS
  rclcpp
  rclcpp_components
  sensor_msgs
  rkros_rockit
)

add_executable(video_rtsp
  src/rkros_rtsp_demo.cpp
  src/rockit_routeline.cpp)

target_include_directories(video_rtsp PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# link RTSP lib
target_link_libraries(video_rtsp ${RTSP_LIBRARY})

ament_target_dependencies(video_rtsp ${COMMON_DEPENDS})

add_executable(h264_recorder
  src/rkros_h264_recorder.cpp)

target_include_directories(h264_recorder PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)

ament_target_dependencies(h264_recorder
  rclcpp
  sensor_msgs
)

# install
install(TARGETS
  video_rtsp
  h264_recorder
  DESTINATION lib/${PROJECT_NAME}
)

# install launch file
install(DIRECTORY
  launch
  DESTINATION share/${PROJECT_NAME}
)

# install include
install(DIRECTORY include/
  DESTINATION include/
)

# install lib
#install(DIRECTORY lib/
#  DESTINATION lib/
#)

# test
if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
