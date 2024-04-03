get_filename_component( CASTOR_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY )
get_filename_component( CASTOR_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR} DIRECTORY )

if ( NOT EXISTS ${CASTOR_SOURCE_DIR}/${ASHES_DIR}/external/Vulkan-Headers/include/ )
	if ( VCPKG_PACKAGE_BUILD OR VCPKG_TOOLCHAIN )
		set( Vulkan_INCLUDE_DIR ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include )
		set( Vulkan_INCLUDE_DIRS ${Vulkan_INCLUDE_DIR} )
	else ()
		find_package( Vulkan REQUIRED )
	endif ()
else ()
	set( Vulkan_INCLUDE_DIR ${CASTOR_SOURCE_DIR}/${ASHES_DIR}/external/Vulkan-Headers/include )
	set( Vulkan_INCLUDE_DIRS ${Vulkan_INCLUDE_DIR} )
endif ()

add_subdirectory( Core )
if ( ${CASTOR_BUILD_CASTOR3D} )
	add_subdirectory( Plugins )
	add_subdirectory( Interop )
endif ()

if ( WIN32 )
	set( C3D_LIB_PREFIX castor3d )
else ()
	set( C3D_LIB_PREFIX libcastor3d )
endif ()

if ( VCPKG_BUILD OR CASTOR_FORCE_VCPKG_SUBMODULES )
	set( Ashes_DIR ${VCPKG_SHARE_DIR}/ashes )
	find_package( Ashes CONFIG REQUIRED ashespp )
else ()
	# Ashes external library
	set( ASHES_BUILD_RENDERER_PLUGINS ${CASTOR_BUILD_PLUGINS} )
	set( ASHES_BUILD_RENDERER_VKN ON )
	set( ASHES_BUILD_RENDERER_OGL OFF )
	set( ASHES_BUILD_RENDERER_D3D11 OFF )
	set( ASHES_BUILD_RENDERER_TEST ${CASTOR_BUILDGRP_TEST} )
	set( ASHES_BUILD_TESTS OFF )
	set( ASHES_BUILD_SAMPLES OFF )
	set( ASHES_BUILD_SW_SAMPLES OFF )

	if ( WIN32 )
		set( Ashes_BINARY_DEFINITIONS
			VK_USE_PLATFORM_WIN32_KHR
		)
	elseif ( ANDROID )
		set( Ashes_BINARY_DEFINITIONS
			VK_USE_PLATFORM_ANDROID_KHR
		)
	elseif ( APPLE )
		set( Ashes_BINARY_DEFINITIONS
			VK_USE_PLATFORM_MACOS_MVK
		)
	else ()
		set( Ashes_BINARY_DEFINITIONS
			VK_USE_PLATFORM_XLIB_KHR
			VK_USE_PLATFORM_XCB_KHR
			VK_USE_PLATFORM_WAYLAND_KHR
		)
	endif ()

	get_filename_component( ASHES_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../${ASHES_DIR} REALPATH )

	if ( NOT VCPKG_TOOLCHAIN )
		set( SPIRV_CROSS_DIR ${ASHES_SOURCE_DIR}/external/spirv_cross REALPATH )
	endif ()
	include( PCHSupport )

	add_subdirectory( ${ASHES_SOURCE_DIR}/source ${CMAKE_BINARY_DIR}/${ASHES_DIR} )
endif ()

find_package( Threads REQUIRED )

set( PLATFORM_LIBRARIES
	Threads::Threads
)

macro( c3d_release_pdbs _TARGET_NAME )
	if( CMAKE_CXX_COMPILER_ID MATCHES "MSVC" )
		target_compile_options( ${_TARGET_NAME} PRIVATE "$<$<CONFIG:Release>:/Zi>" )
		target_link_options( ${_TARGET_NAME} PRIVATE "$<$<CONFIG:Release>:/DEBUG>" )
		target_link_options( ${_TARGET_NAME} PRIVATE "$<$<CONFIG:Release>:/OPT:REF>" )
		target_link_options( ${_TARGET_NAME} PRIVATE "$<$<CONFIG:Release>:/OPT:ICF>" )
	endif()
endmacro()

