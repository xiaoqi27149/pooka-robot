cmake_minimum_required (VERSION 3.15)
project ("rkALG_pclCoreTool")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIE -fPIC -Wunused-value")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIE -fPIC -Wunused-value")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -DNDEBUG -Wno-narrowing -fvisibility=hidden")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG -Wno-narrowing -fvisibility-inlines-hidden -fvisibility=hidden")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -ffunction-sections -fdata-sections")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -ffunction-sections -fdata-sections")
set(CMAKE_EXE_LINKER_FLAGS    "${CMAKE_EXE_LINKER_FLAGS}    -Wl,--gc-sections")
set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,--gc-sections")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--gc-sections")

message("PROJECT_SOURCE_DIR: ${PROJECT_SOURCE_DIR}")
link_directories(${PROJECT_SOURCE_DIR}/lib/aarch64-linux-gnu/)
include_directories(${PROJECT_SOURCE_DIR}/include/)

if (NOT DEFINED EIGEN_DIR)
    set(EIGEN_DIR "/usr/include/eigen3")
endif()

include_directories(${EIGEN_DIR})

# samples
set(SAMPLE_TARGETS
    rkPCL_common
    rkPCL_features
    rkPCL_filter
    rkPCL_registration
    rkPCL_search
    rkPCL_segmentation
)

set(rkPCL_common_SRC   ${CMAKE_CURRENT_SOURCE_DIR}/sample/common_sample.cpp)
set(rkPCL_features_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sample/features_sample.cpp)
set(rkPCL_filter_SRC   ${CMAKE_CURRENT_SOURCE_DIR}/sample/filter_sample.cpp)
set(rkPCL_registration_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sample/registration_sample.cpp)
set(rkPCL_search_SRC   ${CMAKE_CURRENT_SOURCE_DIR}/sample/search_sample.cpp)
set(rkPCL_segmentation_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sample/segmentation_sample.cpp)

foreach(target IN LISTS SAMPLE_TARGETS)
    add_executable(${target} ${${target}_SRC})
endforeach()

foreach(target IN LISTS SAMPLE_TARGETS)
    target_link_libraries(${target} PRIVATE rkALG_pclCore)
endforeach()

foreach(target IN LISTS SAMPLE_TARGETS)
    install(TARGETS ${target}
            RUNTIME DESTINATION usr/bin)
endforeach()

install(FILES ${PROJECT_SOURCE_DIR}/lib/aarch64-linux-gnu/librkALG_pclCore.so
	DESTINATION usr/lib/aarch64-linux-gnu)
