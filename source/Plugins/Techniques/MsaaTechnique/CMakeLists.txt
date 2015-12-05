project( MsaaTechnique )

set( CastorBinsDependencies 
	${CastorBinsDependencies}
	${PROJECT_NAME}
	PARENT_SCOPE
)

set( ${PROJECT_NAME}_DESCRIPTION "MSAA RenderTechnique" )
set( ${PROJECT_NAME}_VERSION_MAJOR	0 )
set( ${PROJECT_NAME}_VERSION_MINOR	8 )
set( ${PROJECT_NAME}_VERSION_BUILD	0 )

if( ${CASTOR_BUILD_RENDERER_OGL} )
	add_definitions( "-DC3D_HAS_GL_RENDERER" )
	include_directories( ${CMAKE_SOURCE_DIR}/Plugins/Renderers/GlRenderSystem/Src )
	include_directories( ${CMAKE_SOURCE_DIR}/Plugins/Renderers/GlRenderSystem/Src/GLSL )
endif ()
if( ${CASTOR_BUILD_RENDERER_D3D11} )
	add_definitions( "-DC3D_HAS_D3D11_RENDERER" )
	include_directories( ${CMAKE_SOURCE_DIR}/Plugins/Renderers/Dx11RenderSystem/Src )
	include_directories( ${D3D9_INCLUDE_DIR} )
	include_directories( ${D3D11_INCLUDE_DIR} )
endif ()

include_directories( ${CMAKE_SOURCE_DIR}/Core/CastorUtils/Src )
include_directories( ${CMAKE_SOURCE_DIR}/Core/Castor3D/Src )
include_directories( ${CMAKE_BINARY_DIR}/Core/CastorUtils/Src )

add_target(
	${PROJECT_NAME}
	plugin
	"Castor3D"
	"${TechniqueLibraries}"
)

set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Plugins/Techniques" )
set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
