cmake_minimum_required(VERSION 3.8)
project(ShRayTracer 
	VERSION 2019.06.09
	LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_VERBOSE_MAKEFILE true)

# If GNU or Clang, Add common/debug/release flags.
if(GNU)
	set(CMAKE_CXX_FLAGS_COMMON	"-Wall -Werror -pedantic")
	set(CMAKE_CXX_FLAGS_DEBUG	"-g -O0 -pg")
	set(CMKAE_CXX_FLAGS_RELEASE "-s -O2 -DNDEBUG -march=native")
endif(GNU)
if(Clang)
	set(CMAKE_CXX_FLAGS_COMMON	"-Wall -Werror -pedantic")
	set(CMAKE_CXX_FLAGS_DEBUG	"-g -O0 -pg")
	set(CMKAE_CXX_FLAGS_RELEASE "-s -O2 -DNDEBUG -march=native")
endif(Clang)

set(ASSIMP_NO_EXPORT "ON")
set(ASSIMP_BUILD_ASSIMP_TOOLS "OFF")
set(ASSIMP_WERROR "ON")
set(ASSIMP_BUILD_TESTS "OFF")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
add_subdirectory(DyUtils)

set(SOURCE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Source")
set(SOURCE
    "${SOURCE_DIRECTORY}/Interface/IMaterial.cc"
    "${SOURCE_DIRECTORY}/Interface/IObject.cc"

    "${SOURCE_DIRECTORY}/KDTree/DTreeNode.cc"

    "${SOURCE_DIRECTORY}/Manager/MScene.cc"
    "${SOURCE_DIRECTORY}/Manager/MMaterial.cc"
    "${SOURCE_DIRECTORY}/Manager/MModel.cc"

    "${SOURCE_DIRECTORY}/OldMaterial/FMatLambertian.cc"
    "${SOURCE_DIRECTORY}/OldMaterial/FMatDielectric.cc"
    "${SOURCE_DIRECTORY}/OldMaterial/FMatMetal.cc"

    "${SOURCE_DIRECTORY}/Shape/FSphere.cc"
    "${SOURCE_DIRECTORY}/Shape/FPlane.cc"
    "${SOURCE_DIRECTORY}/Shape/FTorus.cc"
    "${SOURCE_DIRECTORY}/Shape/FBox.cc"
    "${SOURCE_DIRECTORY}/Shape/FModel.cc"
    "${SOURCE_DIRECTORY}/Shape/FModelMesh.cc"
    "${SOURCE_DIRECTORY}/Shape/FModelPrefab.cc"
    "${SOURCE_DIRECTORY}/Shape/FCone.cc"
    "${SOURCE_DIRECTORY}/Shape/FCapsule.cc"
    "${SOURCE_DIRECTORY}/Shape/PModelCtor.cc"

    "${SOURCE_DIRECTORY}/Id/DMatId.cc"
    "${SOURCE_DIRECTORY}/Id/DMeshId.cc"
    "${SOURCE_DIRECTORY}/Id/DModelId.cc"
    "${SOURCE_DIRECTORY}/Id/DModelBufferId.cc"

    "${SOURCE_DIRECTORY}/Resource/DModel.cc"
    "${SOURCE_DIRECTORY}/Resource/DModelBuffer.cc"
    "${SOURCE_DIRECTORY}/Resource/DModelMesh.cc"
    "${SOURCE_DIRECTORY}/Resource/DModelPrefab.cc"
    "${SOURCE_DIRECTORY}/Material/DMatMetaExternal.cc"

    "${SOURCE_DIRECTORY}/FCamera.cc"
    "${SOURCE_DIRECTORY}/FRenderWorker.cc"
    "${SOURCE_DIRECTORY}/XMain.cc"
    "${SOURCE_DIRECTORY}/XCommon.cc"
    "${SOURCE_DIRECTORY}/Helper/XHelperIO.cc"
    "${SOURCE_DIRECTORY}/Helper/XHelperJson.cc"
    "${SOURCE_DIRECTORY}/Helper/XTinyObj.cc"
)

find_package(Threads REQUIRED)
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/Include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty")
add_library(sse4_1 INTERFACE IMPORTED)
add_executable(${CMAKE_PROJECT_NAME} ${SOURCE})
target_include_directories(${CMAKE_PROJECT_NAME}
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Include

PRIVATE
	${CMAKE_SOURCE_DIR}/ThirdParty
	${CMAKE_SOURCE_DIR}/assimp500/include
    ${CMAKE_SOURCE_DIR}/DyUtils/DyExpression/Include
    ${CMAKE_SOURCE_DIR}/DyUtils/DyStringUtil/Include
    ${CMAKE_SOURCE_DIR}/DyUtils/DyMath/Include
)

message(STATUS 
	"Build ${PROJECT_NAME} with ${CMAKE_CXX_COMPILER_ID} as ${CMAKE_BUILD_TYPE} mode...")
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	target_compile_options(sse4_1 INTERFACE -msse4.1)
	# add_library(DyMath INTERFACE IMPORTED)
	link_directories("${CMAKE_SOURCE_DIR}/DyUtils/DyMath/lib/")
	target_link_libraries(${CMAKE_PROJECT_NAME}
		DyStringUtil
		DyExpression
		DyMath
		pthread
	)
	add_definitions(-lstdc++fs -lpthread)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_COMMON}")
	if ("${CMAKE_BUILD_TYPE}" STREQUAL "DEBUG")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}")
	elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "RELEASE")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMKAE_CXX_FLAGS_RELEASE}")
	else()
		message(FATAL_ERROR "CMAKE_BUILD_TYPE is not specified. Failed to build.")
	endif()
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	
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
