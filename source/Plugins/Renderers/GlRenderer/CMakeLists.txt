project( GlRenderer )

set( ${PROJECT_NAME}_DESCRIPTION "OpenGL Rendering API" )
set( ${PROJECT_NAME}_VERSION_MAJOR 0 )
set( ${PROJECT_NAME}_VERSION_MINOR 1 )
set( ${PROJECT_NAME}_VERSION_BUILD 0 )

find_package( OpenGL )

if ( OpenGL_FOUND )
	set( OpenGlLibraries "" )
	foreach( Lib ${OPENGL_gl_LIBRARY} )
		if( OpenGlLibraries )
			set( OpenGlLibraries "${OpenGlLibraries}|${Lib}" )
		else()
			set( OpenGlLibraries "${Lib}" )
		endif()
	endforeach()

	include_directories(
		${CMAKE_SOURCE_DIR}/Core/CastorUtils/Src
		${CMAKE_BINARY_DIR}/Core/Renderer/Renderer/Src
		${CMAKE_SOURCE_DIR}/Core/Renderer/Renderer/Src
		${CMAKE_BINARY_DIR}/Plugins/Renderers/GlRenderer/Src
		${CMAKE_SOURCE_DIR}/Plugins/Renderers/GlRenderer/Src
	)

	add_definitions( -DGlRenderer_STATIC_LIB )

	add_target(
		${PROJECT_NAME}
		lib
		"Renderer"
		"CastorUtils;Renderer"
		"GlRendererPch.hpp"
		"GlRendererPch.cpp"
	)

	parse_subdir_files( Buffer "Buffer" )
	parse_subdir_files( Command "Command" )
	parse_subdir_files( Command/Commands "Command\\\\Commands" )
	parse_subdir_files( Core "Core" )
	parse_subdir_files( Descriptor "Descriptor" )
	parse_subdir_files( Enum "Enum" )
	parse_subdir_files( Image "Image" )
	parse_subdir_files( Miscellaneous "Miscellaneous" )
	parse_subdir_files( Pipeline "Pipeline" )
	parse_subdir_files( RenderPass "RenderPass" )
	parse_subdir_files( Shader "Shader" )
	parse_subdir_files( Sync "Sync" )
	set_property( TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14 )
	set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Renderer" )
else ()
	if ( NOT OpenGL_FOUND )
		message( SEND_ERROR "OpenGL library not found." )
	endif ()
endif ()
