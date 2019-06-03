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
    "${SOURCE_DIRECTORY}/FSphere.cc"
    "${SOURCE_DIRECTORY}/FMatDielectric.cc"
    "${SOURCE_DIRECTORY}/FMatLambertian.cc"
    "${SOURCE_DIRECTORY}/FMatMetal.cc"
    "${SOURCE_DIRECTORY}/MScene.cc"
    "${SOURCE_DIRECTORY}/XMain.cc"
    "${SOURCE_DIRECTORY}/XCommon.cc"
    "${SOURCE_DIRECTORY}/XHelperIO.cc"
    "${SOURCE_DIRECTORY}/XHelperJson.cc"
    "${SOURCE_DIRECTORY}/XSamples.cc"
)

find_package(Threads REQUIRED)
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/Include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty")
add_library(sse4_1 INTERFACE IMPORTED)
add_executable(${CMAKE_PROJECT_NAME}
    ${SOURCE}
)
target_include_directories(${CMAKE_PROJECT_NAME}
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Include

PRIVATE
	${CMAKE_SOURCE_DIR}/ThirdParty
    ${CMAKE_SOURCE_DIR}/DyUtils/DyExpression/Include
    ${CMAKE_SOURCE_DIR}/DyUtils/DyStringUtil/Include
    ${CMAKE_SOURCE_DIR}/DyUtils/DyMath/Include
)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	target_compile_options(sse4_1 INTERFACE -msse4.1)
	target_link_libraries(${CMAKE_PROJECT_NAME}
		DyStringUtil
		DyExpression
		DyMath
		pthread
	)
	add_definitions(-lstdc++fs -lpthread)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	message(STATUS "Build ${PROJECT_NAME} with ${CMAKE_CXX_COMPILER_ID}...")
    target_compile_options(sse4_1 INTERFACE /arch:SSE4.1)
	target_link_libraries(${CMAKE_PROJECT_NAME}
		DyStringUtil
		DyExpression
		DyMath
	)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS -DUNICODE)
	# Warning level 4 as warning to error.
	# Enable Multiple core building.
	# Force to always compile with W4
	if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
		string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	else()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
	endif()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX /MP /wd4127")
endif()

# INSTALL SETTINGS
set_target_properties(${CMAKE_PROJECT_NAME}
    PROPERTIES
    LINKER_LANGUAGE CXX
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib/${CMAKE_BUILD_TYPE}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib/${CMAKE_BUILD_TYPE}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}"
)
