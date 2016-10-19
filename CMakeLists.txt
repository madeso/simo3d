CMAKE_MINIMUM_REQUIRED(VERSION 3.1.0)
SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS OFF)
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)
PROJECT(simo3d)

SET(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake-modules")

OPTION(BUILD_USE_CLANG_FORMAT "Add clang format to the build" OFF)
OPTION(BUILD_USE_CPPLINT "Add cpplint to the build" OFF)
OPTION(BUILD_USE_DOXYGEN "Add doxygen to the build" OFF)

FIND_PACKAGE(AssImp REQUIRED)
FIND_PACKAGE(OpenGL REQUIRED)

FIND_PACKAGE(Protobuf REQUIRED)
INCLUDE_DIRECTORIES(${PROTOBUF_INCLUDE_DIRS})

# hack ot tell wxwidgets we have full compiler support
# ADD_DEFINITIONS(/DHAVE_TYPE_TRAITS)
# ADD_DEFINITIONS(/DHAVE_TR1_TYPE_TRAITS)

if(${APPLE})
	# disabling http://stackoverflow.com/questions/32626171/xcode-7-how-to-suppress-warning-overrides-a-member-function-but-is-not-marked
	# that apple-clang spews out for wx headers
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-inconsistent-missing-override")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
endif()

# compiler flags
IF(${MSVC})
	ADD_DEFINITIONS(/D _CRT_SECURE_NO_WARNINGS)

	# character set to Not Set=ASCII/SBCS http://public.kitware.com/Bug/view.php?id=12189
	ADD_DEFINITIONS(/D _SBCS)
ENDIF()

# force visual studio and the like to only have debug and release builds
IF(CMAKE_CONFIGURATION_TYPES)
	SET(CMAKE_CONFIGURATION_TYPES Debug Release)
	SET(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING "Reset the configurations to what we need" FORCE)
ENDIF()

# add a target to generate API documentation with Doxygen
# find_package(Doxygen REQUIRED)
# configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.ini ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
CONFIGURE_FILE("${PROJECT_SOURCE_DIR}/simo3d-config-in.h" "${PROJECT_BINARY_DIR}/simo3d-config.h")

# wxWidgets_USE_DEBUG
# wxWidgets_USE_UNICODE
# wxWidgets_USE_UNIVERSAL
# SET(wxWidgets_USE_STATIC, ON)

INCLUDE_DIRECTORIES( "external/CML-1_0_4" )

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})


MESSAGE(STATUS "Including Assimp ${ASSIMP_INCLUDE_DIR}")
INCLUDE_DIRECTORIES(${ASSIMP_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

ADD_SUBDIRECTORY(src)

