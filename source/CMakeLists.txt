cmake_minimum_required( VERSION 3.1 )
cmake_policy( VERSION 3.1 )

#--------------------------------------------------------------------------------------------------
#	Initial configurations
#--------------------------------------------------------------------------------------------------
# Set project name, used in folders and in workspace creation
set( MAIN_PROJECT_NAME "Castor3D" )

# Set project version numbers
set( VERSION_MAJOR 0 )
set( VERSION_MINOR 11 )
set( VERSION_BUILD 0 )
set( VERSION_YEAR 2018 )

# Used to look for external modules
set( CMAKE_MODULE_PATH
	${CMAKE_SOURCE_DIR}/CMake
	${CMAKE_SOURCE_DIR}/CMake/Modules
	${CMAKE_SOURCE_DIR}/CMake/Toolchains
)
set( CMAKE_TEMPLATES_DIR ${CMAKE_SOURCE_DIR}/CMake/Templates )
# Experimental Precompiled headers support for GCC
include( PCHSupport )

set( CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo" CACHE STRING "The configuration types" FORCE )

# Declare the project
project( ${MAIN_PROJECT_NAME} )

include( Setup )
include( Project )
include( CompilerVersion )
include( UnitTest )
include( CompilationFlags )
include( AStyleUtils )
include( ExternalDependencies )

# Organize projects into folders
set_property( GLOBAL PROPERTY USE_FOLDERS ON )

set( COMPILER_FOLDER ${COMPILER} )

if ( ${COMPILER_FOLDER} STREQUAL "vc15" )
	set( COMPILER_FOLDER "vc14" )
endif ()

if ( EXISTS ${CMAKE_SOURCE_DIR}/../dependencies/${COMPILER_FOLDER}/${PROJECTS_PLATFORM}/ )
	message( STATUS "Found dependencies folder, fetching libraries from it" )
	get_filename_component( DEPENDENCIES_DIR ${CMAKE_SOURCE_DIR}/../dependencies/${COMPILER_FOLDER}/${PROJECTS_PLATFORM} REALPATH )
	set( ENV{FREETYPE_DIR} ${DEPENDENCIES_DIR}/freetype )
	set( ZLIB_ROOT ${DEPENDENCIES_DIR}/zlib )
	set( FreeImage_ROOT_DIR ${DEPENDENCIES_DIR}/FreeImage )
	set( FbxSDK_ROOT_DIR ${DEPENDENCIES_DIR}/FbxSDK )
	set( ASSIMP_ROOT_DIR ${DEPENDENCIES_DIR}/assimp )
	set( glslang_ROOT_DIR ${DEPENDENCIES_DIR}/glslang )
	set( SPIRV_ROOT_DIR ${DEPENDENCIES_DIR}/glslang )
	set( GLM_ROOT_DIR ${DEPENDENCIES_DIR}/glm )
	set( FFMPEG_DIR ${DEPENDENCIES_DIR}/ffmpeg )
	set( OpenCV_DIR ${DEPENDENCIES_DIR}/opencv )
	set( OpenCV_LIBRARY_DIR ${DEPENDENCIES_DIR}/opencv/lib )
	set( OpenCV_LIB_DIR_OPT ${DEPENDENCIES_DIR}/opencv/lib )

	if ( EXISTS ${DEPENDENCIES_DIR}/wxWidgets )
		set( wxWidgets_ROOT_DIR ${DEPENDENCIES_DIR}/wxWidgets CACHE PATH "wxWidgets root directory" )
		set( wxWidgets_LIB_DIR ${DEPENDENCIES_DIR}/wxWidgets/lib/vc_lib CACHE PATH "wxWidgets libraries directory" )
	endif ()
endif ()

#--------------------------------------------------------------------------------------------------
#	Looking for needed libraries
#--------------------------------------------------------------------------------------------------
if ( CMAKE_COMPILER_IS_GNUCXX )
	DumpCompilerVersion( Castor_COMPILER_VERSION )
	msg_debug( "GCC version : ${Castor_COMPILER_VERSION}" )
	if ( ${Castor_COMPILER_VERSION} LESS "49" )
		message( FATAL_ERROR "Your compiler is too old, consider an update." )
	else ()
		set( Boost_FOUND TRUE )
	endif ()
elseif ( ${CMAKE_CXX_COMPILER_ID} MATCHES "Clang" )
	DumpCompilerVersion( Castor_COMPILER_VERSION )
	msg_debug( "clang version : ${Castor_COMPILER_VERSION}" )
	set( Boost_FOUND TRUE )
elseif ( MSVC )
	if ( MSVC60 OR MSVC70 OR MSVC71 OR MSVC80 OR MSVC10 OR MSVC11 OR MSVC12 )
		message( FATAL_ERROR "Your compiler is too old, consider an update." )
	else ()
		set( Boost_FOUND TRUE )
	endif ()
else ()
	message( FATAL_ERROR "Unsupported compiler." )
endif ()

add_compile_options( $<$<CXX_COMPILER_ID:MSVC>:/MP> )

find_package( Doxygen )

#--------------------------------------------------------------------------------------------------
#	Adding include dirs to include search path
#--------------------------------------------------------------------------------------------------
set( RENDERERLIB_DIR external/RendererLib/source/Renderer )
include_directories( Core/CastorUtils/Src )
include_directories( ${RENDERERLIB_DIR}/Renderer/Src )
include_directories( Core/GlslWriter/Src )
include_directories( Core/Castor3D/Src )
include_directories( ${CMAKE_CURRENT_BINARY_DIR}/Core/CastorUtils/Src )
include_directories( ${CMAKE_CURRENT_BINARY_DIR}/${RENDERERLIB_DIR}/Renderer/Src )
include_directories( ${CMAKE_CURRENT_BINARY_DIR}/Core/Castor3D/Src )

if( Boost_FOUND )
	include_directories( ${Boost_INCLUDE_DIRS} )
	message( STATUS "+ Found Boost" )
endif()
if( DOXYGEN_FOUND )
	message( STATUS "+ Found Doxygen (version ${DOXYGEN_VERSION})" )
endif ()

#--------------------------------------------------------------------------------------------------
#	Normalising libraries names
#--------------------------------------------------------------------------------------------------
#Boost Libs
set( BoostLibraries "")
foreach(Lib ${Boost_LIBRARIES})
	if (BoostLibraries)
		set(BoostLibraries "${BoostLibraries}|${Lib}")
	else()
		set(BoostLibraries "${Lib}")
	endif()
endforeach()

msg_debug( "CMAKE_BUILD_TYPE : ${CMAKE_BUILD_TYPE}")
msg_debug( "CMAKE_C_FLAGS    : ${CMAKE_C_FLAGS}" )
msg_debug( "CMAKE_CXX_FLAGS  : ${CMAKE_CXX_FLAGS}" )

set( CastorMinLibraries
	${BoostLibraries}
)

set( CastorBinsDependencies
	CastorUtils
	Castor3D
)

#--------------------------------------------------------------------------------------------------
#	MSVC specifics
#--------------------------------------------------------------------------------------------------
if( MSVC )
	# We remove the existing /Zm flag to replace it by /Zm300
	string( REGEX REPLACE ".*(/Zm[0-9]+).*" "\\1" ZmFlag ${CMAKE_CXX_FLAGS} )
	if( (NOT "${ZmFlag}" STREQUAL "") AND (NOT "${CMAKE_CXX_FLAGS}" STREQUAL "${ZmFlag}") )
		string( REPLACE ${ZmFlag} "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} )
	endif()
	set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zm300" )
	# Copy Rebracer.xml file so we can use Rebracer extension to uniformise code formatting
	file( COPY ${CMAKE_SOURCE_DIR}/../data/VS/Rebracer.xml DESTINATION ${CMAKE_BINARY_DIR} )
endif()

add_definitions(
 -DVERSION_MAJOR=${VERSION_MAJOR}
 -DVERSION_MINOR=${VERSION_MINOR}
 -DVERSION_BUILD=${VERSION_BUILD}
)

option( CASTOR_BUILDGRP_TEST "Build test projects" FALSE )
option( CASTOR_BUILDGRP_SETUP "Build setup projects" FALSE )

# Small macro to add subdirectory files to current target source and header files
macro( parse_subdir_files _FOLDER _GROUP )
	file( GLOB _FOLDER_HEADER_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/${_FOLDER}/*.h
		${CMAKE_CURRENT_SOURCE_DIR}/${_FOLDER}/*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/${_FOLDER}/*.inl
 )
	file( GLOB _FOLDER_SOURCE_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/${_FOLDER}/*.c
		${CMAKE_CURRENT_SOURCE_DIR}/${_FOLDER}/*.cpp
 )
	source_group( "Header Files\\${_GROUP}" FILES ${_FOLDER_HEADER_FILES} )
	source_group( "Source Files\\${_GROUP}" FILES ${_FOLDER_SOURCE_FILES} )
endmacro()

if ( MSVC OR NOT "${CMAKE_BUILD_TYPE}" STREQUAL "" )
	if( Boost_FOUND )
		if ( CASTOR_BUILDGRP_TEST )
			enable_testing()
		endif ()

		add_subdirectory( ${RENDERERLIB_DIR} )
		add_subdirectory( Core )
		if ( ${CASTOR_BUILD_CASTOR3D} )
			add_subdirectory( Plugins )
			add_subdirectory( Samples )
			add_subdirectory( Interop )
		endif ()
		add_subdirectory( Tools )

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
		set( msgtest "Test projects :" )
		CoreSummary( ${msg} ${msgtest} )
		if ( ${CASTOR_BUILD_CASTOR3D} )
			PluginsSummary( ${msg} ${msgtest} )
			SamplesSummary( ${msg} ${msgtest} )
			InteropSummary( ${msg} ${msgtest} )
		endif ()
		ToolsSummary( ${msg} ${msgtest} )
		message( STATUS "${msg}\n" )
		if( ${CASTOR_BUILDGRP_TEST} )
			message( STATUS "${msgtest}\n" )
		endif ()

		if ( ${CASTOR_BUILDGRP_SETUP} )
			if ( ( "${PROJECTS_PACKAGE_WIX}" STREQUAL "ON" ) OR ( "${PROJECTS_PACKAGE_DEB}" STREQUAL "ON" ) OR ( "${PROJECTS_PACKAGE_RPM}" STREQUAL "ON" ) )
				get_filename_component( PARENT_DIR ${CMAKE_SOURCE_DIR} PATH )
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
				configure_file( ${PARENT_DIR}/LICENSE ${CMAKE_BINARY_DIR}/LICENSE.txt COPYONLY )
				set( CPACK_RESOURCE_FILE_LICENSE "${CMAKE_BINARY_DIR}/LICENSE.txt" )
				set( CMAKE_SYSTEM_ARCHITECTURE ${PROJECTS_PLATFORM} )

				if ( "${PROJECTS_PACKAGE_WIX}" STREQUAL "ON" )
					set( CPACK_GENERATOR "WIX" )
					set( CPACK_PACKAGE_INSTALL_DIRECTORY "Castor3D" )
					set( CPACK_WIX_UPGRADE_GUID "42BDC578-D000-4995-A2F7-3C94B618CBF3" )
					set( CPACK_WIX_PRODUCT_GUID "02ACFE05-CFCE-4E1D-85DA-59A068DDCF70" )
					set( CPACK_WIX_PRODUCT_ICON "${PARENT_DIR}/install/castor.ico" )
					set( CPACK_WIX_UI_DIALOG "${PARENT_DIR}/install/dialog.bmp" )
					set( CPACK_WIX_UI_BANNER "${PARENT_DIR}/install/banner.bmp" )
					set( CPACK_WIX_PROGRAM_MENU_FOLDER "Castor3D" )
					set( CPACK_WIX_CULTURES "fr-FR;en-EN" )
					include(CPack)
				elseif ( "${PROJECTS_PACKAGE_DEB}" STREQUAL "ON" )
					set( CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX} )
					set( CPACK_DEB_COMPONENT_INSTALL ON )
					set( CPACK_GENERATOR "DEB" )
					set( CPACK_DEBIAN_PACKAGE_MAINTAINER "Sylvain Doremus" )
					set( CPACK_DEBIAN_PACKAGE_DEPENDS "${CPACK_DEBIAN_PACKAGE_DEPENDS}libfreetype6 (>= 2.5.2), libfreeimage3 (>= 3.15.4), zlib1g (>= 1.2.8)" )
					set( CPACK_DEBIAN_PACKAGE_ARCHITECTURE "${CMAKE_SYSTEM_ARCHITECTURE}" )
					set( CPACK_DEBIAN_PACKAGE_SECTION "devel" )
					set( CPACK_DEBIAN_PACKAGE_PRIORITY "optional" )
					include(CPack)
				elseif ( "${PROJECTS_PACKAGE_RPM}" STREQUAL "ON" )
					set( ${Castor_PACKAGE_DIR} ${CMAKE_BINARY_DIR}/_CPack_Packages/Linux/RPM )
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

			cpack_add_component_group( Main
				EXPANDED
				DISPLAY_NAME "Main"
				DESCRIPTION "Main components to run Castor3D."
			)

			cpack_add_component_group( Core
				DISPLAY_NAME "Core libraries"
				DESCRIPTION "Base libraries needed to use Castor3D."
				PARENT_GROUP Main
			)

			cpack_add_component_group( Plugins
				DISPLAY_NAME "Plugins"
				DESCRIPTION "Plugins usable in Castor3D."
				PARENT_GROUP Main
			)

			cpack_add_component_group( Tools
				DISPLAY_NAME "Tools"
				DESCRIPTION "Some tools"
				PARENT_GROUP Main
			)

			cpack_add_component_group( Samples
				DISPLAY_NAME "Samples"
				DESCRIPTION "Sample applications and scenes using Castor3D."
				PARENT_GROUP Main
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
		set( msg "Castor projects can't be built because of following missing libraries :" )
		if( Boost_NEEDED AND NOT Boost_FOUND )
			set( msg "${msg}\n\tBoost thread" )
		endif()
		message( SEND_ERROR ${msg} )
	endif()
else()
	message( SEND_ERROR "Please select a build type (Debug or Release)" )
endif()
