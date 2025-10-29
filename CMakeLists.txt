cmake_minimum_required(VERSION 3.12)

project(voxel)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "")
    set (CMAKE_BUILD_TYPE "Debug" CACHE STRING "Set the build type")
endif ()

file(GLOB_RECURSE SRCS ${CMAKE_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE INCS ${CMAKE_SOURCE_DIR}/src/*.h)
add_executable(voxel ${SRCS} ${INCS})

target_include_directories(voxel PUBLIC "C:\\VulkanSDK\\1.4.328.1\\Include")
target_link_directories(voxel PUBLIC "C:\\VulkanSDK\\1.4.328.1\\Lib")

target_link_libraries(voxel SDL2 SDL2main volk shaderc_combinedd spirv-cross-reflectd spirv-cross-cored)

target_compile_definitions(voxel PUBLIC $<$<CONFIG:Debug>:VOXEL_DEBUG>)
target_link_options(voxel PUBLIC /ignore:4099)

set_property(TARGET voxel PROPERTY CXX_STANDARD 17)
