project( D3D11RenderSystem )

set( Castor3DBinsDependencies 
	${Castor3DBinsDependencies}
	${PROJECT_NAME}
	PARENT_SCOPE
)

set( AshesLibraries
	${AshesLibraries}
	D3D11Renderer
	PARENT_SCOPE
)

set( ShaderWriterLibraries
	${ShaderWriterLibraries}
	CompilerHlsl
	PARENT_SCOPE
)

set( ${PROJECT_NAME}_DESCRIPTION "Direct3D 11 Renderer" )
set( ${PROJECT_NAME}_VERSION_MAJOR 0 )
set( ${PROJECT_NAME}_VERSION_MINOR 1 )
set( ${PROJECT_NAME}_VERSION_BUILD 0 )

include_directories(
	${RendererIncludes}
	${CASTOR_SOURCE_DIR}/${ASHES_DIR}/include/D3D11Renderer
	${CASTOR_BINARY_DIR}/${ASHES_DIR}/include/D3D11Renderer
)

add_target(
	${PROJECT_NAME}
	plugin
	${CASTOR_SOURCE_DIR}/include/Plugins/Renderers/${PROJECT_NAME}
	${CASTOR_SOURCE_DIR}/source/Plugins/Renderers/${PROJECT_NAME}
	"Castor3D"
	"${RendererLibraries};D3D11Renderer;CompilerHlsl"
	${CASTOR_SOURCE_DIR}/include/Plugins/Renderers/${PROJECT_NAME}/${PROJECT_NAME}Pch.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Renderers/${PROJECT_NAME}/${PROJECT_NAME}Pch.cpp
	"-DD3D11Renderer_STATIC"
	"-DD3D11Renderer_STATIC"
)

set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Plugins/Renderers" )
set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
