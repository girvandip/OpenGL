# CMake entry point
cmake_minimum_required (VERSION 3.0)
project (OpenGL-Graphics)

find_package(OpenGL REQUIRED)

SET(GCC_COVERAGE_COMPILE_FLAGS "-std=c++1z")
SET(GCC_COVERAGE_LINK_FLAGS    "-std=c++1z")

SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )
SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}" )


if( CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR )
    message( FATAL_ERROR "Please select another Build Directory ! (and give it a clever name, like bin_Visual2012_64bits/)" )
endif()
if( CMAKE_SOURCE_DIR MATCHES " " )
	message( "Your Source Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()
if( CMAKE_BINARY_DIR MATCHES " " )
	message( "Your Build Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()



# Compile external dependencies
add_subdirectory (libs)

# On Visual 2005 and above, this module can set the debug working directory
cmake_policy(SET CMP0026 OLD)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/libs/rpavlik-cmake-modules")
include(CreateLaunchers)
include(MSVCMultipleProcessCompile) # /MP


include_directories(
	libs/glfw-3.2.1/include/
  libs/glm-0.9.9-a2/
	libs/glew-2.1.0/include/
	.
)

set(ALL_LIBS
	${OPENGL_LIBRARY}
	glfw
	GLEW_210
)

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
)

# Tugas 01 Triangle (no shader)
add_executable(tugas01_triangle
	tugas01_triangle/main.cpp
)
target_link_libraries(tugas01_triangle
	${ALL_LIBS}
)
# Xcode and Visual working directories
set_target_properties(tugas01_triangle PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tugas01_triangle/")
create_target_launcher(tugas01_triangle WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/tugas01_triangle/")

# Tugas 02 Car (no shader)
add_executable(tugas02_car
	tugas02_car/main.cpp
)
target_link_libraries(tugas02_car
	${ALL_LIBS}
)
# Xcode and Visual working directories
set_target_properties(tugas02_car PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tugas02_car/")
create_target_launcher(tugas02_car WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/tugas02_car/")


# Tugas 01 Triangle (shader)
add_executable(tugas01_triangle_shader
	tugas01_triangle_shader/main.cpp
	common/core/Engine.cpp
	common/core/Engine.hpp

  tugas01_triangle_shader/shader.fragment
	tugas01_triangle_shader/shader.vertex
)
target_link_libraries(tugas01_triangle_shader
	${ALL_LIBS}
)
# Xcode and Visual working directories
set_target_properties(tugas01_triangle_shader PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tugas01_triangle_shader/")
create_target_launcher(tugas01_triangle_shader WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/tugas01_triangle_shader/")

SOURCE_GROUP(common REGULAR_EXPRESSION ".*/common/.*" )
SOURCE_GROUP(shaders REGULAR_EXPRESSION ".*/.*hlsl" )

if (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )
add_custom_command(
   TARGET tugas01_triangle POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/tugas01_triangle${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/tugas01_triangle/"
)

add_custom_command(
   TARGET tugas01_triangle_shader POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/tugas01_triangle_shader${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/tugas01_triangle_shader/"
)

elseif (${CMAKE_GENERATOR} MATCHES "Xcode" )

endif (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )
