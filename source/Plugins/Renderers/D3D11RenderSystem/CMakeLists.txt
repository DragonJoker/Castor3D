project( D3D11RenderSystem )

set( Castor3DBinsDependencies 
	${Castor3DBinsDependencies}
	${PROJECT_NAME}
	PARENT_SCOPE
)

set( ${PROJECT_NAME}_DESCRIPTION "Direct3D 11 Renderer" )
set( ${PROJECT_NAME}_VERSION_MAJOR 0 )
set( ${PROJECT_NAME}_VERSION_MINOR 1 )
set( ${PROJECT_NAME}_VERSION_BUILD 0 )

include_directories( ${Castor3DIncludeDirs} )
include_directories( ${CMAKE_SOURCE_DIR}/${ASHES_DIR}/D3D11Renderer/Src )
include_directories( ${CMAKE_BINARY_DIR}/${ASHES_DIR}/D3D11Renderer/Src )
include_directories( ${CMAKE_SOURCE_DIR}/Plugins/Renderers/D3D11RenderSystem/Src )

add_target(
	${PROJECT_NAME}
	plugin
	"Castor3D"
	"${CastorMinLibraries};D3D11Renderer;CompilerHlsl"
	${CMAKE_CURRENT_SOURCE_DIR}/Src/D3D11RenderSystemPch.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/D3D11RenderSystemPch.cpp
	"-DD3D11Renderer_STATIC"
	"-DD3D11Renderer_STATIC"
	""
	"${SPIRV_FILES}"
)

set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Plugins/Renderers" )
set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
