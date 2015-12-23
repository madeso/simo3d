FILE(GLOB app_src_glob *.cc;*.h;*.inl)
set(app_src ${app_src_glob} "${PROJECT_BINARY_DIR}/simo3d-config.h")

source_group("" FILES ${app_src})
source_group(cmake FILES "${PROJECT_SOURCE_DIR}/simo3d-config-in.h")

set(all_src ${app_src} "${PROJECT_SOURCE_DIR}/simo3d-config-in.h" ${src_glew} ${src_soil} ${src_jsoncpp} ${src_lua})

message(STATUS "Linking with OpenGL ${OPENGL_LIBRARIES}")

if(BUILD_USE_DOXYGEN)
	message(STATUS "Doxygen exe is ${DOXYGEN_EXECUTABLE} with args ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile in dir ${CMAKE_SOURCE_DIR}/simo3d")
endif(BUILD_USE_DOXYGEN)

add_executable(simo3d WIN32 ${all_src})

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

target_link_libraries(simo3d ${OPENGL_LIBRARIES} ${wxWidgets_LIBRARIES} ${ASSIMP_LIBRARIES} )