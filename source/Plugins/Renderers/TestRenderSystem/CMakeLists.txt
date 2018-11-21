project( TestRenderSystem )

set( Castor3DBinsDependencies 
	${Castor3DBinsDependencies}
	${PROJECT_NAME}
	PARENT_SCOPE
)

set( ${PROJECT_NAME}_DESCRIPTION "Test Renderer" )
set( ${PROJECT_NAME}_VERSION_MAJOR 0 )
set( ${PROJECT_NAME}_VERSION_MINOR 11 )
set( ${PROJECT_NAME}_VERSION_BUILD 0 )

include_directories( ${Castor3DIncludeDirs} )
include_directories( ${CMAKE_SOURCE_DIR}/${ASHES_DIR}/TestRenderer/Src )
include_directories( ${CMAKE_BINARY_DIR}/${ASHES_DIR}/TestRenderer/Src )
include_directories( ${CMAKE_SOURCE_DIR}/Plugins/Renderers/TestRenderSystem/Src )

add_target(
	${PROJECT_NAME}
	plugin
	"Castor3D"
	"${CastorMinLibraries};TestRenderer"
	${CMAKE_CURRENT_SOURCE_DIR}/Src/TestRenderSystemPch.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/TestRenderSystemPch.cpp
	"-DTestRenderer_STATIC"
	"-DTestRenderer_STATIC"
)

set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Plugins/Renderers" )
set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
