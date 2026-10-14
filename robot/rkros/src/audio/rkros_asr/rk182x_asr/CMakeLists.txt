cmake_minimum_required(VERSION 3.8)
project(rkros_asr)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(rkros_audio_msgs REQUIRED)
find_package(rkros_asr_msgs REQUIRED)

# 添加RockX2 SDK路径
set(ROCKX_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/rockx2)
set(ASR_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)
message(STATUS "ROCKX_ROOT_DIR: ${ROCKX_ROOT_DIR}")
message(STATUS "ASR_ROOT_DIR: ${ASR_ROOT_DIR}")

# 检查RockX2库和头文件路径
set(ROCKX_INCLUDE_DIR ${ROCKX_ROOT_DIR}/lib/Linux/include)
set(ROCKX_LIB_DIR ${ROCKX_ROOT_DIR}/lib/Linux/aarch64)
set(ASR_LIB_DIR ${ASR_ROOT_DIR}/Linux/aarch64)

# 检查头文件是否存在
if(EXISTS ${ROCKX_INCLUDE_DIR}/rockx.h)
  message(STATUS "RockX header found at: ${ROCKX_INCLUDE_DIR}")
else()
  message(WARNING "RockX header not found at: ${ROCKX_INCLUDE_DIR}")
endif()

# 检查库文件是否存在
if(EXISTS ${ROCKX_LIB_DIR}/librockx2.so)
  message(STATUS "RockX library found at: ${ROCKX_LIB_DIR}")
  set(ROCKX_LIBRARIES ${ROCKX_LIB_DIR}/librockx2.so ${ROCKX_LIB_DIR}/librockx_modules.so)
else()
  message(WARNING "RockX library not found at: ${ROCKX_LIB_DIR}")
endif()

if(EXISTS ${ASR_LIB_DIR}/librockasr.so)
  message(STATUS "RockASR library found at: ${ASR_LIB_DIR}")
  set(ASR_LIBRARIES ${ASR_LIB_DIR}/librockasr.so)
else()
  message(WARNING "RockASR library not found at: ${ASR_LIB_DIR}")
endif()

# 添加可执行文件 - 需要在消息生成之前声明，但依赖要在之后设置
add_executable(rk_asr_node src/rk_asr_node.cpp)
add_executable(rk_asr_result_subscriber src/rk_asr_result_subscriber.cpp)



# 设置ASR头文件路径
set(ASR_INCLUDE_DIR ${ASR_ROOT_DIR}/Linux/include)

# 链接库
target_include_directories(rk_asr_node PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)

target_include_directories(rk_asr_node PUBLIC ${ROCKX_INCLUDE_DIR} ${ASR_INCLUDE_DIR})

# 同时链接RockX和ASR库
target_link_libraries(rk_asr_node ${ROCKX_LIBRARIES} ${ASR_LIBRARIES} m dl pthread)

ament_target_dependencies(rk_asr_node
  rclcpp
  std_msgs
  sensor_msgs
  rkros_audio_msgs
  rkros_asr_msgs)

ament_target_dependencies(rk_asr_result_subscriber
  rclcpp
  std_msgs
  rkros_asr_msgs)



# 使用新的方式链接类型支持库
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(_add_compile_options -Wno-deprecated-declarations)
  set_target_properties(rk_asr_node rk_asr_result_subscriber PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
  )
endif()


# 安装目标
install(TARGETS
  rk_asr_node
  rk_asr_result_subscriber
  DESTINATION lib/${PROJECT_NAME})

# 安装RockX库文件
install(FILES
  ${ROCKX_LIB_DIR}/librockx2.so
  ${ROCKX_LIB_DIR}/librockx_modules.so
  ${ROCKX_LIB_DIR}/librknnrt.so
  DESTINATION lib)

# 安装ASR库文件
install(FILES
  ${ASR_LIB_DIR}/librockasr.so
  ${ASR_LIB_DIR}/libonnxruntime.so
  ${ASR_LIB_DIR}/librknn3_api.so
  DESTINATION lib)

# 安装配置文件
install(FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/rockx_auth_config.json
  DESTINATION share/${PROJECT_NAME}/config)

# 安装数据文件
install(DIRECTORY
  ${CMAKE_CURRENT_SOURCE_DIR}/data
  DESTINATION share/${PROJECT_NAME})

ament_package()
