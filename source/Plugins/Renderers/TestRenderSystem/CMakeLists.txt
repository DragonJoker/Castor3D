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

include_directories(
	${Castor3DIncludeDirs}
	${CMAKE_SOURCE_DIR}/${ASHES_DIR}/include/TestRenderer
	${CMAKE_BINARY_DIR}/${ASHES_DIR}/include/TestRenderer
	${CASTOR_SOURCE_DIR}/include/Plugins/Renderers
	${CASTOR_BINARY_DIR}/include/Plugins/Renderers
)

add_target(
	${PROJECT_NAME}
	plugin
	${CASTOR_SOURCE_DIR}/include/Plugins/Renderers/${PROJECT_NAME}
	${CASTOR_SOURCE_DIR}/source/Plugins/Renderers/${PROJECT_NAME}
	"Castor3D"
	"${CastorMinLibraries};TestRenderer;CompilerGlsl"
	${CASTOR_SOURCE_DIR}/include/Plugins/Renderers/${PROJECT_NAME}/${PROJECT_NAME}Pch.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Renderers/${PROJECT_NAME}/${PROJECT_NAME}Pch.cpp
	"-DTestRenderer_STATIC"
	"-DTestRenderer_STATIC"
)

set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Plugins/Renderers" )
set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
