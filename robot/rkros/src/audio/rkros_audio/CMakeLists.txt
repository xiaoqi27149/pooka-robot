cmake_minimum_required(VERSION 3.8)
project(rkros_audio)

# Set default C/C++ standards if not already defined
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

# Find required packages
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclpy REQUIRED)
find_package(std_msgs REQUIRED)
find_package(rkros_audio_msgs REQUIRED)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

# Find required system libraries
find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)

# Check for required libraries - ALSA, SWRESAMPLE, and AVUTIL are required
pkg_check_modules(ALSA REQUIRED alsa)
pkg_check_modules(SWRESAMPLE REQUIRED libswresample)
pkg_check_modules(AVUTIL REQUIRED libavutil)

# Add para and lib library paths to library search path
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/para ${CMAKE_CURRENT_SOURCE_DIR}/lib)

# Both ALSA and SWRESAMPLE are required - no conditional checks needed
include_directories(${ALSA_INCLUDE_DIRS})
include_directories(${SWRESAMPLE_INCLUDE_DIRS})

# Audio publisher node - publishes audio data
add_executable(audio_publisher src/audio_publisher.cpp)
ament_target_dependencies(audio_publisher rclcpp std_msgs rkros_audio_msgs)
target_link_libraries(audio_publisher ${ALSA_LIBRARIES})

# Audio subscriber node - subscribes to audio data and plays it
add_executable(audio_subscriber src/audio_subscriber.cpp)
ament_target_dependencies(audio_subscriber rclcpp std_msgs rkros_audio_msgs)
target_link_libraries(audio_subscriber
  ${ALSA_LIBRARIES}
  ${SWRESAMPLE_LIBRARIES}
  ${AVUTIL_LIBRARIES})

# Audio RK processor node - processes audio with Rockchip library integration
add_executable(audio_rk_processor src/audio_rk_processor.cpp)
ament_target_dependencies(audio_rk_processor rclcpp std_msgs rkros_audio_msgs ament_index_cpp)
target_include_directories(audio_rk_processor PRIVATE include)

# Link against the Rockchip library during build
target_link_libraries(audio_rk_processor
  ${ALSA_LIBRARIES}
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/librk_audio_hci.so
  Threads::Threads)

# Install executable binaries
install(TARGETS audio_publisher audio_subscriber audio_rk_processor
        DESTINATION lib/${PROJECT_NAME})

# Install Rockchip library file to the lib directory where libraries are expected
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/lib/librk_audio_hci.so
        DESTINATION lib)

# Install para directory contents (excluding the library file which is installed separately) to share directory
install(DIRECTORY para/
        DESTINATION share/${PROJECT_NAME}/para)

# Launch files for different configurations
install(DIRECTORY launch DESTINATION share/${PROJECT_NAME}/)

# Export runtime dependencies
ament_export_dependencies(rosidl_default_runtime)

ament_package()
