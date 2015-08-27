cmake_minimum_required( VERSION 2.8 )
#--------------------------------------------------------------------------------------------------
#	Initial configurations
#--------------------------------------------------------------------------------------------------
# Set project name, used in folders and in workspace creation
set( MAIN_PROJECT_NAME "Castor3D" )

# Set project version numbers
set( VERSION_MAJOR	0		)
set( VERSION_MINOR	7		)
set( VERSION_BUILD	1		)
set( VERSION_YEAR	2014	)

# Used to look for external modules
set( CMAKE_MODULE_PATH
	${CMAKE_SOURCE_DIR}/CMake
	${CMAKE_SOURCE_DIR}/CMake/Modules
)
set( CMAKE_TEMPLATES_DIR ${CMAKE_SOURCE_DIR}/CMake/Templates )
# Experimental Precompiled headers support for GCC
include( PCHSupport )
# Force Unicode projects (MBCS is past :P)
add_definitions( -DUNICODE -D_UNICODE )

set( CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Set only Debug and Release configuration types" FORCE )
    
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

#--------------------------------------------------------------------------------------------------
#	Looking for needed libraries
#--------------------------------------------------------------------------------------------------
set( Boost_NEEDED TRUE )

if ( CMAKE_COMPILER_IS_GNUCXX )
	DumpCompilerVersion( Castor_COMPILER_VERSION )
	msg_debug( "GCC version : ${Castor_COMPILER_VERSION}" )
	if ( ${Castor_COMPILER_VERSION} LESS "46" )
		find_package( Boost REQUIRED thread system chrono )
	elseif ( WIN32 )
		find_package( Boost REQUIRED thread system )
    else ()
		set( Boost_NEEDED FALSE )
		set( Boost_FOUND TRUE )
	endif ()
elseif ( ${CMAKE_CXX_COMPILER_ID} MATCHES "Clang" )
	DumpCompilerVersion( Castor_COMPILER_VERSION )
	msg_debug( "clang version : ${Castor_COMPILER_VERSION}" )
	set( Boost_NEEDED FALSE )
	set( Boost_FOUND TRUE )
elseif ( BORLAND )
	find_package( Boost REQUIRED thread system )
elseif ( NOT MSVC11 AND NOT MSVC12 )
	find_package( Boost REQUIRED thread system chrono )
else ()
	set( Boost_NEEDED FALSE )
	set( Boost_FOUND TRUE )
endif ()

find_package( Doxygen )

#--------------------------------------------------------------------------------------------------
#	Adding include dirs to include search path
#--------------------------------------------------------------------------------------------------
include_directories( Core/CastorUtils/Src )
include_directories( Core/Castor3D/Src )
include_directories( ${CMAKE_CURRENT_BINARY_DIR}/Core/Castor3D/Src )

if( Boost_FOUND AND Boost_NEEDED )
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

if( MSVC )
	# We remove the existing /Zm flag to replace it by /Zm300
	string( REGEX REPLACE ".*(/Zm[0-9]+).*" "\\1" ZmFlag ${CMAKE_CXX_FLAGS} )
	if( (NOT "${ZmFlag}" STREQUAL "") AND (NOT "${CMAKE_CXX_FLAGS}" STREQUAL "${ZmFlag}") )
		string( REPLACE ${ZmFlag} "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} )
	endif()
	set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zm300" )
endif()

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

if ( MSVC OR NOT "${CMAKE_BUILD_TYPE}" STREQUAL "" )
	if( Boost_FOUND )
		add_subdirectory( Core )
		add_subdirectory( Importers )
		add_subdirectory( Dividers )
		add_subdirectory( Techniques )
		add_subdirectory( Renderers )
		add_subdirectory( Samples )
		add_subdirectory( Tools )
		add_subdirectory( Bindings )
		add_subdirectory( PostEffects )
	
		CoreInit()
		ImportersInit()
		DividersInit()
		TechniquesInit()
		RenderersInit()
		SamplesInit()
		ToolsInit()
		BindingsInit()
		PostEffectsInit()
	
		CoreBuild()
		ImportersBuild()
		DividersBuild()
		TechniquesBuild()
		RenderersBuild()
		SamplesBuild()
		ToolsBuild()
		BindingsBuild()
		PostEffectsBuild()
	
		set( msg "\nCastor following projects will be built" )
		CoreSummary( ${msg} )
		ImportersSummary( ${msg} )
		DividersSummary( ${msg} )
		TechniquesSummary( ${msg} )
		RenderersSummary( ${msg} )
		SamplesSummary( ${msg} )
		ToolsSummary( ${msg} )
		BindingsSummary( ${msg} )
		PostEffectsSummary( ${msg} )
		message( STATUS ${msg} )

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
			set( CPACK_RESOURCE_FILE_LICENSE "${PARENT_DIR}/LGPL.txt" )

			if ( "${PROJECTS_PACKAGE_WIX}" STREQUAL "ON" )
				set( CPACK_GENERATOR "WIX" )
				set( CPACK_PACKAGE_INSTALL_DIRECTORY "Castor3D" )
				set( CPACK_WIX_UPGRADE_GUID "6B17505B-A8B5-4EEC-8978-1BF24715A8A1" )
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
				set( CPACK_DEBIAN_PACKAGE_DEPENDS "${CPACK_DEBIAN_PACKAGE_DEPENDS}libfreetype6-dev (>= 2.4.2), libfreeimage-dev (>= 3.10.0)" )
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
				set( CPACK_RPM_PACKAGE_LICENSE "LGPL" )
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

		CoreSetup()
		ImportersSetup()
		DividersSetup()
		TechniquesSetup()
		RenderersSetup()
		SamplesSetup()
		ToolsSetup()
		BindingsSetup()
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
