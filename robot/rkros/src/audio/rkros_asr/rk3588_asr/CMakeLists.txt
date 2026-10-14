cmake_minimum_required(VERSION 3.8)
project(rkros_asr)

# Define RockX SDK version
set(ROCKX_SDK_VERSION "V1.0.0_20250621")

# Check for ROS2
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

# Set system name, consistent with original build-linux.sh
if(NOT CMAKE_SYSTEM_NAME)
    set(CMAKE_SYSTEM_NAME Linux)
endif()

# Find dependency packages
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(rkros_audio_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(rkros_asr_msgs REQUIRED)

# Include header directories
include_directories(include)

# Include RockX SDK
set(RockX2_DIR ${CMAKE_CURRENT_SOURCE_DIR}/RK3588_ROCKX2_SDK_${ROCKX_SDK_VERSION}/lib/${CMAKE_SYSTEM_NAME})
find_package(RockX2 REQUIRED)

# Include RockASR headers
set(RockASR_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/lib/${CMAKE_SYSTEM_NAME}/include)
# Add RockX SDK header paths
include_directories(
  ${RockX2_INCLUDE_DIRS}
  ${RockASR_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}/RK3588_ROCKX2_SDK_${ROCKX_SDK_VERSION}/lib/${CMAKE_SYSTEM_NAME}/include
)

# Define data path (relative to package directory)
set(DATA_PATH ${CMAKE_CURRENT_SOURCE_DIR}/data)



# Link library files
# Check target architecture, consistent with original build script
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set (TARGET_LIB_ARCH aarch64)
else()
    set (TARGET_LIB_ARCH armhf)
endif()

file(GLOB RockASR_LIBS ${CMAKE_CURRENT_SOURCE_DIR}/lib/${CMAKE_SYSTEM_NAME}/${TARGET_LIB_ARCH}/*.so)
file(GLOB RockX2_LIBS ${CMAKE_CURRENT_SOURCE_DIR}/RK3588_ROCKX2_SDK_${ROCKX_SDK_VERSION}/lib/${CMAKE_SYSTEM_NAME}/${TARGET_LIB_ARCH}/*.so)

# Add executable files (using different names to avoid conflicts)
add_executable(rk_asr_node
  src/rk_asr_node.cpp
)

add_executable(rk_asr_result_subscriber
  src/rk_asr_result_subscriber.cpp
)

# Use target-oriented cmake syntax to set compiler and linker options
target_compile_features(rk_asr_node PUBLIC c_std_99 cxx_std_14)
target_compile_options(rk_asr_node PRIVATE -Wall -Wextra)

# Link libraries
ament_target_dependencies(rk_asr_node
  rclcpp
  std_msgs
  ament_index_cpp
  geometry_msgs
  rkros_audio_msgs
  rkros_asr_msgs
)

ament_target_dependencies(rk_asr_result_subscriber
  rclcpp
  std_msgs
  rkros_asr_msgs
)

# Link RockX libraries
target_link_libraries(rk_asr_node
  ${RockASR_LIBS}
  ${RockX2_LIBS}
)

# Install executable files
install(TARGETS
  rk_asr_node
  rk_asr_result_subscriber
  DESTINATION lib/${PROJECT_NAME}
)

# Install configuration files
install(FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/rockx_auth_config.json
  DESTINATION share/${PROJECT_NAME}/config
)

# Install RockX SDK library files to standard lib directory
install(DIRECTORY
  ${CMAKE_CURRENT_SOURCE_DIR}/RK3588_ROCKX2_SDK_${ROCKX_SDK_VERSION}/lib/${CMAKE_SYSTEM_NAME}/${TARGET_LIB_ARCH}/
  DESTINATION lib
  FILES_MATCHING PATTERN "*.so*"
)

# Install RockASR library files to standard lib directory
install(DIRECTORY
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/${CMAKE_SYSTEM_NAME}/${TARGET_LIB_ARCH}/
  DESTINATION lib
  FILES_MATCHING PATTERN "*.so*"
)

ament_package()
