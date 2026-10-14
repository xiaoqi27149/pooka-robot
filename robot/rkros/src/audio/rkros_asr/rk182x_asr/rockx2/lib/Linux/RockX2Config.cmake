
if (CMAKE_SYSTEM_NAME STREQUAL "Android")
    set (TARGET_LIB_ARCH ${CMAKE_ANDROID_ARCH_ABI})
else()
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set (TARGET_LIB_ARCH aarch64)
    else()
        set (TARGET_LIB_ARCH armhf)
    endif()
    if (CMAKE_C_COMPILER MATCHES "uclibc")
        set (TARGET_LIB_ARCH ${TARGET_LIB_ARCH}_uclibc)
    endif()
endif()

set(RockX2_INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR}/include)
set(RockX2_LIBS
    ${CMAKE_CURRENT_LIST_DIR}/${TARGET_LIB_ARCH}/librockx2.so
    ${CMAKE_CURRENT_LIST_DIR}/${TARGET_LIB_ARCH}/librockx_modules.so 
    ${CMAKE_CURRENT_LIST_DIR}/${TARGET_LIB_ARCH}/librknnrt.so
)
