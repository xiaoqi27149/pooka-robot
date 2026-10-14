cmake_minimum_required(VERSION 3.8)
project(rkros_idc)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)

# Path to rkALG_IDC (root of the rkALG_IDC tree). Can be overridden with -DRKALG_IDC_DIR=/path/to/rkALG_IDC
set(RKALG_IDC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../../common_algorithm/idc/rkalg_idc_lut_demo" CACHE PATH "Path to rkalg_idc_lut_demo root directory")

if(NOT EXISTS "${RKALG_IDC_DIR}")
  message(FATAL_ERROR "rkalg_idc_lut_demo directory not found at: ${RKALG_IDC_DIR}\nPlease ensure the rkalg_idc_lut_demo module is available before building.")
endif()

if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64|ARM64")
  set(_arch_dir "${RKALG_IDC_DIR}/lib/gcc-linaro-6.3.1-2017.05-aarch64")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "armv7|armhf|ARM")
  set(_arch_dir "${RKALG_IDC_DIR}/lib/arm-linux-gnu")
else()
  message(FATAL_ERROR "Unsupported architecture: ${CMAKE_SYSTEM_PROCESSOR}")
endif()

install(DIRECTORY ${RKALG_IDC_DIR}/include/
  DESTINATION include/${PROJECT_NAME}
  FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)

install(DIRECTORY ${_arch_dir}/
  DESTINATION lib
  FILES_MATCHING PATTERN "*.so*"
)

ament_export_include_directories(include)
ament_export_libraries(rkalg_idc)

configure_file(cmake/rkros_idc.cmake.in
               "${CMAKE_CURRENT_BINARY_DIR}/rkros_idc.cmake" @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/rkros_idc.cmake"
        DESTINATION share/${PROJECT_NAME})

ament_package()
