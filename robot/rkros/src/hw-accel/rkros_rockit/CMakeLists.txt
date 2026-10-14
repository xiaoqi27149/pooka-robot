cmake_minimum_required(VERSION 3.8)
project(rkros_rockit)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)

if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64|ARM64")
  set(_arch_dir "${CMAKE_CURRENT_SOURCE_DIR}/lib/arm64")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "armv7|armhf|ARM")
  set(_arch_dir "${CMAKE_CURRENT_SOURCE_DIR}/lib/arm32")
else()
  message(FATAL_ERROR "Unsupported architecture: ${CMAKE_SYSTEM_PROCESSOR}")
endif()

install(DIRECTORY include/
        DESTINATION include/${PROJECT_NAME})

install(DIRECTORY ${_arch_dir}/
        DESTINATION lib
        FILES_MATCHING PATTERN "*.so*")

ament_export_include_directories(include)
ament_export_libraries(rockit)

configure_file(cmake/rkros_rockit.cmake.in
               "${CMAKE_CURRENT_BINARY_DIR}/rkros_rockit.cmake" @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/rkros_rockit.cmake"
        DESTINATION share/${PROJECT_NAME})

ament_package()
