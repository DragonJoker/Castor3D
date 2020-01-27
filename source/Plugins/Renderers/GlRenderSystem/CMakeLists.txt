set( FOLDER_NAME GlRenderSystem )
project( castor3d${FOLDER_NAME} )

set( Castor3DBinsDependencies 
	${Castor3DBinsDependencies}
	castor::${FOLDER_NAME}
	PARENT_SCOPE
)

set( ${PROJECT_NAME}_DESCRIPTION "OpenGL Renderer" )
set( ${PROJECT_NAME}_VERSION_MAJOR 0 )
set( ${PROJECT_NAME}_VERSION_MINOR 11 )
set( ${PROJECT_NAME}_VERSION_BUILD 0 )

add_target(
	${PROJECT_NAME}
	plugin
	${CASTOR_SOURCE_DIR}/source/Plugins/Renderers/${FOLDER_NAME}
	${CASTOR_SOURCE_DIR}/source/Plugins/Renderers/${FOLDER_NAME}
	""
	""
	${CASTOR_SOURCE_DIR}/source/Plugins/Renderers/${FOLDER_NAME}/${FOLDER_NAME}Pch.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Renderers/${FOLDER_NAME}/${FOLDER_NAME}Pch.cpp
)
add_library( castor::${FOLDER_NAME}
	ALIAS
	${PROJECT_NAME}
)
target_include_directories( ${PROJECT_NAME}
	PRIVATE
		${RendererIncludes}
)
target_compile_definitions( ${PROJECT_NAME}
	PRIVATE
		${COMPILE_DEFINITIONS}
)
target_link_libraries( ${PROJECT_NAME}
	PUBLIC
		castor::Castor3D
	PRIVATE
		ashes::RendererCommon
		ashes::GlRenderer
		sdw::CompilerGlsl
)

set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Plugins/Renderers" )
set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
