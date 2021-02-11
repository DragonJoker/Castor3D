get_filename_component( CASTOR_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY )
get_filename_component( CASTOR_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR} DIRECTORY )

set( Vulkan_INCLUDE_DIR ${CASTOR_SOURCE_DIR}/${ASHES_DIR}/external/Vulkan-Headers/include )
set( Vulkan_INCLUDE_DIRS ${Vulkan_INCLUDE_DIR} )

add_subdirectory( Core )
if ( ${CASTOR_BUILD_CASTOR3D} )
	add_subdirectory( Plugins )
	add_subdirectory( Interop )
endif ()

# Ashes external library
set( ASHES_BUILD_RENDERER_PLUGINS ${CASTOR_BUILD_PLUGINS} )
set( ASHES_BUILD_RENDERER_VKN ON )
set( ASHES_BUILD_TESTS OFF )
set( ASHES_BUILD_SAMPLES OFF )
set( ASHES_BUILD_SW_SAMPLES OFF )

set( ASHES_BUILD_RENDERER_TEST ${CASTOR_BUILDGRP_TEST} )
if ( WIN32 )
	set( Ashes_BINARY_DEFINITIONS VK_USE_PLATFORM_WIN32_KHR )
elseif ( ANDROID )
	set( Ashes_BINARY_DEFINITIONS VK_USE_PLATFORM_ANDROID_KHR )
elseif ( APPLE )
	set( Ashes_BINARY_DEFINITIONS VK_USE_PLATFORM_MACOS_MVK )
else ()
	set( Ashes_BINARY_DEFINITIONS VK_USE_PLATFORM_XLIB_KHR )
endif ()

if ( NOT VCPKG_TOOLCHAIN )
	get_filename_component( SPIRV_CROSS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../${ASHES_DIR}/external/spirv_cross REALPATH )
endif ()
include( PCHSupport )
add_subdirectory( ../${ASHES_DIR}/source ${CMAKE_BINARY_DIR}/${ASHES_DIR} )

if ( ${CASTOR_BUILD_CASTOR3D} )
	if ( CASTOR_USE_GLSLANG )
		if ( CASTOR_USE_SYSTEM_LIBS )
			if ( VCPKG_TOOLCHAIN )
				find_package( glslang CONFIG REQUIRED glslang OGLCompiler OSDependent SPIRV SPVRemapper )
				find_path( glslang_INCLUDE_DIR glslang/Public/ShaderLang.h )
			else ()
				find_package( glslang REQUIRED glslang OSDependent OGLCompiler SPIRV )
				if ( glslang_FOUND )
					set( CASTOR_USE_GLSLANG ON )
				else ()
					set( CASTOR_USE_GLSLANG OFF )
				endif ()
			endif ()
			set( glslang_LIBRARIES
				glslang::OSDependent
				glslang::glslang
				glslang::OGLCompiler
				glslang::SPIRV
			)
		elseif ( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../external/glslang/ )
			set( glslang_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../external/glslang )
			set( ENABLE_GLSLANG_BINARIES OFF CACHE BOOL "Builds glslangValidator and spirv-remap" FORCE )
			set( ENABLE_AMD_EXTENSIONS OFF CACHE BOOL "Enables support of AMD-specific extensions" FORCE )
			set( ENABLE_NV_EXTENSIONS OFF CACHE BOOL "Enables support of Nvidia-specific extensions" FORCE )
			set( ENABLE_HLSL OFF CACHE BOOL "Enables HLSL input support" FORCE )
			set( BUILD_TESTING OFF )
			set( SKIP_GLSLANG_INSTALL ON )
			add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/../external/glslang ${CMAKE_BINARY_DIR}/external/glslang )
			set_property( TARGET glslang PROPERTY FOLDER "External/glslang" )
			set_property( TARGET OGLCompiler PROPERTY FOLDER "External/glslang" )
			set_property( TARGET OSDependent PROPERTY FOLDER "External/glslang" )
			set_property( TARGET SPIRV PROPERTY FOLDER "External/glslang" )
			set_property( TARGET SPVRemapper PROPERTY FOLDER "External/glslang" )
		endif ()
	endif ()

	# ShaderWriter external library
	set( SDW_BUILD_EXPORTERS ON )
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
	set( SDW_BASE_DIR "External/ShaderWriter" )
	add_subdirectory( ../${SHADERWRITER_DIR} ${CMAKE_BINARY_DIR}/${SHADERWRITER_DIR} )
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

if ( TARGET Castor3D )
	set( CASTOR_INSTEXP_LIST
		Castor3D
		${CASTOR_INSTEXP_LIST}
	)
endif ()

install(
	TARGETS CastorUtils ${CASTOR_INSTEXP_LIST}
	EXPORT Castor3D
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)
install( EXPORT Castor3D
	FILE Castor3DConfig.cmake
	NAMESPACE castor::
	DESTINATION lib/cmake/castor3d
)
include(CMakePackageConfigHelpers)
write_basic_package_version_file( Castor3DConfigVersion.cmake
	VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_BUILD}
	COMPATIBILITY AnyNewerVersion
)

set( CastorBinsDependencies
	${CastorBinsDependencies}
	PARENT_SCOPE
)
set( Castor3DBinsDependencies
	${Castor3DBinsDependencies}
	PARENT_SCOPE
)
