project( GlES3RenderSystem )

SET( CastorBinsDependencies 
	${CastorBinsDependencies}
	${PROJECT_NAME}
	PARENT_SCOPE
)

set( ${PROJECT_NAME}_DESCRIPTION "OpenGL ES 3.0 Renderer" )
set( ${PROJECT_NAME}_VERSION_MAJOR 0 )
set( ${PROJECT_NAME}_VERSION_MINOR 10 )
set( ${PROJECT_NAME}_VERSION_BUILD 0 )

include_directories( ${CMAKE_SOURCE_DIR}/Core/CastorUtils/Src )
include_directories( ${CMAKE_SOURCE_DIR}/Core/GlslWriter/Src )
include_directories( ${CMAKE_SOURCE_DIR}/Core/Castor3D/Src )
include_directories( ${CMAKE_SOURCE_DIR}/Plugins/Renderers/GlES3RenderSystem/Src )
include_directories( ${CMAKE_BINARY_DIR}/Core/CastorUtils/Src )

add_target(
	${PROJECT_NAME}
	api_plugin
	"Castor3D"
	"${CastorMinLibraries};EGL;GLESv3"
	"GlES3RenderSystemPch.hpp"
	"GlES3RenderSystemPch.cpp"
)

parse_subdir_files( Buffer Buffer )
parse_subdir_files( Common Common  )
parse_subdir_files( FrameBuffer FrameBuffer )
parse_subdir_files( Mesh Mesh )
parse_subdir_files( Miscellaneous Miscellaneous )
parse_subdir_files( Render Render )
parse_subdir_files( Shader Shader )
parse_subdir_files( State State )
parse_subdir_files( Texture Texture )

set( Ogl "yes" PARENT_SCOPE )

set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Plugins/Renderers" )
set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
