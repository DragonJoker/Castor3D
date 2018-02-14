project( Renderer )

SET( CastorBinsDependencies 
	${CastorBinsDependencies}
	${PROJECT_NAME}
	PARENT_SCOPE
)

set( CastorMinLibraries
	${CastorMinLibraries}
	${PROJECT_NAME}
	PARENT_SCOPE
)

set( ${PROJECT_NAME}_DESCRIPTION "Renderer API" )
set( ${PROJECT_NAME}_VERSION_MAJOR 0 )
set( ${PROJECT_NAME}_VERSION_MINOR 1 )
set( ${PROJECT_NAME}_VERSION_BUILD 0 )

include_directories( ${CMAKE_SOURCE_DIR}/Core/CastorUtils/Src )
include_directories( ${CMAKE_BINARY_DIR}/Core/CastorUtils/Src )
include_directories( ${CMAKE_SOURCE_DIR}/Core/GlslWriter/Src )
include_directories( ${CMAKE_BINARY_DIR}/Core/GlslWriter/Src )
include_directories( ${CMAKE_BINARY_DIR}/Core/Renderer/Src )

set( RENDERLIB_WIN32 0 )
set( RENDERLIB_XCB 0 )
set( RENDERLIB_XLIB 0 )
set( RENDERLIB_ANDROID 0 )

if ( WIN32 )
	set( RENDERLIB_WIN32 1 )
elseif ( ANDROID )
	set( RENDERLIB_ANDROID 1 )
else ()
	set( RENDERLIB_XLIB 1 )
endif ()

set( ${PROJECT_NAME}_CONFIG_HEADER
	${CMAKE_CURRENT_BINARY_DIR}/Src/RendererConfig.hpp
)

configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/Src/RendererConfig.hpp.in
	${${PROJECT_NAME}_CONFIG_HEADER}
	NEWLINE_STYLE LF
)

add_target(
	${PROJECT_NAME}
	lib
	"CastorUtils;GlslWriter"
	"${CastorMinLibraries}"
	"RendererPch.hpp"
	"RendererPch.cpp"
	""
	""
	""
	"${${PROJECT_NAME}_CONFIG_HEADER}"
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

install(
	FILES ${CMAKE_CURRENT_BINARY_DIR}/Src/RendererConfig.hpp
	DESTINATION include/${PROJECT_NAME}
	COMPONENT ${PROJECT_NAME}_dev
)

set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Renderer" )
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
target_add_doc( ${PROJECT_NAME} "French" "*.h *.hpp" )
target_add_doc( ${PROJECT_NAME} "English" "*.h *.hpp" )
set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
