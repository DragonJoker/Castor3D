project( VkRenderer )

option( VKRENDERER_GLSL_TO_SPV "Enable Compiling from GLSL to SPIR-V, using glslang and SPIRV libraries" TRUE )

set( ${PROJECT_NAME}_DESCRIPTION "Vulkan Rendering API" )
set( ${PROJECT_NAME}_VERSION_MAJOR 0 )
set( ${PROJECT_NAME}_VERSION_MINOR 1 )
set( ${PROJECT_NAME}_VERSION_BUILD 0 )

if ( VKRENDERER_GLSL_TO_SPV )
	set( VKRENDERER_GLSL_TO_SPV 1 )
	find_package( glslang )
	find_package( SPIRV )

	if ( glslang_FOUND AND SPIRV_FOUND )
		include_directories(
			${glslang_INCLUDE_DIR}
			${SPIRV_INCLUDE_DIR}
		)
		set( BinaryLibraries
			${BinaryLibraries}
			PARENT_SCOPE
		)
	endif ()
else ()
	set( VKRENDERER_GLSL_TO_SPV 0 )
	set( SPIRV_FOUND TRUE )
	set( glslang_FOUND TRUE )
endif ()

set( ${PROJECT_NAME}_CONFIG_HEADER
	${CMAKE_CURRENT_BINARY_DIR}/Src/VkRendererConfig.hpp
)

configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/Src/VkRendererConfig.hpp.in
	${${PROJECT_NAME}_CONFIG_HEADER}
	NEWLINE_STYLE LF
)

if ( glslang_FOUND AND SPIRV_FOUND )
	include_directories(
		${CMAKE_SOURCE_DIR}/Core/CastorUtils/Src
		${CMAKE_BINARY_DIR}/Core/Renderer/Renderer/Src
		${CMAKE_SOURCE_DIR}/Core/Renderer/Renderer/Src
		${CMAKE_BINARY_DIR}/Plugins/Renderers/VkRenderer/Src
		${CMAKE_SOURCE_DIR}/Plugins/Renderers/VkRenderer/Src
	)

	add_definitions( -DVkRenderer_STATIC_LIB )

	add_target(
		${PROJECT_NAME}
		lib
		"Renderer"
		"CastorUtils;Renderer"
		"VkRendererPch.hpp"
		"VkRendererPch.cpp"
	)

	parse_subdir_files( Buffer "Buffer" )
	parse_subdir_files( Command "Command" )
	parse_subdir_files( Core "Core" )
	parse_subdir_files( Descriptor "Descriptor" )
	parse_subdir_files( Enum "Enum" )
	parse_subdir_files( Image "Image" )
	parse_subdir_files( Miscellaneous "Miscellaneous" )
	parse_subdir_files( Pipeline "Pipeline" )
	parse_subdir_files( RenderPass "RenderPass" )
	parse_subdir_files( Shader "Shader" )
	parse_subdir_files( Sync "Sync" )
	parse_subdir_files( vulkan "vulkan" )

	set_property( TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14 )
	set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Renderer" )
else ()
	if ( NOT glslang_FOUND )
		message( SEND_ERROR "glslang library not found." )
	endif ()
	if ( NOT SPIRV_FOUND )
		message( SEND_ERROR "SPIRV library not found." )
	endif ()
endif ()
