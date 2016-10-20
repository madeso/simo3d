file(GLOB app_src_glob *.cc;*.h;*.inl)

set(src_lua
    ../external/lua/src/lapi.c
    ../external/lua/src/lauxlib.c
    ../external/lua/src/lbaselib.c
    ../external/lua/src/lbitlib.c
    ../external/lua/src/lcode.c
    ../external/lua/src/lcorolib.c
    ../external/lua/src/lctype.c
    ../external/lua/src/ldblib.c
    ../external/lua/src/ldebug.c
    ../external/lua/src/ldo.c
    ../external/lua/src/ldump.c
    ../external/lua/src/lfunc.c
    ../external/lua/src/lgc.c
    ../external/lua/src/linit.c
    ../external/lua/src/liolib.c
    ../external/lua/src/llex.c
    ../external/lua/src/lmathlib.c
    ../external/lua/src/lmem.c
    ../external/lua/src/loadlib.c
    ../external/lua/src/lobject.c
    ../external/lua/src/lopcodes.c
    ../external/lua/src/loslib.c
    ../external/lua/src/lparser.c
    ../external/lua/src/lstate.c
    ../external/lua/src/lstring.c
    ../external/lua/src/lstrlib.c
    ../external/lua/src/ltable.c
    ../external/lua/src/ltablib.c
    ../external/lua/src/ltm.c
    ../external/lua/src/lundump.c
    ../external/lua/src/lutf8lib.c
    ../external/lua/src/lvm.c
    ../external/lua/src/lzio.c
    )

find_package(wxWidgets COMPONENTS core base gl)
include( "${wxWidgets_USE_FILE}" )

include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/external/lua/src/)
include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/external/sol2/)

include_directories(SYSTEM ${CMAKE_CURRENT_BINARY_DIR})
protobuf_generate_cpp(GUI_PROTO_SRCS GUI_PROTO_HDRS gui.proto)
set(app_src ${app_src_glob} "${PROJECT_BINARY_DIR}/simo3d-config.h" gui.proto)

set(pbjson_src
    ${CMAKE_SOURCE_DIR}/external/pbjson/src/pbjson.cpp
    ${CMAKE_SOURCE_DIR}/external/pbjson/src/pbjson.hpp
    ${CMAKE_SOURCE_DIR}/external/pbjson/src/bin2ascii.h
    )

source_group("" FILES ${app_src})
source_group("lua" FILES ${src_lua})
source_group("generated" FILES ${GUI_PROTO_SRCS} ${GUI_PROTO_HDRS})
source_group("external" FILES ${pbjson_src})
source_group(cmake FILES "${PROJECT_SOURCE_DIR}/simo3d-config-in.h")

set(all_src ${app_src} ${src_lua} "${PROJECT_SOURCE_DIR}/simo3d-config-in.h"
    ${src_glew} ${src_soil} ${src_jsoncpp} ${pbjson_src}
    ${GUI_PROTO_SRCS} ${GUI_PROTO_HDRS})

include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/external/pbjson/src/)

message(STATUS "Linking with OpenGL ${OPENGL_LIBRARIES}")

if(BUILD_USE_DOXYGEN)
	message(STATUS "Doxygen exe is ${DOXYGEN_EXECUTABLE} with args \
	${CMAKE_CURRENT_BINARY_DIR}/Doxyfile in dir ${CMAKE_SOURCE_DIR}/simo3d")
endif(BUILD_USE_DOXYGEN)

add_executable(simo3d WIN32 MACOSX_BUNDLE ${all_src})
set_target_properties(simo3d
                      PROPERTIES
                      RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/dist"
                      DEBUG_OUTPUT_NAME "simo3d-debug"
                      RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/dist"
                      RELEASE_OUTPUT_NAME "simo3d"
                      )

# set_property(TARGET simo3d PROPERTY CXX_STANDARD 11)
# set_property(TARGET simo3d PROPERTY CMAKE_CXX_STANDARD_REQUIRED ON)
target_link_libraries(simo3d
                      ${OPENGL_LIBRARIES}
                      ${wxWidgets_LIBRARIES}
                      ${ASSIMP_LIBRARIES}
                      ${PROTOBUF_LIBRARIES}
                      )
target_link_libraries(simo3d ${CMAKE_DL_LIBS})
target_link_libraries(simo3d ${CMAKE_THREAD_LIBS_INIT})

if(BUILD_USE_CLANG_FORMAT)
	add_custom_command(TARGET simo3d
				             PRE_BUILD
				             COMMAND ./call-clang-format.py ${CMAKE_CURRENT_SOURCE_DIR}/*.cc ${CMAKE_CURRENT_SOURCE_DIR}/*.h  ${mainsrc}
				             WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
				             COMMENT "Running clang format" VERBATIM)
endif()

if(BUILD_USE_CPPLINT)
	add_custom_command(TARGET simo3d
			        		   PRE_BUILD
			        		   COMMAND ./cpplint.py ARGS --output=vs7 ${CMAKE_CURRENT_SOURCE_DIR}/*.cc ${CMAKE_CURRENT_SOURCE_DIR}/*.h  ${mainsrc}
				        	   WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
				        	   COMMENT "Running CppLint" VERBATIM)
endif()

if(BUILD_USE_DOXYGEN)
	add_custom_command(TARGET simo3d
					           PRE_LINK
				        	   COMMAND ${DOXYGEN_EXECUTABLE} ARGS ${CMAKE_CURRENT_BINARY_DIR}/../Doxyfile
				        	   WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
			        		   COMMENT "Running Doxygen" VERBATIM)
endif()

