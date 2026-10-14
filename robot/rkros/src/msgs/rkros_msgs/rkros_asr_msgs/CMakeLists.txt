cmake_minimum_required(VERSION 3.8)
project(rkros_asr_msgs)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

find_package(ament_cmake REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(builtin_interfaces REQUIRED)

# Generate messages
rosidl_generate_interfaces(${PROJECT_NAME}
  "msg/RkAsrResult.msg"
  DEPENDENCIES builtin_interfaces
)

ament_package()