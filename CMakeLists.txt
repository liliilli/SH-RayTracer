cmake_minimum_required(VERSION 3.8)
project(ShRayTracer CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_VERBOSE_MAKEFILE true)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
add_subdirectory(DyUtils)

set(SOURCE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Source")
set(SOURCE
    "${SOURCE_DIRECTORY}/FCamera.cc"
    "${SOURCE_DIRECTORY}/FRenderWorker.cc"
    "${SOURCE_DIRECTORY}/XMain.cc"
    "${SOURCE_DIRECTORY}/XCommon.cc"
    "${SOURCE_DIRECTORY}/XSamples.cc"
)

find_package(Threads REQUIRED)
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/Include")
add_executable(${CMAKE_PROJECT_NAME}
    ${SOURCE}
)
target_include_directories(${CMAKE_PROJECT_NAME}
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Include

PRIVATE
    ${CMAKE_SOURCE_DIR}/DyUtils/DyExpression/Include
    ${CMAKE_SOURCE_DIR}/DyUtils/DyStringUtil/Include
    ${CMAKE_SOURCE_DIR}/DyUtils/DyMath/Include
)
target_link_libraries(${CMAKE_PROJECT_NAME}
    DyStringUtil
    DyMath
    pthread
)
add_definitions(-lstdc++fs -lpthread)

# INSTALL SETTINGS
set_target_properties(${CMAKE_PROJECT_NAME}
    PROPERTIES
    LINKER_LANGUAGE CXX
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib/${CMAKE_BUILD_TYPE}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib/${CMAKE_BUILD_TYPE}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}"
)