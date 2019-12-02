get_filename_component( CASTOR_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY )
get_filename_component( CASTOR_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR} DIRECTORY )

include_directories(
	${CASTOR_SOURCE_DIR}/include/Core
	${CASTOR_BINARY_DIR}/include/Core
)

find_package( Vulkan )

if( Vulkan_FOUND )
	if ( APPLE )
		add_definitions( -DVK_USE_PLATFORM_MACOS_MVK )
	elseif ( WIN32 )
		add_definitions( -DVK_USE_PLATFORM_WIN32_KHR )
	elseif ( ANDROID )
		add_definitions( -DVK_USE_PLATFORM_ANDROID_KHR )
	else ()
		add_definitions( -DVK_USE_PLATFORM_XLIB_KHR )
	endif ()
	set( Castor3DIncludeDirs
		${CASTOR_SOURCE_DIR}/${ASHES_DIR}/include
		${CASTOR_BINARY_DIR}/${ASHES_DIR}/include
		${CMAKE_BINARY_DIR}/include
		${CASTOR_BINARY_DIR}/${ASHES_DIR}/source
		${CASTOR_SOURCE_DIR}/${SHADERWRITER_DIR}/include
		${CASTOR_BINARY_DIR}/${SHADERWRITER_DIR}/source
		${CASTOR_SOURCE_DIR}/include/Core
		${CASTOR_SOURCE_DIR}/include/Core
		${CASTOR_BINARY_DIR}/include/Core
		${CASTOR_BINARY_DIR}/source/Core
		${Vulkan_INCLUDE_DIRS}
	)

	add_subdirectory( Core )
	if ( ${CASTOR_BUILD_CASTOR3D} )
		add_subdirectory( Plugins )
		add_subdirectory( Samples )
		add_subdirectory( Interop )
	endif ()
	add_subdirectory( Tools )

	if ( ${CASTOR_BUILD_CASTOR3D} )
		# Ashes external library
		if ( NOT CASTOR_BUILDGRP_RENDERER )
			set( ASHES_BUILD_RENDERER_PLUGINS FALSE )
		else ()
			set( ASHES_BUILD_RENDERER_PLUGINS TRUE )
			set( ASHES_BUILD_RENDERER_OGL3 OFF )
			set( ASHES_BUILD_RENDERER_OGL4 ${CASTOR_BUILD_RENDERER_OGL4} )
			set( ASHES_BUILD_RENDERER_VKN ${CASTOR_BUILD_RENDERER_VKN} )
			set( ASHES_BUILD_RENDERER_D3D11 ${CASTOR_BUILD_RENDERER_D3D11} )
			set( ASHES_BUILD_RENDERER_TEST ${CASTOR_BUILDGRP_TEST} )
		endif ()
		add_subdirectory( ../${ASHES_DIR}/source ${CMAKE_BINARY_DIR}/${ASHES_DIR} )
		set( AshesLibraries
			ashes::common
			ashes::ashes
			ashes::ashespp
		)
		set_property( TARGET ashes_common PROPERTY FOLDER "External/Ashes/Core" )
		set_property( TARGET ashes PROPERTY FOLDER "External/Ashes/Core" )
		set_property( TARGET ashespp PROPERTY FOLDER "External/Ashes/Core" )
		if ( CASTOR_BUILDGRP_RENDERER )
			if ( TARGET Gl3Renderer )
				set_property( TARGET Gl3Renderer PROPERTY FOLDER "External/Ashes/Renderers" )
			endif ()
			if ( TARGET Gl4Renderer )
				set_property( TARGET Gl4Renderer PROPERTY FOLDER "External/Ashes/Renderers" )
			endif ()
			if ( TARGET VkRenderer )
				set_property( TARGET VkRenderer PROPERTY FOLDER "External/Ashes/Renderers" )
			endif ()
			if ( TARGET D3D11Renderer )
				set_property( TARGET D3D11Renderer PROPERTY FOLDER "External/Ashes/Renderers" )
			endif ()
			if ( TARGET TestRenderer )
				set_property( TARGET TestRenderer PROPERTY FOLDER "External/Ashes/Renderers" )
			endif ()
			if ( TARGET RendererCommon )
				set_property( TARGET RendererCommon PROPERTY FOLDER "External/Ashes/Renderers" )
			endif ()
			if ( TARGET GlRendererCommon )
				set_property( TARGET ashes::GlRendererCommon PROPERTY FOLDER "External/Ashes/Renderers" )
			endif ()
		endif ()

		if ( CASTOR_USE_GLSLANG )
			set( ENABLE_GLSLANG_BINARIES FALSE CACHE BOOL "Builds glslangValidator and spirv-remap" FORCE )
			set( ENABLE_AMD_EXTENSIONS FALSE CACHE BOOL "Enables support of AMD-specific extensions" FORCE )
			set( ENABLE_NV_EXTENSIONS FALSE CACHE BOOL "Enables support of Nvidia-specific extensions" FORCE )
			set( ENABLE_HLSL FALSE CACHE BOOL "Enables HLSL input support" FORCE )
			set( BUILD_TESTING OFF )
			set( SKIP_GLSLANG_INSTALL ON )
			add_subdirectory( ../external/glslang ${CMAKE_BINARY_DIR}/external/glslang )
			set_property( TARGET glslang PROPERTY FOLDER "External/glslang" )
			set_property( TARGET OGLCompiler PROPERTY FOLDER "External/glslang" )
			set_property( TARGET OSDependent PROPERTY FOLDER "External/glslang" )
			set_property( TARGET SPIRV PROPERTY FOLDER "External/glslang" )
			set_property( TARGET SPVRemapper PROPERTY FOLDER "External/glslang" )
		endif ()

		# ShaderWriter external library
		if ( NOT CASTOR_BUILDGRP_RENDERER )
			set( SDW_BUILD_EXPORTERS FALSE )
		else ()
			set( SDW_BUILD_EXPORTERS TRUE )
			if ( CASTOR_BUILD_RENDERER_OGL3 OR CASTOR_BUILD_RENDERER_OGL4  OR CASTOR_BUILD_RENDERER_VKN )
				set( SDW_BUILD_EXPORTER_GLSL ON )
			else ()
				set( SDW_BUILD_EXPORTER_GLSL OFF )
			endif ()
			set( SDW_BUILD_EXPORTER_SPIRV ${CASTOR_BUILD_RENDERER_VKN} )
			if ( WIN32 )
				set( SDW_BUILD_EXPORTER_HLSL ${CASTOR_BUILD_RENDERER_D3D11} )
			else ()
				set( SDW_BUILD_EXPORTER_HLSL FALSE )
			endif ()
		endif ()
		set( SDW_BUILD_SDW FALSE )
		add_subdirectory( ../${SHADERWRITER_DIR}/source ${CMAKE_BINARY_DIR}/${SHADERWRITER_DIR} )
		set( ShaderWriterLibraries
			ShaderAST
			ShaderWriter
		)
		set_property( TARGET ShaderAST PROPERTY FOLDER "External/ShaderWriter/Core" )
		set_property( TARGET ShaderWriter PROPERTY FOLDER "External/ShaderWriter/Core" )
		if ( CASTOR_BUILDGRP_RENDERER )
			if ( TARGET CompilerGlsl )
				set_property( TARGET CompilerGlsl PROPERTY FOLDER "External/ShaderWriter/Exporters" )
			endif ()
			if ( TARGET CompilerHlsl )
				set_property( TARGET CompilerHlsl PROPERTY FOLDER "External/ShaderWriter/Exporters" )
			endif ()
			if ( TARGET CompilerSpirV )
				set_property( TARGET CompilerSpirV PROPERTY FOLDER "External/ShaderWriter/Exporters" )
			endif ()
		endif ()
	endif ()

	CoreInit()
	if ( ${CASTOR_BUILD_CASTOR3D} )
		PluginsInit()
		SamplesInit()
		InteropInit()
	endif ()
	ToolsInit()

	CoreBuild()
	if ( ${CASTOR_BUILD_CASTOR3D} )
		PluginsBuild()
		SamplesBuild()
		InteropBuild()
	endif ()
	ToolsBuild()

	set( msg "\nCastor following projects will be built" )
	CoreSummary( ${msg} )
	if ( ${CASTOR_BUILD_CASTOR3D} )
		PluginsSummary( ${msg} )
		SamplesSummary( ${msg} )
		InteropSummary( ${msg} )
	endif ()
	ToolsSummary( ${msg} )
	message( STATUS "${msg}\n" )
else ()
	message( FATAL_ERROR "Couldn't find Vulkan SDK" )
endif ()