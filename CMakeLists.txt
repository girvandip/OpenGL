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

# Tugas 02 Car (shader)
add_executable(tugas02_car_shader
	tugas02_car_shader/main.cpp
	common/core/Engine.cpp
	common/core/Engine.hpp

  tugas02_car_shader/shader.fragment
	tugas02_car_shader/shader.vertex
)
target_link_libraries(tugas02_car_shader
	${ALL_LIBS}
)
# Xcode and Visual working directories
set_target_properties(tugas02_car_shader PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tugas02_car_shader/")
create_target_launcher(tugas02_car_shader WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/tugas02_car_shader/")

SOURCE_GROUP(common REGULAR_EXPRESSION ".*/common/.*" )
SOURCE_GROUP(shaders REGULAR_EXPRESSION ".*/.*hlsl" )

# Tugas 03 Car Camera
add_executable(tugas03_car_camera
	# tugas03_car_camera/main.cpp
	# tugas03_car_camera/cube.cpp
	# tugas03_car_camera/cube.hpp
	# tugas03_car_camera/camera.hpp
	# tugas03_car_camera/fs.glsl
	tugas03_car_camera/stb_image.h
	tugas03_car_camera/ImageLoader.h
	tugas03_car_camera/Shader.h
	tugas03_car_camera/CarViewer.cpp
	# tugas03_car_camera/vs.glsl
)
target_link_libraries(tugas03_car_camera
	${ALL_LIBS}
)
# Xcode and Visual working directories
set_target_properties(tugas03_car_camera PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tugas03_car_camera/")
create_target_launcher(tugas03_car_camera WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/tugas03_car_camera/")

# SOURCE_GROUP(cube REGULAR_EXPRESSION "cube.*" )
# SOURCE_GROUP(gals REGULAR_EXPRESSION "*.glsl" )

# Tugas 04 Car Lighting
add_executable(tugas04_car_lighting
	tugas04_car_lighting/main.cpp
	tugas04_car_lighting/stb_image.h
	tugas04_car_lighting/ImageLoader.h
	tugas04_car_lighting/Shader.h
)
target_link_libraries(tugas04_car_lighting
	${ALL_LIBS}
)
# Xcode and Visual working directories
set_target_properties(tugas04_car_lighting PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tugas04_car_lighting/")
create_target_launcher(tugas04_car_lighting WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/tugas04_car_lighting/")

# SOURCE_GROUP(cube REGULAR_EXPRESSION "cube.*" )
# SOURCE_GROUP(gals REGULAR_EXPRESSION "*.glsl" )


# Tugas 05 Car Particle
add_executable(tugas05_car_particle
	tugas05_car_particle/main.cpp
	tugas05_car_particle/stb_image.h
	tugas05_car_particle/ImageLoader.h
	tugas05_car_particle/Shader.h
)
target_link_libraries(tugas05_car_particle
	${ALL_LIBS}
)
# Xcode and Visual working directories
set_target_properties(tugas05_car_particle PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tugas05_car_particle/")
create_target_launcher(tugas05_car_particle WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/tugas05_car_particle/")

# SOURCE_GROUP(cube REGULAR_EXPRESSION "cube.*" )
# SOURCE_GROUP(gals REGULAR_EXPRESSION "*.glsl" )

if (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )
add_custom_command(
   TARGET tugas01_triangle POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/tugas01_triangle${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/tugas01_triangle/"
)

add_custom_command(
   TARGET tugas02_car POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/tugas02_car${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/tugas02_car/"
)

add_custom_command(
   TARGET tugas03_car_camera POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/tugas03_car_camera${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/tugas03_car_camera/"
)

add_custom_command(
   TARGET tugas01_triangle_shader POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/tugas01_triangle_shader${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/tugas01_triangle_shader/"
)

add_custom_command(
   TARGET tugas02_car_shader POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/tugas02_car_shader${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/tugas02_car_shader/"
)

add_custom_command(
   TARGET tugas04_car_lighting POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/tugas04_car_lighting${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/tugas04_car_lighting/"
)

add_custom_command(
   TARGET tugas05_car_particle POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/tugas05_car_particle${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/tugas05_car_particle/"
)

elseif (${CMAKE_GENERATOR} MATCHES "Xcode" )

endif (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )
