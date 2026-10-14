cmake_minimum_required(VERSION 3.8)
project(rkros_dmabuf)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rkros_zcopy_msgs REQUIRED)
find_package(rkros_common REQUIRED)

add_library(${PROJECT_NAME} SHARED
  src/dmabuf.cpp
  src/dmabuf_msg.cpp
)

target_link_libraries(${PROJECT_NAME}
  # 添加依赖的非ROS2库
)

target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

# 添加依赖的ROS2包
ament_target_dependencies(${PROJECT_NAME}
  rclcpp rkros_zcopy_msgs rkros_common
)

# 安装头文件，供其他包使用
install(
  DIRECTORY include/${PROJECT_NAME}
  DESTINATION include
)

# 安装库文件，供其他包使用
install(
  TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

# 把头文件和库加入ament的导出列表
ament_export_include_directories(include)
ament_export_libraries(${PROJECT_NAME})
#ament_export_targets(${PROJECT_NAME})

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

  add_executable(${PROJECT_NAME}_test test/test_rkros_dmabuf.cpp)
  target_link_libraries(${PROJECT_NAME}_test ${PROJECT_NAME})
  target_include_directories(${PROJECT_NAME}_test PUBLIC include)
  install(TARGETS ${PROJECT_NAME}_test DESTINATION lib/${PROJECT_NAME})
endif()

ament_package()
