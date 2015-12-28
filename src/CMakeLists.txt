FILE(GLOB app_src_glob *.cc;*.h;*.inl)

find_package(wxWidgets COMPONENTS core base gl)
include( "${wxWidgets_USE_FILE}" )

FILE(GLOB lua_src_glob ${CMAKE_SOURCE_DIR}/external/lua-5.3.2/*.c)
FILE(GLOB lua_hdr_glob ${CMAKE_SOURCE_DIR}/external/lua-5.3.2/*.yh)

set(lua_src ${lua_src_glob} ${lua_hdr_glob})
include_directories(${CMAKE_SOURCE_DIR}/external/selene/)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
PROTOBUF_GENERATE_CPP(GUI_PROTO_SRCS GUI_PROTO_HDRS gui.proto)
set(app_src ${app_src_glob} "${PROJECT_BINARY_DIR}/simo3d-config.h" gui.proto)

set(pbjson_src
	${CMAKE_SOURCE_DIR}/external/pbjson/src/pbjson.cpp
	${CMAKE_SOURCE_DIR}/external/pbjson/src/pbjson.hpp
	${CMAKE_SOURCE_DIR}/external/pbjson/src/bin2ascii.h
	)

source_group("" FILES ${app_src})
source_group("generated" FILES ${GUI_PROTO_SRCS} ${GUI_PROTO_HDRS})
source_group("external" FILES ${pbjson_src})
source_group("lua" FILES ${lua_src})
source_group(cmake FILES "${PROJECT_SOURCE_DIR}/simo3d-config-in.h")

set(all_src ${app_src} ${lua_src} "${PROJECT_SOURCE_DIR}/simo3d-config-in.h" ${src_glew} ${src_soil} ${src_jsoncpp} ${src_lua} ${GUI_PROTO_SRCS} ${GUI_PROTO_HDRS}  ${pbjson_src})

include_directories(${CMAKE_SOURCE_DIR}/external/pbjson/src/)

message(STATUS "Linking with OpenGL ${OPENGL_LIBRARIES}")

if(BUILD_USE_DOXYGEN)
	message(STATUS "Doxygen exe is ${DOXYGEN_EXECUTABLE} with args ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile in dir ${CMAKE_SOURCE_DIR}/simo3d")
endif(BUILD_USE_DOXYGEN)

add_executable(simo3d WIN32 MACOSX_BUNDLE ${all_src})
set_target_properties( simo3d
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/dist"
	DEBUG_OUTPUT_NAME "simo3d-debug"
	RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/dist"
	RELEASE_OUTPUT_NAME "simo3d"
)

if(BUILD_USE_CLANG_FORMAT)
	add_custom_command(TARGET simo3d
				   PRE_BUILD
				   COMMAND ./call-clang-format.py ${CMAKE_CURRENT_SOURCE_DIR}/*.cc ${CMAKE_CURRENT_SOURCE_DIR}/*.h  ${mainsrc}
				   WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
				   COMMENT "Running clang format" VERBATIM)
endif(BUILD_USE_CLANG_FORMAT)

if(BUILD_USE_CPPLINT)
	add_custom_command(TARGET simo3d
					   PRE_BUILD
					   COMMAND ./cpplint.py ARGS --output=vs7 ${CMAKE_CURRENT_SOURCE_DIR}/*.cc ${CMAKE_CURRENT_SOURCE_DIR}/*.h  ${mainsrc}
					   WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
					   COMMENT "Running CppLint" VERBATIM)
endif(BUILD_USE_CPPLINT)

if(BUILD_USE_DOXYGEN)
	add_custom_command(TARGET simo3d
					   PRE_LINK
					   COMMAND ${DOXYGEN_EXECUTABLE} ARGS ${CMAKE_CURRENT_BINARY_DIR}/../Doxyfile
					   WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
					   COMMENT "Running Doxygen" VERBATIM)
endif(BUILD_USE_DOXYGEN)

target_link_libraries(simo3d ${OPENGL_LIBRARIES} ${wxWidgets_LIBRARIES} ${ASSIMP_LIBRARIES} ${PROTOBUF_LIBRARIES} )
