cmake_minimum_required(VERSION 3.8)
project(rkros_opencv)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(CMAKE_CXX_STANDARD 17)

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rkros_dmabuf REQUIRED)
find_package(rkros_common REQUIRED)
find_package(OpenCV REQUIRED)

if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64|ARM64")
  set(_arch_dir "${CMAKE_CURRENT_SOURCE_DIR}/lib/arm64")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "armv7|armhf|ARM")
  set(_arch_dir "${CMAKE_CURRENT_SOURCE_DIR}/lib/arm32")
else()
  message(FATAL_ERROR "Unsupported architecture: ${CMAKE_SYSTEM_PROCESSOR}")
endif()

install(DIRECTORY include/${PROJECT_NAME}
        DESTINATION include/)

install(DIRECTORY ${_arch_dir}/
        DESTINATION lib
        FILES_MATCHING PATTERN "*.so*")

ament_export_include_directories(include)
ament_export_libraries(${PROJECT_NAME})

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

  add_executable(bench_cv
    test/bench_cv.cpp
  )
  target_include_directories(bench_cv PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
  )
  ament_target_dependencies(bench_cv
    rkros_dmabuf
    rkros_common
  )
  target_link_directories(bench_cv PRIVATE ${_arch_dir})
  target_link_libraries(bench_cv ${OpenCV_LIBS} ${PROJECT_NAME})
  install(TARGETS bench_cv DESTINATION lib/${PROJECT_NAME})
endif()

ament_package()
