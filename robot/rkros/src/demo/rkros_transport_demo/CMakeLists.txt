cmake_minimum_required(VERSION 3.8)
project(rkros_transport_demo)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_components REQUIRED)
find_package(builtin_interfaces REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(rkros_zcopy_ctl REQUIRED)
find_package(rkros_zcopy_msgs REQUIRED)
find_package(rkros_dmabuf REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(rkros_common REQUIRED)

include_directories(include)

rosidl_generate_interfaces(${PROJECT_NAME}
  msg/BigMessage.msg
  DEPENDENCIES builtin_interfaces
)

rosidl_get_typesupport_target(cpp_typesupport_target
  ${PROJECT_NAME} "rosidl_typesupport_cpp")

# publisher_component
add_library(publisher_component SHARED src/publisher_component.cpp)
ament_target_dependencies(publisher_component
  rclcpp
  rclcpp_components
  rkros_zcopy_ctl
  rkros_zcopy_msgs
  rkros_dmabuf
  rkros_common
)

target_link_libraries(publisher_component "${cpp_typesupport_target}")

rclcpp_components_register_node(publisher_component
  PLUGIN "rkros_transport_demo::PublisherComponent"
  EXECUTABLE publisher_node)

# subscriber_component
add_library(subscriber_component SHARED src/subscriber_component.cpp)
ament_target_dependencies(subscriber_component rclcpp rclcpp_components rkros_zcopy_msgs rkros_dmabuf)
target_link_libraries(subscriber_component "${cpp_typesupport_target}")

rclcpp_components_register_node(subscriber_component
  PLUGIN "rkros_transport_demo::SubscriberComponent"
  EXECUTABLE subscriber_node)

# single_process_launch
add_executable(single_process_launch src/single_process_launch.cpp)
ament_target_dependencies(single_process_launch rclcpp)
target_link_libraries(single_process_launch
  "${cpp_typesupport_target}"
  publisher_component
  subscriber_component)

# install
install(TARGETS publisher_component subscriber_component
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

install(TARGETS single_process_launch DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY launch DESTINATION share/${PROJECT_NAME})

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
