cmake_minimum_required(VERSION 3.8)
project(rkros_vcam)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_components REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(rkros_dmabuf REQUIRED)
find_package(rkros_zcopy_msgs REQUIRED)
find_package(rkros_common REQUIRED)
find_package(rkros_opencv REQUIRED)
find_package(OpenCV REQUIRED)
find_package(rkros_image_proc REQUIRED)

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


include_directories(include)

add_library(${PROJECT_NAME}_node SHARED
  src/vcam_node.cpp
  src/image_generator.cpp
)

target_include_directories(${PROJECT_NAME}_node PRIVATE include)

ament_target_dependencies(${PROJECT_NAME}_node
  rclcpp
  rclcpp_components
  sensor_msgs
  rkros_dmabuf
  rkros_zcopy_msgs
  rkros_common
  OpenCV
  rkros_opencv
  rkros_image_proc
)

rclcpp_components_register_nodes(${PROJECT_NAME}_node "rkros_vcam::VCamNode")

install(TARGETS ${PROJECT_NAME}_node
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

add_executable(${PROJECT_NAME}
  src/vcam_main.cpp
)
target_link_libraries(${PROJECT_NAME}
  ${PROJECT_NAME}_node
)
ament_target_dependencies(${PROJECT_NAME}
  rclcpp
)
install(TARGETS ${PROJECT_NAME}
  DESTINATION lib/${PROJECT_NAME}
)

# install launch and config
install(DIRECTORY launch DESTINATION share/${PROJECT_NAME})

ament_package()
