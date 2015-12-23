cmake_minimum_required(VERSION 3.1.0)
set_property(GLOBAL PROPERTY USE_FOLDERS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(simo3d)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake-modules")

option(BUILD_USE_CLANG_FORMAT "Add clang format to the build" ON)
option(BUILD_USE_CPPLINT "Add cpplint to the build" ON)
option(BUILD_USE_DOXYGEN "Add doxygen to the build" ON)
option(BUILD_USE_CPPCHECK "run cppcheck on all source" ON)


# compiler flags
if(${MSVC})
	ADD_DEFINITIONS(/D _CRT_SECURE_NO_WARNINGS)
	
	# character set to Not Set=ASCII/SBCS http://public.kitware.com/Bug/view.php?id=12189
	ADD_DEFINITIONS(/D _SBCS)
endif()

# force visual studio and the like to only have debug and release builds
if(CMAKE_CONFIGURATION_TYPES)
	set(CMAKE_CONFIGURATION_TYPES Debug Release)
	set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING "Reset the configurations to what we need" FORCE)
endif()

# add a target to generate API documentation with Doxygen
# find_package(Doxygen REQUIRED)
# configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.ini ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
configure_file ("${PROJECT_SOURCE_DIR}/simo3d-config-in.h" "${PROJECT_BINARY_DIR}/simo3d-config.h")

# wxWidgets_USE_DEBUG
# wxWidgets_USE_UNICODE
# wxWidgets_USE_UNIVERSAL
SET(wxWidgets_USE_STATIC, ON)

find_package(wxWidgets COMPONENTS core base)
include( "${wxWidgets_USE_FILE}" )

message(STATUS "wxWidgets_FOUND = ${wxWidgets_FOUND}")
message(STATUS "wxWidgets_INCLUDE_DIRS = ${wxWidgets_INCLUDE_DIRS}")
message(STATUS "wxWidgets_LIBRARIES = ${wxWidgets_LIBRARIES}")
message(STATUS "wxWidgets_LIBRARY_DIRS = ${wxWidgets_LIBRARY_DIRS}")
message(STATUS "wxWidgets_DEFINITIONS = ${wxWidgets_DEFINITIONS}")
message(STATUS "wxWidgets_DEFINITIONS_DEBUG = ${wxWidgets_DEFINITIONS_DEBUG}")
message(STATUS "wxWidgets_CXX_FLAGS = ${wxWidgets_CXX_FLAGS}")
message(STATUS "wxWidgets_USE_FILE = ${wxWidgets_USE_FILE}")

include_directories(${CMAKE_SOURCE_DIR})


include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(src)