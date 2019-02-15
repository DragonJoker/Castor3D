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

include_directories(
	${RendererIncludes}
	${CASTOR_SOURCE_DIR}/${ASHES_DIR}/include/VkRenderer
	${CASTOR_BINARY_DIR}/${ASHES_DIR}/include/VkRenderer
)

if ( EXISTS ${CASTOR_SOURCE_DIR}/${ASHES_DIR}/../external/SPIRV-Cross/spirv_glsl.cpp )
	set( SPIRV_CROSS_DIR
		${CASTOR_SOURCE_DIR}/${ASHES_DIR}/../external/SPIRV-Cross
	)
	set( SPIRV_FILES
		${SPIRV_CROSS_DIR}/GLSL.std.450.h
		${SPIRV_CROSS_DIR}/spirv_common.hpp
		${SPIRV_CROSS_DIR}/spirv.hpp
		${SPIRV_CROSS_DIR}/spirv_cross.hpp
		${SPIRV_CROSS_DIR}/spirv_cfg.hpp
		${SPIRV_CROSS_DIR}/spirv_glsl.hpp
		${SPIRV_CROSS_DIR}/spirv_cross.cpp
		${SPIRV_CROSS_DIR}/spirv_cfg.cpp
		${SPIRV_CROSS_DIR}/spirv_glsl.cpp
	)
	include_directories( ${SPIRV_CROSS_DIR} )
	add_definitions( -DC3DVkRenderer_HasSPIRVCross )
endif ()

add_target(
	${PROJECT_NAME}
	plugin
	${CASTOR_SOURCE_DIR}/include/Plugins/Renderers/${PROJECT_NAME}
	${CASTOR_SOURCE_DIR}/source/Plugins/Renderers/${PROJECT_NAME}
	"Castor3D"
	"${RendererLibraries};VkRenderer;CompilerSpirV"
	${CASTOR_SOURCE_DIR}/include/Plugins/Renderers/${PROJECT_NAME}/${PROJECT_NAME}Pch.hpp
	${CASTOR_SOURCE_DIR}/source/Plugins/Renderers/${PROJECT_NAME}/${PROJECT_NAME}Pch.cpp
	"-DVkRenderer_STATIC"
	"-DVkRenderer_STATIC"
	""
	"${SPIRV_FILES}"
)

set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Plugins/Renderers" )
set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
