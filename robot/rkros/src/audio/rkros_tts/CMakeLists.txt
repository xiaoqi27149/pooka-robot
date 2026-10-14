cmake_minimum_required(VERSION 3.8)
project(rkros_tts)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(rkros_audio_msgs REQUIRED)

# Include directories
include_directories(
  include
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link directories
link_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/lib
)

# Add executable for TTS node
add_executable(rktts_node src/rktts_node.cpp)

# Link libraries - using the installed library location
find_library(RKTTS_LIB
  NAMES librktts.so
  PATHS ${CMAKE_CURRENT_SOURCE_DIR}/lib/Linux/aarch64
  NO_DEFAULT_PATH
)

find_library(RKNNRT_LIB
  NAMES librknnrt.so
  PATHS ${CMAKE_CURRENT_SOURCE_DIR}/lib/Linux/aarch64
  NO_DEFAULT_PATH
)

# Link libraries
target_link_libraries(rktts_node
  ${RKTTS_LIB}
  ${RKNNRT_LIB}
)

# Add include directories to target
target_include_directories(rktts_node PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)

# Link packages
ament_target_dependencies(rktts_node
  rclcpp
  std_msgs
  rkros_audio_msgs
)

# Install executable
install(TARGETS
  rktts_node
  DESTINATION lib/${PROJECT_NAME}
)

# Install other directories
install(DIRECTORY
  include/
  DESTINATION include/${PROJECT_NAME}
)

install(DIRECTORY
  lib/Linux/aarch64/
  DESTINATION lib
  FILES_MATCHING PATTERN "*.so"
)

# Install model files based on TARGET_SOC
#set(TARGET_SOC "rk3588" CACHE STRING "Target SoC for model files")
#install(FILES
#  models/${TARGET_SOC}/am_part1.data
#  models/${TARGET_SOC}/am_part2.data
#  models/${TARGET_SOC}/voc.data
#  DESTINATION share/${PROJECT_NAME}/models
#)

install(DIRECTORY
  frontend_extras/
  DESTINATION share/${PROJECT_NAME}/frontend_extras
)

# Install the environment hook
if(NOT WIN32)
  # Create the environment hook
  set(ENV_HOOK_FILE ${CMAKE_CURRENT_BINARY_DIR}/rkros_tts_env_hook.sh)
  file(WRITE ${ENV_HOOK_FILE}
    "export LD_LIBRARY_PATH=\${AMENT_CURRENT_PREFIX}/lib:\${LD_LIBRARY_PATH}\n")

  # Install the environment hook
  ament_environment_hooks(
    "${CMAKE_CURRENT_BINARY_DIR}/rkros_tts_env_hook.sh"
  )
endif()

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

ament_package()
