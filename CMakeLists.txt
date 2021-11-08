cmake_minimum_required( VERSION 3.15 )
cmake_policy( VERSION 3.15 )

if ( VCPKG_TARGET_TRIPLET )
	set( VCPKG_TOOLCHAIN ON )
endif ()

set( MAIN_PROJECT_NAME "Castor3D" )

set( VERSION_MAJOR 0 )
set( VERSION_MINOR 11 )
set( VERSION_BUILD 0 )
set( VERSION_YEAR 2021 )

# Setup lookup for external modules
if ( VCPKG_TOOLCHAIN )
	set( CMAKE_MODULE_PATH
		${CMAKE_CURRENT_SOURCE_DIR}/CMake
		${CMAKE_CURRENT_SOURCE_DIR}/CMake/Modules
		${CMAKE_CURRENT_SOURCE_DIR}/CMake/Toolchains
	)
	set( CMAKE_MAP_IMPORTED_CONFIG_MINSIZEREL Release )
	set( CMAKE_MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release )
else ()
	set( CMAKE_MODULE_PATH
		${CMAKE_CURRENT_SOURCE_DIR}/CMake
		${CMAKE_CURRENT_SOURCE_DIR}/CMake/Modules
		${CMAKE_CURRENT_SOURCE_DIR}/CMake/Modules/Traditional
		${CMAKE_CURRENT_SOURCE_DIR}/CMake/Toolchains
	)
endif ()

set( CMAKE_TEMPLATES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/CMake/Templates )
set( CMAKE_POSITION_INDEPENDENT_CODE ON )

include( PrecompiledHeaders )

set( CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo" CACHE STRING "The configuration types" FORCE )

project( ${MAIN_PROJECT_NAME} )

include( Setup )
include( Project )
include( CompilerVersion )
include( UnitTest )
include( CompilationFlags )
include( AStyleUtils )
include( Doxygen )
include( ExternalDependencies )

set_property( GLOBAL PROPERTY USE_FOLDERS ON )

set( COMPILER_FOLDER "vc14" )

if ( NOT ANDROID )
	if ( VCPKG_TOOLCHAIN )
		set( CASTOR_USE_SYSTEM_LIBS ON )
	elseif ( WIN32 )
		option( CASTOR_USE_SYSTEM_LIBS "Use system (or dependencies submodule) provided FreeType and zlib" OFF )
	else ()
		set( CASTOR_USE_SYSTEM_LIBS ON )
	endif ()
else ()
	set( CASTOR_USE_SYSTEM_LIBS OFF )
endif ()

option( CASTOR_USE_GLSLANG "Use glslang to convert GLSL shaders to SPIR-V" OFF )
option( CASTOR_BUILGRP_RENDERER "Build Ashes renderer plugins" ON )

if ( WIN32 )
	option( CASTOR_USE_DEPS_WXWIDGETS "Use wxWidgets from dependencies submodule instead of system/vcpkg one" OFF )
endif ()

set( DEPENDENCIES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/${COMPILER_FOLDER}/${PROJECTS_PLATFORM} )

if ( CASTOR_USE_SYSTEM_LIBS AND WIN32 AND NOT VCPKG_TOOLCHAIN )
	msg_debug( "Fetching system libraries from dependencies submodule." )
	if ( EXISTS ${DEPENDENCIES_DIR}/glslang/ )
		set( glslang_DIR ${DEPENDENCIES_DIR}/glslang )
		set( SPIRV_ROOT_DIR ${DEPENDENCIES_DIR}/glslang )
	endif ()
	if ( EXISTS ${DEPENDENCIES_DIR}/glm/ )
		set( GLM_ROOT_DIR ${DEPENDENCIES_DIR}/glm )
	endif ()
	if ( EXISTS ${DEPENDENCIES_DIR}/ffmpeg/ )
		set( FFMPEG_DIR ${DEPENDENCIES_DIR}/ffmpeg )
	endif ()
	if ( EXISTS ${DEPENDENCIES_DIR}/assimp/ )
		set( assimp_DIR ${DEPENDENCIES_DIR}/assimp )
	endif ()
	if ( EXISTS ${DEPENDENCIES_DIR}/freetype/ )
		set( ENV{FREETYPE_DIR} ${DEPENDENCIES_DIR}/freetype )
		set( FREETYPE_USE_DEPENDENCIES ON )
	else ()
		set( FREETYPE_USE_DEPENDENCIES OFF )
	endif ()
	if ( EXISTS ${DEPENDENCIES_DIR}/zlib/ )
		set( ZLIB_ROOT ${DEPENDENCIES_DIR}/zlib )
	endif ()
	if ( EXISTS ${DEPENDENCIES_DIR}/wxWidgets/ )
		set( wxWidgets_ROOT_DIR ${DEPENDENCIES_DIR}/wxWidgets CACHE PATH "wxWidgets root directory" )
		set( wxWidgets_LIB_DIR ${DEPENDENCIES_DIR}/wxWidgets/lib CACHE PATH "wxWidgets libraries directory" )
	endif ()
endif ()
if ( CASTOR_USE_DEPS_WXWIDGETS AND WIN32 )
	if ( NOT EXISTS ${DEPENDENCIES_DIR}/wxWidgets/ )
		message( SEND_ERROR "${DEPENDENCIES_DIR}/wxWidgets not found" )
	endif ()
	set( wxWidgets_ROOT_DIR ${DEPENDENCIES_DIR}/wxWidgets CACHE PATH "wxWidgets root directory" FORCE )
	set( wxWidgets_LIB_DIR ${DEPENDENCIES_DIR}/wxWidgets/vc_x64_lib CACHE PATH "wxWidgets libraries directory" FORCE )
endif ()

if ( CMAKE_COMPILER_IS_GNUCXX )
	DumpCompilerVersion( Castor_COMPILER_VERSION )
	msg_debug( "GCC version : ${Castor_COMPILER_VERSION}" )
	if ( ${Castor_COMPILER_VERSION} LESS "100" )
		message( FATAL_ERROR "Your compiler is too old (${Castor_COMPILER_VERSION}), consider an update." )
	endif ()
elseif ( ${CMAKE_CXX_COMPILER_ID} MATCHES "Clang" )
	DumpCompilerVersion( Castor_COMPILER_VERSION )
	msg_debug( "clang version : ${Castor_COMPILER_VERSION}" )
elseif ( MSVC )
	if ( MSVC60 OR MSVC70 OR MSVC71 OR MSVC80 OR MSVC10 OR MSVC11 OR MSVC12 )
		message( FATAL_ERROR "Your compiler is too old, consider an update." )
	endif ()
else ()
	message( FATAL_ERROR "Unsupported compiler." )
endif ()

add_compile_options( $<$<CXX_COMPILER_ID:Clang>:-Wno-inconsistent-missing-override> ) # for wxWidgets

add_compile_definitions( C3D_VERSION_MAJOR=${VERSION_MAJOR} )
add_compile_definitions( C3D_VERSION_MINOR=${VERSION_MINOR} )
add_compile_definitions( C3D_VERSION_BUILD=${VERSION_BUILD} )
add_compile_definitions( $<$<CXX_COMPILER_ID:MSVC>:_ENFORCE_MATCHING_ALLOCATORS=0> )

set( ASHES_DIR external/Ashes )
set( SHADERWRITER_DIR external/ShaderWriter )
set( RENDERGRAPH_DIR external/RenderGraph )

msg_debug( "CMAKE_BUILD_TYPE : ${CMAKE_BUILD_TYPE}")
msg_debug( "CMAKE_C_FLAGS    : ${CMAKE_C_FLAGS}" )
msg_debug( "CMAKE_CXX_FLAGS  : ${CMAKE_CXX_FLAGS}" )

set( CastorBinsDependencies
	castor::CastorUtils
)

set( Castor3DBinsDependencies
	castor::CastorUtils
)

#--------------------------------------------------------------------------------------------------
#	MSVC specifics
#--------------------------------------------------------------------------------------------------
if( MSVC )
	# Copy Rebracer.xml file so we can use Rebracer extension to uniformise code formatting
	file( COPY ${CMAKE_CURRENT_SOURCE_DIR}/data/VS/Rebracer.xml DESTINATION ${CMAKE_CURRENT_BINARY_DIR} )
endif()

option( CASTOR_BUILDGRP_TEST "Build test projects" OFF )
option( CASTOR_BUILDGRP_SETUP "Build setup projects" OFF )
option( CASTOR_BUILDGRP_SAMPLE "Build Castor3D samples." OFF )
option( CASTOR_BUILD_CASTOR3D "Build Castor3D" ON )
option( CASTOR_C3D_NEEDS_GLSL "Tells if Castor3D needs CompilerGlsl from ShaderWriter" OFF )
option( CASTOR_GUICOMMON_NEEDS_HLSL "Tells if GuiCommon needs CompilerHlsl from ShaderWriter" OFF )

# Small macro to add subdirectory files to current target source and header files
macro( parse_subdir_files_ex _BASE_DIR _PROJGROUP _FOLDER _GROUP )
	set( __PROJGROUP "${_PROJGROUP}" )
	if ( NOT "${__PROJGROUP}" STREQUAL "" )
		set( __PROJGROUP "${__PROJGROUP}/" )
	endif ()
	file( GLOB _FOLDER_HEADER_FILES
		${_BASE_DIR}/include/${__PROJGROUP}${PROJECT_NAME}/${_FOLDER}/*.h
		${_BASE_DIR}/include/${__PROJGROUP}${PROJECT_NAME}/${_FOLDER}/*.hh
		${_BASE_DIR}/include/${__PROJGROUP}${PROJECT_NAME}/${_FOLDER}/*.hpp
		${_BASE_DIR}/include/${__PROJGROUP}${PROJECT_NAME}/${_FOLDER}/*.inl
		${_BASE_DIR}/source/${__PROJGROUP}${PROJECT_NAME}/${_FOLDER}/*.h
		${_BASE_DIR}/source/${__PROJGROUP}${PROJECT_NAME}/${_FOLDER}/*.hh
		${_BASE_DIR}/source/${__PROJGROUP}${PROJECT_NAME}/${_FOLDER}/*.hpp
		${_BASE_DIR}/source/${__PROJGROUP}${PROJECT_NAME}/${_FOLDER}/*.inl
	)
	file( GLOB _FOLDER_ENUM_FILES
		${_BASE_DIR}/include/${__PROJGROUP}${PROJECT_NAME}/${_FOLDER}/*.enum
	)
	file( GLOB _FOLDER_SCRIPT_FILES
		${_BASE_DIR}/script/${__PROJGROUP}${PROJECT_NAME}/${_FOLDER}/*.py
	)
	file( GLOB _FOLDER_NATVIS_FILES
		${_BASE_DIR}/include/${__PROJGROUP}${PROJECT_NAME}/${_FOLDER}/*.natvis
		${_BASE_DIR}/source/${__PROJGROUP}${PROJECT_NAME}/${_FOLDER}/*.natvis
	)
	file( GLOB _FOLDER_SOURCE_FILES
		${_BASE_DIR}/source/${__PROJGROUP}${PROJECT_NAME}/${_FOLDER}/*.c
		${_BASE_DIR}/source/${__PROJGROUP}${PROJECT_NAME}/${_FOLDER}/*.cc
		${_BASE_DIR}/source/${__PROJGROUP}${PROJECT_NAME}/${_FOLDER}/*.cpp
		${_BASE_DIR}/include/${__PROJGROUP}${PROJECT_NAME}/${_FOLDER}/*.c
		${_BASE_DIR}/include/${__PROJGROUP}${PROJECT_NAME}/${_FOLDER}/*.cc
		${_BASE_DIR}/include/${__PROJGROUP}${PROJECT_NAME}/${_FOLDER}/*.cpp
	)
	if ( NOT "${_FOLDER_SCRIPT_FILES}" STREQUAL "" )
		source_group( "Script Files\\${_GROUP}" FILES ${_FOLDER_SCRIPT_FILES} )
	endif ()
	if ( NOT "${_FOLDER_ENUM_FILES}" STREQUAL "" )
		source_group( "Enum Files\\${_GROUP}" FILES ${_FOLDER_ENUM_FILES} )
	endif ()
	if ( NOT "${_FOLDER_HEADER_FILES}" STREQUAL "" )
		source_group( "Header Files\\${_GROUP}" FILES ${_FOLDER_HEADER_FILES} )
	endif ()
	if ( NOT "${_FOLDER_SOURCE_FILES}" STREQUAL "" )
		source_group( "Source Files\\${_GROUP}" FILES ${_FOLDER_SOURCE_FILES} )
	endif ()
	if ( NOT "${_FOLDER_NATVIS_FILES}" STREQUAL "" )
		source_group( "Natvis Files\\${_GROUP}" FILES ${_FOLDER_NATVIS_FILES} )
	endif ()
endmacro()

macro( c3d_parse_subdir_files _BASE_DIR _FOLDER _SUBFOLDER _GROUP )
	parse_subdir_files_ex( ${_BASE_DIR} ${_FOLDER} ${_SUBFOLDER} ${_GROUP} )
endmacro()

macro( parse_subdir_files _BASE_DIR _FOLDER _GROUP )
	parse_subdir_files_ex( ${_BASE_DIR} "" ${_FOLDER} ${_GROUP} )
endmacro()

if ( MSVC OR NOT "${CMAKE_BUILD_TYPE}" STREQUAL "" )
	if ( CASTOR_BUILDGRP_TEST )
		enable_testing()
	endif ()

	if ( NOT CASTOR_USE_SYSTEM_LIBS )
		set( ZLIB_SKIP_INSTALL 1 )
		add_subdirectory( external/zlib )
		set_property( TARGET ZLIB PROPERTY FOLDER "External" )

		if( NOT APPLE )
			add_subdirectory( external/FreeType )
			set_property( TARGET FreeType PROPERTY FOLDER "External" )
		endif ()
	endif ()

	set( EXTERNAL_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external )
	set( EXTERNAL_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/external )

	set( Ashes_BASE_DIR "Core/Ashes" )
	set( Ashes_SOURCE_DIR ${EXTERNAL_SOURCE_DIR}/Ashes )
	set( Ashes_BINARY_DIR ${EXTERNAL_BINARY_DIR}/Ashes )

	set( RenderGraph_SOURCE_DIR ${EXTERNAL_SOURCE_DIR}/RenderGraph )
	set( RenderGraph_BINARY_DIR ${EXTERNAL_BINARY_DIR}/RenderGraph )

	set( Castor3D_MATERIAL_DIRS )
	set( Castor3D_MATERIAL_DEPS )

	add_subdirectory( source )
	add_subdirectory( tools )

	if ( CASTOR_BUILD_CASTOR3D )
		add_subdirectory( samples )
	endif ()

	if( CASTOR_BUILDGRP_TEST )
		add_subdirectory( test )
	endif()

	if ( CASTOR_BUILDGRP_SETUP )
		if ( ( "${PROJECTS_PACKAGE_WIX}" STREQUAL "ON" ) OR ( "${PROJECTS_PACKAGE_DEB}" STREQUAL "ON" ) OR ( "${PROJECTS_PACKAGE_RPM}" STREQUAL "ON" ) )
			file( MAKE_DIRECTORY "${PROJECTS_SETUP_OUTPUT_DIR}/${CMAKE_BUILD_TYPE}" )
			set( CPACK_PACKAGE_NAME "Castor3D" )
			set( CPACK_PACKAGE_VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_BUILD}" )
			set( CPACK_PACKAGE_DESCRIPTION_SUMMARY "Castor3D Engine" )
			set( CPACK_PACKAGE_DESCRIPTION "Allows Castor3D developments" )
			set( CPACK_PACKAGE_RELEASE 1 )
			set( CPACK_PACKAGE_CONTACT "dragonjoker59@hotmail.com" )
			set( CPACK_PACKAGE_VENDOR "DragonJoker" )
			set( CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_PACKAGE_RELEASE}.${PROJECTS_PLATFORM}" )
			set( CPACK_OUTPUT_FILE_PREFIX "${PROJECTS_SETUP_OUTPUT_DIR}/${CMAKE_BUILD_TYPE}" )
			configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE ${CMAKE_CURRENT_BINARY_DIR}/LICENSE.txt COPYONLY )
			set( CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_BINARY_DIR}/LICENSE.txt" )
			set( CMAKE_SYSTEM_ARCHITECTURE ${PROJECTS_PLATFORM} )

			if ( "${PROJECTS_PACKAGE_WIX}" STREQUAL "ON" )
				set( CPACK_GENERATOR "WIX" )
				set( CPACK_PACKAGE_INSTALL_DIRECTORY "Castor3D" )
				set( CPACK_WIX_UPGRADE_GUID "6138748D-A6CE-4BB9-BE4B-A2E67C405336" )
				set( CPACK_WIX_PRODUCT_GUID "02ACFE05-CFCE-4E1D-85DA-59A068DDCF70" )
				set( CPACK_WIX_PRODUCT_ICON "${CMAKE_CURRENT_SOURCE_DIR}/install/castor.ico" )
				set( CPACK_WIX_UI_DIALOG "${CMAKE_CURRENT_SOURCE_DIR}/install/dialog.bmp" )
				set( CPACK_WIX_UI_BANNER "${CMAKE_CURRENT_SOURCE_DIR}/install/banner.bmp" )
				set( CPACK_WIX_PROGRAM_MENU_FOLDER "Castor3D" )
				set( CPACK_WIX_CULTURES "fr-FR;en-EN" )
				include(CPack)
			elseif ( "${PROJECTS_PACKAGE_DEB}" STREQUAL "ON" )
				set( CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX} )
				set( CPACK_DEB_COMPONENT_INSTALL ON )
				set( CPACK_GENERATOR "DEB" )
				set( CPACK_DEBIAN_PACKAGE_MAINTAINER "Sylvain Doremus" )
				set( CPACK_DEBIAN_PACKAGE_DEPENDS "${CPACK_DEBIAN_PACKAGE_DEPENDS}libfreetype6 (>= 2.5.2), zlib1g (>= 1.2.8)" )
				set( CPACK_DEBIAN_PACKAGE_ARCHITECTURE "${CMAKE_SYSTEM_ARCHITECTURE}" )
				set( CPACK_DEBIAN_PACKAGE_SECTION "devel" )
				set( CPACK_DEBIAN_PACKAGE_PRIORITY "optional" )
				include(CPack)
			elseif ( "${PROJECTS_PACKAGE_RPM}" STREQUAL "ON" )
				set( ${Castor_PACKAGE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/_CPack_Packages/Linux/RPM )
				set( CPACK_RPM_COMPONENT_INSTALL ON )
				set( CPACK_RPM_PACKAGE_SUMMARY "Castor3D" )
				set( CPACK_RPM_PACKAGE_ARCHITECTURE "${CMAKE_SYSTEM_ARCHITECTURE}" )
				set( CPACK_RPM_PACKAGE_GROUP "Development/Tools/Other" )
				set( CPACK_RPM_PACKAGE_LICENSE "MIT" )
				set( CPACK_RPM_SPEC_MORE_DEFINE "%define _rpmdir ${Castor_PACKAGE_DIR}" )
				set( CPACK_RPM_SPEC_MORE_DEFINE "%define _rpmfilename ${CPACK_PACKAGE_FILE_NAME}.rpm" )
				set( CPACK_RPM_SPEC_MORE_DEFINE "%define _unpackaged_files_terminate_build 0" )
				set( CPACK_RPM_SPEC_MORE_DEFINE "%define _topdir ${Castor_PACKAGE_DIR}" )
				include(CPack)
			endif()
		endif ()

		cpack_add_install_type( Minimal )
		cpack_add_install_type( Developer )
		cpack_add_install_type( Full )

		cpack_add_component_group( Core
			DISPLAY_NAME "Core libraries"
			DESCRIPTION "Base libraries needed to use Castor3D."
		)

		cpack_add_component_group( Plugins
			DISPLAY_NAME "Plugins"
			DESCRIPTION "Plugins usable in Castor3D."
		)

		cpack_add_component_group( Tools
			DISPLAY_NAME "Tools"
			DESCRIPTION "Some tools"
		)

		cpack_add_component_group( Samples
			DISPLAY_NAME "Samples"
			DESCRIPTION "Sample applications and scenes using Castor3D."
		)

		cpack_add_component_group( Development
			DISPLAY_NAME "Development"
			DESCRIPTION "All the tools needed to develop a project using Castor3D."
		)

		cpack_add_component_group( Test
			DISPLAY_NAME "Test"
			DESCRIPTION "Applications used to perform regression and performance tests."
			PARENT_GROUP Development
		)

		cpack_add_component_group( Documentation
			DISPLAY_NAME "Documentation"
			DESCRIPTION "Projects documentations"
			PARENT_GROUP Development
		)

		CoreSetup()
		if ( ${CASTOR_BUILD_CASTOR3D} )
			PluginsSetup()
			SamplesSetup()
			InteropSetup()
		endif ()
		ToolsSetup()
	endif ()
else()
	message( SEND_ERROR "Please select a build type (Debug or Release)" )
endif()
