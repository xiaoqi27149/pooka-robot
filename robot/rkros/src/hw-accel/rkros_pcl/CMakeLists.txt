cmake_minimum_required(VERSION 3.8)
project(rkros_pcl)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)

# Path to rkALG_PCL (root of the rkALG_PCL tree). Can be overridden with -DRKALG_PCL_DIR=/path/to/rkALG_PCL
set(RKALG_PCL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../../common_algorithm/pcl/rkALG_PCL" CACHE PATH "Path to rkALG_PCL root directory")

if(NOT EXISTS "${RKALG_PCL_DIR}")
  message(FATAL_ERROR "rkALG_PCL directory not found at: ${RKALG_PCL_DIR}\nPlease ensure the rkALG_PCL module is available before building.")
endif()

if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64|ARM64")
  set(_arch_dir "${RKALG_PCL_DIR}/lib/aarch64-linux-gnu")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "armv7|armhf|ARM")
  set(_arch_dir "${RKALG_PCL_DIR}/lib/arm-linux-gnu")
else()
  message(FATAL_ERROR "Unsupported architecture: ${CMAKE_SYSTEM_PROCESSOR}")
endif()

install(DIRECTORY ${RKALG_PCL_DIR}/include/
  DESTINATION include/${PROJECT_NAME}
  FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)

install(DIRECTORY ${_arch_dir}/
  DESTINATION lib
  FILES_MATCHING PATTERN "*.so*"
)

ament_export_include_directories(include)
ament_export_libraries(rkALG_pclCore)

configure_file(cmake/rkros_pcl.cmake.in
               "${CMAKE_CURRENT_BINARY_DIR}/rkros_pcl.cmake" @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/rkros_pcl.cmake"
        DESTINATION share/${PROJECT_NAME})

ament_package()
