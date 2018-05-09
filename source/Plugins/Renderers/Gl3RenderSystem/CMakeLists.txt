project( Gl3RenderSystem )

set( CastorBinsDependencies 
	${CastorBinsDependencies}
	${PROJECT_NAME}
	PARENT_SCOPE
)

set( ${PROJECT_NAME}_DESCRIPTION "OpenGL 3.2 Renderer" )
set( ${PROJECT_NAME}_VERSION_MAJOR 0 )
set( ${PROJECT_NAME}_VERSION_MINOR 1 )
set( ${PROJECT_NAME}_VERSION_BUILD 0 )

include_directories( ${CMAKE_SOURCE_DIR}/Core/CastorUtils/Src )
include_directories( ${CMAKE_SOURCE_DIR}/Core/GlslWriter/Src )
include_directories( ${CMAKE_SOURCE_DIR}/Core/Castor3D/Src )
include_directories( ${CMAKE_SOURCE_DIR}/${RENDERERLIB_DIR}/Gl3Renderer/Src )
include_directories( ${CMAKE_SOURCE_DIR}/${RENDERERLIB_DIR}/Gl3RenderSystem/Src )
include_directories( ${CMAKE_BINARY_DIR}/Core/CastorUtils/Src )

add_target(
	${PROJECT_NAME}
	api_plugin
	"Castor3D"
	"${CastorMinLibraries};${OpenGlLibraries};Gl3Renderer"
	"Gl3RenderSystemPch.hpp"
	"Gl3RenderSystemPch.cpp"
	"-DGl3Renderer_STATIC"
	"-DGl3Renderer_STATIC"
)

set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Plugins/Renderers" )
set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
