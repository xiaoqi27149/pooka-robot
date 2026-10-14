cmake_minimum_required(VERSION 3.10)

project(rknn_yoloe_demo)

if (ENABLE_ASAN)
	message(STATUS "BUILD WITH ADDRESS SANITIZER")
	set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
	set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
	set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
endif ()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../../3rdparty/ 3rdparty.out)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../../utils/ utils.out)

if (CMAKE_SYSTEM_NAME STREQUAL "Android")
  set(OpenCV_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../3rdparty/opencv/opencv-android-sdk-build/sdk/native/jni/abi-${CMAKE_ANDROID_ARCH_ABI})
else()
    if(TARGET_LIB_ARCH STREQUAL "lib")
      set(OpenCV_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../3rdparty/opencv/opencv-linux-armhf/share/OpenCV)
    else()
      set(OpenCV_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../3rdparty/opencv/opencv-linux-aarch64/share/OpenCV)
    endif()
endif()
find_package(OpenCV REQUIRED)
message(STATUS OpenCV_DIR=${OpenCV_DIR})
message(STATUS OpenCV_LIBS=${OpenCV_LIBS})

set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")

set(CMAKE_INSTALL_RPATH "$ORIGIN/lib")

file(GLOB SRCS ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)

#dma
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../3rdparty/allocator/dma)

#drm
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../3rdparty/allocator/drm)


include_directories(${CMAKE_SOURCE_DIR}/tokenizer)
include_directories(${CMAKE_SOURCE_DIR}/rknpu2/clip_text)
include_directories(${CMAKE_SOURCE_DIR}/rknpu2/yoloe)

set(clip_text rknpu2/clip_text/clip_text.cc)
set(yoloe rknpu2/yoloe/yoloe.cc)
set(clip_tokenizer tokenizer/clip_tokenizer.cpp)

add_executable(${PROJECT_NAME}
    main.cc
	postprocess.cc
    ${clip_text}
    ${yoloe}
	${clip_tokenizer}
)

target_link_libraries(${PROJECT_NAME}
	imageutils
    imagedrawing
    fileutils
    ${LIBRKNNRT}
    ${OpenCV_LIBS}
    dl
)

if (CMAKE_SYSTEM_NAME STREQUAL "Android")
    target_link_libraries(${PROJECT_NAME}
    log
)
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(${PROJECT_NAME}
        pthread
)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${LIBRKNNRT_INCLUDES}
     ${LIBTIMER_INCLUDES}
)

install(TARGETS ${PROJECT_NAME} DESTINATION .)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../model/detect_classes.txt DESTINATION ./model)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../model/bus.jpg DESTINATION ./model)
file(GLOB RKNN_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../model/*.rknn")
install(FILES ${RKNN_FILES} DESTINATION model)