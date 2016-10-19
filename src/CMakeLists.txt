FILE(GLOB app_src_glob *.cc;*.h;*.inl)

SET(src_lua
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

FIND_PACKAGE(wxWidgets COMPONENTS core base gl)
INCLUDE( "${wxWidgets_USE_FILE}" )

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/external/lua/src/)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/external/sol2/)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
PROTOBUF_GENERATE_CPP(GUI_PROTO_SRCS GUI_PROTO_HDRS gui.proto)
SET(app_src ${app_src_glob} "${PROJECT_BINARY_DIR}/simo3d-config.h" gui.proto)

SET(pbjson_src
    ${CMAKE_SOURCE_DIR}/external/pbjson/src/pbjson.cpp
    ${CMAKE_SOURCE_DIR}/external/pbjson/src/pbjson.hpp
    ${CMAKE_SOURCE_DIR}/external/pbjson/src/bin2ascii.h
    )

SOURCE_GROUP("" FILES ${app_src})
SOURCE_GROUP("lua" FILES ${src_lua})
SOURCE_GROUP("generated" FILES ${GUI_PROTO_SRCS} ${GUI_PROTO_HDRS})
SOURCE_GROUP("external" FILES ${pbjson_src})
SOURCE_GROUP(cmake FILES "${PROJECT_SOURCE_DIR}/simo3d-config-in.h")

SET(all_src ${app_src} ${src_lua} "${PROJECT_SOURCE_DIR}/simo3d-config-in.h" ${src_glew} ${src_soil} ${src_jsoncpp} ${GUI_PROTO_SRCS} ${GUI_PROTO_HDRS}  ${pbjson_src})

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/external/pbjson/src/)

MESSAGE(STATUS "Linking with OpenGL ${OPENGL_LIBRARIES}")

IF(BUILD_USE_DOXYGEN)
	MESSAGE(STATUS "Doxygen exe is ${DOXYGEN_EXECUTABLE} with args ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile in dir ${CMAKE_SOURCE_DIR}/simo3d")
ENDIF(BUILD_USE_DOXYGEN)

ADD_EXECUTABLE(simo3d WIN32 MACOSX_BUNDLE ${all_src})
SET_TARGET_PROPERTIES(simo3d
                      PROPERTIES
                      RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/dist"
                      DEBUG_OUTPUT_NAME "simo3d-debug"
                      RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/dist"
                      RELEASE_OUTPUT_NAME "simo3d"
                      )

# SET_PROPERTY(TARGET simo3d PROPERTY CXX_STANDARD 11)
# SET_PROPERTY(TARGET simo3d PROPERTY CMAKE_CXX_STANDARD_REQUIRED ON)
TARGET_LINK_LIBRARIES(simo3d ${OPENGL_LIBRARIES} ${wxWidgets_LIBRARIES} ${ASSIMP_LIBRARIES} ${PROTOBUF_LIBRARIES} )
TARGET_LINK_LIBRARIES(simo3d ${CMAKE_DL_LIBS})
TARGET_LINK_LIBRARIES(simo3d ${CMAKE_THREAD_LIBS_INIT})

IF(BUILD_USE_CLANG_FORMAT)
	ADD_CUSTOM_COMMAND(TARGET simo3d
				             PRE_BUILD
				             COMMAND ./call-clang-format.py ${CMAKE_CURRENT_SOURCE_DIR}/*.cc ${CMAKE_CURRENT_SOURCE_DIR}/*.h  ${mainsrc}
				             WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
				             COMMENT "Running clang format" VERBATIM)
ENDIF(BUILD_USE_CLANG_FORMAT)

IF(BUILD_USE_CPPLINT)
	ADD_CUSTOM_COMMAND(TARGET simo3d
			        		   PRE_BUILD
			        		   COMMAND ./cpplint.py ARGS --output=vs7 ${CMAKE_CURRENT_SOURCE_DIR}/*.cc ${CMAKE_CURRENT_SOURCE_DIR}/*.h  ${mainsrc}
				        	   WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
				        	   COMMENT "Running CppLint" VERBATIM)
ENDIF(BUILD_USE_CPPLINT)

IF(BUILD_USE_DOXYGEN)
	ADD_CUSTOM_COMMAND(TARGET simo3d
					           PRE_LINK
				        	   COMMAND ${DOXYGEN_EXECUTABLE} ARGS ${CMAKE_CURRENT_BINARY_DIR}/../Doxyfile
				        	   WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
			        		   COMMENT "Running Doxygen" VERBATIM)
ENDIF(BUILD_USE_DOXYGEN)

