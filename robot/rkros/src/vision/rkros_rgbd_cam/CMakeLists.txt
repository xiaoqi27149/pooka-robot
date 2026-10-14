cmake_minimum_required(VERSION 3.8)
project(rkros_rgbd_cam)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_compile_options(-w)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_components REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(image_transport REQUIRED)
find_package(std_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(OpenCV REQUIRED)

include_directories(
  include
  ${OpenCV_INCLUDE_DIRS}
)

link_directories(
  ${PROJECT_SOURCE_DIR}/lib
)

add_library(rmsl_core SHARED IMPORTED)
set_target_properties(rmsl_core PROPERTIES
  IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/lib/librmsl_core.so
  INTERFACE_INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/include
)

# ============================================================================
# ROS2 main node(exec)
# ============================================================================

set(rmsl32x_node_src
  src/rmsl32x_node.cpp
  src/rmsl32x_param.cpp
)

add_executable(rmsl32x_node ${rmsl32x_node_src})

ament_target_dependencies(rmsl32x_node
  rclcpp
  rclcpp_components
  sensor_msgs
  cv_bridge
  image_transport
  std_msgs
  tf2
  tf2_ros
)

target_link_libraries(rmsl32x_node
  rmsl_core
  ${OpenCV_LIBS}
  pthread
  dl
)

# ============================================================================
# component build
# ============================================================================

set(rmsl32x_component_src
  src/rmsl32x_node.cpp
  src/rmsl32x_param.cpp
)

add_library(rmsl_component SHARED ${rmsl32x_component_src})

target_compile_definitions(rmsl_component PRIVATE COMPOSITION_BUILD)

ament_target_dependencies(rmsl_component
  rclcpp
  rclcpp_components
  sensor_msgs
  cv_bridge
  image_transport
  std_msgs
  tf2
  tf2_ros
)

target_link_libraries(rmsl_component
  rmsl_core
  ${OpenCV_LIBS}
  pthread
  dl
)

if(rclcpp_components_FOUND)
  rclcpp_components_register_nodes(rmsl_component "RMSLNode")
endif()

# ============================================================================
# tools node
# ============================================================================

add_executable(list_device_node src/list_device_node.cpp)
ament_target_dependencies(list_device_node rclcpp)
target_link_libraries(list_device_node udev)

add_executable(switch_model src/switch_model.cpp)
ament_target_dependencies(switch_model rclcpp)
target_link_libraries(switch_model rmsl_core)

# ============================================================================
# install
# ============================================================================

install(TARGETS rmsl32x_node
  DESTINATION lib/${PROJECT_NAME}
)

install(TARGETS rmsl_component
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

install(TARGETS list_device_node switch_model
  DESTINATION lib/${PROJECT_NAME}
)

install(FILES lib/librmsl_core.so
  DESTINATION lib
)

install(DIRECTORY include/
  DESTINATION include
  FILES_MATCHING PATTERN "*.h"
)

install(DIRECTORY launch
  DESTINATION share/${PROJECT_NAME}
)

ament_export_dependencies(rclcpp sensor_msgs cv_bridge image_transport std_msgs tf2 tf2_ros)

ament_package()