if ( ${CASTOR_BUILD_CASTOR3D} )
	if ( CASTOR_USE_GLSLANG )
		option( CASTOR_FORCE_GLSLANG_SUBMODULE "Force use of glslang submodule" OFF )
		if ( NOT CASTOR_FORCE_GLSLANG_SUBMODULE )
			if ( VCPKG_TOOLCHAIN )
				set( glslang_DIR ${VCPKG_SHARE_DIR}/glslang )
				find_package( glslang CONFIG REQUIRED glslang GenericCodeGen OSDependent SPIRV SPVRemapper )
				if ( TARGET glslang::SPIRV )
					get_target_property( glslang_INCLUDE_DIR glslang::SPIRV INTERFACE_INCLUDE_DIRECTORIES )
					find_path( glslang_INCLUDE_DIR glslang/Public/ShaderLang.h )
					set( glslang_LIBRARIES
						glslang::OSDependent
						glslang::glslang
						glslang::GenericCodeGen
						glslang::SPIRV
						glslang::SPVRemapper
					)
				else ()
					get_target_property( glslang_INCLUDE_DIR SPIRV INTERFACE_INCLUDE_DIRECTORIES )
					find_path( glslang_INCLUDE_DIR glslang/Public/ShaderLang.h )
					set( glslang_LIBRARIES
						OSDependent
						glslang
						GenericCodeGen
						SPIRV
						SPVRemapper
					)
				endif ()
			else ()
				find_package( glslang REQUIRED glslang OSDependent GenericCodeGen SPIRV )
				if ( glslang_FOUND )
					set( CASTOR_USE_GLSLANG ON )
				else ()
					set( CASTOR_USE_GLSLANG OFF )
				endif ()
				set( glslang_LIBRARIES
					glslang::glslang
					glslang::GenericCodeGen
					glslang::SPIRV
				)
			endif ()
		elseif ( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../external/glslang/ )
			set( glslang_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../external )
			if ( NOT TARGET glslang )
				set( ENABLE_GLSLANG_BINARIES OFF CACHE BOOL "Builds glslangValidator and spirv-remap" FORCE )
				set( ENABLE_AMD_EXTENSIONS OFF CACHE BOOL "Enables support of AMD-specific extensions" FORCE )
				set( ENABLE_NV_EXTENSIONS OFF CACHE BOOL "Enables support of Nvidia-specific extensions" FORCE )
				set( ENABLE_HLSL OFF CACHE BOOL "Enables HLSL input support" FORCE )
				set( ENABLE_CTEST OFF CACHE BOOL "Enables testing" FORCE )
				set( BUILD_TESTING OFF CACHE BOOL "Build the testing tree" FORCE )
				set( SKIP_GLSLANG_INSTALL ON )
				add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/../external/glslang ${CMAKE_BINARY_DIR}/external/glslang )
				set_property( TARGET glslang PROPERTY FOLDER "External/glslang" )
				set_property( TARGET GenericCodeGen PROPERTY FOLDER "External/glslang" )
				set_property( TARGET OSDependent PROPERTY FOLDER "External/glslang" )
				set_property( TARGET SPIRV PROPERTY FOLDER "External/glslang" )
				set_property( TARGET SPVRemapper PROPERTY FOLDER "External/glslang" )
			endif ()
			set( glslang_LIBRARIES
				OSDependent
				glslang
				GenericCodeGen
				SPIRV
				SPVRemapper
			)
		endif ()
	endif ()

	if ( VCPKG_BUILD OR CASTOR_FORCE_VCPKG_SUBMODULES )
		set( RenderGraph_DIR ${VCPKG_SHARE_DIR}/rendergraph )
		find_package( RenderGraph CONFIG REQUIRED )
	else ()
		# RenderGraph external library
		set( CRG_BUILD_STATIC OFF )
		set( CRG_UNITY_BUILD ON )
		set( CRG_BUILD_TESTS OFF )
		set( CRG_BASE_DIR "Core/RenderGraph" )
		set( VULKAN_HEADERS_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../${ASHES_DIR}/external/Vulkan-Headers/include )
		add_subdirectory( ../${RENDERGRAPH_DIR} ${CMAKE_BINARY_DIR}/${RENDERGRAPH_DIR} )
	endif ()

	if ( VCPKG_BUILD OR CASTOR_FORCE_VCPKG_SUBMODULES )
		set( ShaderWriter_DIR ${VCPKG_SHARE_DIR}/shaderwriter )
		set( SdwModules CompilerSpirV )
		if ( CASTOR_GUICOMMON_NEEDS_HLSL )
			set( SdwModules ${SdwModules} CompilerHlsl )
		endif ()
		if ( CASTOR_C3D_NEEDS_GLSL )
			set( SdwModules ${SdwModules} CompilerGlsl )
		endif ()
		find_package( ShaderWriter CONFIG REQUIRED ShaderAST ShaderWriter ${SdwModules} )
	else ()
		# ShaderWriter external library
		set( SDW_BUILD_EXPORTERS ON )
		set( SDW_UNITY_BUILD ${PROJECTS_UNITY_BUILD} )
		set( SDW_BUILD_STATIC OFF )
		set( SDW_BUILD_STATIC_SDAST OFF )
		set( SDW_BUILD_STATIC_SDW OFF )
		set( SDW_BUILD_STATIC_EXPORTERS OFF )
		set( SDW_BUILD_EXPORTER_HLSL ${CASTOR_GUICOMMON_NEEDS_HLSL} )
		set( SDW_BUILD_EXPORTER_GLSL ${CASTOR_C3D_NEEDS_GLSL} )
		set( SDW_BUILD_EXPORTER_SPIRV ON )
		set( SDW_BUILD_VULKAN_LAYER OFF )
		set( SDW_BUILD_TESTS OFF )
		set( SDW_BUILD_SDW OFF )
		set( SDW_GENERATE_SOURCE OFF )
		set( SDW_BASE_DIR "Core/ShaderWriter" )
		add_subdirectory( ../${SHADERWRITER_DIR} ${CMAKE_BINARY_DIR}/${SHADERWRITER_DIR} )
	endif ()

	set( meshoptimizer_DIR ${VCPKG_SHARE_DIR}/meshoptimizer )
	find_package( meshoptimizer CONFIG REQUIRED )
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

set( msg "\nCastor following projects will be built:" )
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
set( CASTOR_INSTEXP_LIST
	${CASTOR_INSTEXP_LIST}
	PARENT_SCOPE
)