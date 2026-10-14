cmake_minimum_required(VERSION 3.8)
project(rkros_mipi_cam)

# set c++ version
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find depends
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(OpenCV REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(rkros_rockit REQUIRED)
find_package(rkros_dmabuf REQUIRED)
find_package(rkros_zcopy_msgs REQUIRED)
find_package(rkros_common REQUIRED)
find_package(rclcpp_components REQUIRED)

add_library(rkros_mipi_cam_component SHARED
    src/rkros_mipi_node.cpp
    src/rockit_capture.cpp
)

# define COMPOSITION_BUILD，block the main func
target_compile_definitions(rkros_mipi_cam_component PRIVATE COMPOSITION_BUILD)

target_include_directories(rkros_mipi_cam_component PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# depends list
set(COMMON_DEPENDS
    rclcpp
    rclcpp_components
    OpenCV
    sensor_msgs
    rkros_rockit
    rkros_dmabuf
    rkros_zcopy_msgs
    rkros_common
)

ament_target_dependencies(rkros_mipi_cam_component ${COMMON_DEPENDS})

# component_regiter
rclcpp_components_register_nodes(rkros_mipi_cam_component "RockitCameraNode")

add_executable(rkros_mipi_cam_node
    src/rkros_mipi_node.cpp
    src/rockit_capture.cpp
)

target_include_directories(rkros_mipi_cam_node PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# add ament depends
ament_target_dependencies(rkros_mipi_cam_node ${COMMON_DEPENDS})

# install
install(TARGETS
    rkros_mipi_cam_node
    rkros_mipi_cam_component
    DESTINATION lib/${PROJECT_NAME}
)

# install launch file
install(DIRECTORY
  launch
  DESTINATION share/${PROJECT_NAME}
)

# install include
#install(DIRECTORY include/
#    DESTINATION include/
#)

# test
if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
