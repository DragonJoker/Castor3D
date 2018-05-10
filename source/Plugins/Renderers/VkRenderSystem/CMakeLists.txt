project( VkRenderSystem )

SET( Castor3DBinsDependencies 
	${Castor3DBinsDependencies}
	${PROJECT_NAME}
	PARENT_SCOPE
)

set( ${PROJECT_NAME}_DESCRIPTION "Vulkan Renderer" )
set( ${PROJECT_NAME}_VERSION_MAJOR 0 )
set( ${PROJECT_NAME}_VERSION_MINOR 1 )
set( ${PROJECT_NAME}_VERSION_BUILD 0 )

include_directories( ${Castor3DIncludeDirs} )
include_directories( ${CMAKE_SOURCE_DIR}/${RENDERERLIB_DIR}/VkRenderer/Src )
include_directories( ${CMAKE_BINARY_DIR}/${RENDERERLIB_DIR}/VkRenderer/Src )
include_directories( ${CMAKE_SOURCE_DIR}/Plugins/Renderers/VkRenderSystem/Src )

add_target(
	${PROJECT_NAME}
	api_plugin
	"Castor3D"
	"${CastorMinLibraries};VkRenderer"
	"VkRenderSystemPch.hpp"
	"VkRenderSystemPch.cpp"
	"-DVkRenderer_STATIC"
	"-DVkRenderer_STATIC"
)

set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Plugins/Renderers" )
set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
