get_filename_component( CASTOR_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY )
get_filename_component( CASTOR_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR} DIRECTORY )

set( Vulkan_INCLUDE_DIR ${CASTOR_SOURCE_DIR}/${ASHES_DIR}/external/Vulkan-Headers/include )
set( Vulkan_INCLUDE_DIRS ${Vulkan_INCLUDE_DIR} )

add_subdirectory( Core )
if ( ${CASTOR_BUILD_CASTOR3D} )
	add_subdirectory( Plugins )
	add_subdirectory( Interop )
endif ()

if ( ${CASTOR_BUILD_CASTOR3D} )
	# Ashes external library
	set( ASHES_BUILD_RENDERER_PLUGINS ON )
	set( ASHES_BUILD_RENDERER_OGL ON )
	set( ASHES_BUILD_RENDERER_VKN ON )

	if ( WIN32 )
		set( ASHES_BUILD_RENDERER_D3D11 ON )
	endif ()

	if ( CASTOR_BUILDGRP_TEST )
		set( ASHES_BUILD_RENDERER_TEST ON )
	endif ()

	set( ASHES_BUILD_TESTS ${CASTOR_BUILDGRP_TEST} )
	
	if ( WIN32 )
		set( ASHES_BUILD_SAMPLES ${CASTOR_BUILDGRP_SAMPLE} )
		set( ASHES_BUILD_SW_SAMPLES ${CASTOR_BUILDGRP_SAMPLE} )
	else ()
		set( ASHES_BUILD_SAMPLES OFF )
		set( ASHES_BUILD_SW_SAMPLES OFF )
	endif ()

	if ( WIN32 )
		set( Ashes_BINARY_DEFINITIONS VK_USE_PLATFORM_WIN32_KHR )
	elseif ( ANDROID )
		set( Ashes_BINARY_DEFINITIONS VK_USE_PLATFORM_ANDROID_KHR )
	elseif ( APPLE )
		set( Ashes_BINARY_DEFINITIONS VK_USE_PLATFORM_MACOS_MVK )
	else ()
		set( Ashes_BINARY_DEFINITIONS VK_USE_PLATFORM_XLIB_KHR )
	endif ()

	get_filename_component( SPIRV_CROSS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../${ASHES_DIR}/external/SPIRV-Cross REALPATH )
	add_subdirectory( ../${ASHES_DIR}/source ${CMAKE_BINARY_DIR}/${ASHES_DIR} )
	if ( ASHES_BUILD_SAMPLES )
		add_subdirectory( ../${ASHES_DIR}/samples ${CMAKE_BINARY_DIR}/${ASHES_DIR}/samples )
	endif ()
	add_subdirectory( ../${ASHES_DIR}/test ${CMAKE_BINARY_DIR}/${ASHES_DIR}/test )

	if ( CASTOR_USE_GLSLANG )
		if ( VCPKG_TOOLCHAIN OR NOT EXISTS ../${ASHES_DIR}/external/glslang/glslang/Public/ShaderLang.h )
			if ( VCPKG_TOOLCHAIN )
				find_package( glslang CONFIG )
				set( glslang_LIBRARIES
					glslang::OSDependent
					glslang::glslang
					glslang::OGLCompiler
					glslang::SPIRV
				)
				find_path( glslang_INCLUDE_DIR glslang/Public/ShaderLang.h )
			else ()
				find_package( glslang )
				if ( glslang_FOUND )
					set( CASTOR_USE_GLSLANG ON )
				else ()
					set( CASTOR_USE_GLSLANG OFF )
				endif ()
			endif ()
		else ()
			set( glslang_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/${ASHES_DIR}/external/glslang )
			set( ENABLE_GLSLANG_BINARIES OFF CACHE BOOL "Builds glslangValidator and spirv-remap" FORCE )
			set( ENABLE_AMD_EXTENSIONS OFF CACHE BOOL "Enables support of AMD-specific extensions" FORCE )
			set( ENABLE_NV_EXTENSIONS OFF CACHE BOOL "Enables support of Nvidia-specific extensions" FORCE )
			set( ENABLE_HLSL OFF CACHE BOOL "Enables HLSL input support" FORCE )
			set( BUILD_TESTING OFF )
			set( SKIP_GLSLANG_INSTALL ON )
			add_subdirectory( ../external/glslang ${CMAKE_BINARY_DIR}/external/glslang )
			set_property( TARGET glslang PROPERTY FOLDER "External/glslang" )
			set_property( TARGET OGLCompiler PROPERTY FOLDER "External/glslang" )
			set_property( TARGET OSDependent PROPERTY FOLDER "External/glslang" )
			set_property( TARGET SPIRV PROPERTY FOLDER "External/glslang" )
			set_property( TARGET SPVRemapper PROPERTY FOLDER "External/glslang" )
		endif ()
	endif ()

	# ShaderWriter external library
	if ( NOT CASTOR_BUILDGRP_RENDERER )
		if ( CASTOR_BUILDGRP_TEST )
			set( SDW_BUILD_EXPORTER_GLSL ON )
			set( SDW_BUILD_EXPORTER_HLSL OFF )
		endif ()
	else ()
		if ( CASTOR_BUILD_RENDERER_OGL OR CASTOR_BUILD_RENDERER_VKN OR CASTOR_BUILDGRP_TEST )
			set( SDW_BUILD_EXPORTER_GLSL ON )
		else ()
			set( SDW_BUILD_EXPORTER_GLSL OFF )
		endif ()
		if ( WIN32 )
			set( SDW_BUILD_EXPORTER_HLSL ${CASTOR_BUILD_RENDERER_D3D11} )
		else ()
			set( SDW_BUILD_EXPORTER_HLSL OFF )
		endif ()
	endif ()
	set( SDW_BUILD_EXPORTERS ON )
	set( SDW_BUILD_EXPORTER_SPIRV ON )
	set( SDW_BUILD_TESTS ${CASTOR_BUILDGRP_TEST} )
	set( SDW_BUILD_SDW OFF )
	set( SDW_GENERATE_SOURCE OFF )
	set( SDW_BASE_DIR "External/ShaderWriter" )
	add_subdirectory( ../${SHADERWRITER_DIR}/source ${CMAKE_BINARY_DIR}/${SHADERWRITER_DIR} )

	if ( SDW_BUILD_TESTS AND SDW_BUILD_EXPORTERS )
		add_subdirectory( ../${SHADERWRITER_DIR}/test ${CMAKE_BINARY_DIR}/${SHADERWRITER_DIR}/test )
	endif ()
endif ()

CoreInit()
if ( ${CASTOR_BUILD_CASTOR3D} )
	PluginsInit()
	InteropInit()
endif ()

CoreBuild()
if ( ${CASTOR_BUILD_CASTOR3D} )
	PluginsBuild()
	InteropBuild()
endif ()

set( msg "\nCastor following projects will be built" )
CoreSummary( ${msg} )
if ( ${CASTOR_BUILD_CASTOR3D} )
	PluginsSummary( ${msg} )
	InteropSummary( ${msg} )
endif ()
message( STATUS "${msg}\n" )

set( CastorBinsDependencies
	${CastorBinsDependencies}
	PARENT_SCOPE
)
set( Castor3DBinsDependencies
	${Castor3DBinsDependencies}
	PARENT_SCOPE
)
