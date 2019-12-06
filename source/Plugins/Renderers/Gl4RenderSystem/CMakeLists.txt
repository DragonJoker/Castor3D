project( Gl4RenderSystem )

set( Castor3DBinsDependencies 
	${Castor3DBinsDependencies}
	castor::${PROJECT_NAME}
	PARENT_SCOPE
)

set( AshesLibraries
	${AshesLibraries}
	ashes::Gl4Renderer
	PARENT_SCOPE
)

set( ShaderWriterLibraries
	${ShaderWriterLibraries}
	sdw::CompilerGlsl
	PARENT_SCOPE
)

set( ${PROJECT_NAME}_DESCRIPTION "OpenGL 4.2 Renderer" )
set( ${PROJECT_NAME}_VERSION_MAJOR 0 )
set( ${PROJECT_NAME}_VERSION_MINOR 11 )
set( ${PROJECT_NAME}_VERSION_BUILD 0 )

add_target(
	${PROJECT_NAME}
	plugin
	${CASTOR_SOURCE_DIR}/include/Plugins/Renderers/${PROJECT_NAME}
	${CASTOR_SOURCE_DIR}/source/Plugins/Renderers/${PROJECT_NAME}
	""
	""
	${CASTOR_SOURCE_DIR}/include/Plugins/Renderers/${PROJECT_NAME}/${PROJECT_NAME}Pch.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Renderers/${PROJECT_NAME}/${PROJECT_NAME}Pch.cpp
)
add_library( castor::${PROJECT_NAME}
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
		ashes::Gl4Renderer
		sdw::CompilerGlsl
)

set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Plugins/Renderers" )
set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
