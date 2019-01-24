project( GlES3RenderSystem )

SET( Castor3DBinsDependencies 
	${Castor3DBinsDependencies}
	${PROJECT_NAME}
	PARENT_SCOPE
)

set( ${PROJECT_NAME}_DESCRIPTION "OpenGL ES 3.0 Renderer" )
set( ${PROJECT_NAME}_VERSION_MAJOR 0 )
set( ${PROJECT_NAME}_VERSION_MINOR 11 )
set( ${PROJECT_NAME}_VERSION_BUILD 0 )

include_directories( ${Castor3DIncludeDirs} )
include_directories( ${CMAKE_SOURCE_DIR}/Plugins/Renderers/GlES3RenderSystem/Src )

add_target(
	${PROJECT_NAME}
	plugin
	"Castor3D"
	"${CastorMinLibraries};EGL;GLESv3"
	${CMAKE_CURRENT_SOURCE_DIR}/Src/GlES3RenderSystemPch.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/GlES3RenderSystemPch.cpp
)

parse_subdir_files( Src/Buffer Buffer )
parse_subdir_files( Src/Common Common  )
parse_subdir_files( Src/FrameBuffer FrameBuffer )
parse_subdir_files( Src/Mesh Mesh )
parse_subdir_files( Src/Miscellaneous Miscellaneous )
parse_subdir_files( Src/Render Render )
parse_subdir_files( Src/Shader Shader )
parse_subdir_files( Src/State State )
parse_subdir_files( Src/Texture Texture )

set( Ogl "yes" PARENT_SCOPE )

set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Plugins/Renderers" )
set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
