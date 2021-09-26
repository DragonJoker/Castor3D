get_filename_component( CASTOR_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY )
get_filename_component( CASTOR_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR} DIRECTORY )

option( CASTOR_BUILD_SAMPLE_OCEAN_LIGHTING "Build Ocean Lighting FFT sample (needs wxWidgets library)" FALSE )
option( CASTOR_BUILD_SAMPLE_CASTOR_TD "Build Castor Tower Defense (needs GuiCommon library)" TRUE )
if ( MSVC )
	option( CASTOR_BUILD_SAMPLE_CASTOR_VIEWER_SHARP "Build CastorViewerSharp (needs ComCastor3D library)" FALSE )
endif ()

function( SamplesInit )
	set( Ocean "no (Not wanted)" PARENT_SCOPE )
	set( CTd "no (Not wanted)" PARENT_SCOPE )
	if ( MSVC )
		set( CViewSh "no (Not wanted)" PARENT_SCOPE )
	else ()
		set( CViewSh "no (Not available)" PARENT_SCOPE )
	endif ()
endfunction( SamplesInit )

function( SamplesBuild )
	if( CASTOR_BUILDGRP_SAMPLE )
		set( CPACK_DEBIAN_PACKAGE_DEPENDS "${CPACK_DEBIAN_PACKAGE_DEPENDS}libwxgtk3.0 (>= 3.0)" )
		set( SAMPLES_TARGETS )
		if( CASTOR_BUILD_SAMPLE_OCEAN_LIGHTING
			OR CASTOR_BUILD_SAMPLE_CASTOR_TD )
			if( APPLE )
				set( GTK_FOUND TRUE )
				set( GTK_LIBRARIES
					"-framework AppKit"
					"-framework QuartzCore"
					"-framework Metal"
				)
			elseif ( WIN32 )
				set( GTK_FOUND TRUE )
			else()
				find_package( GTK3 )
				find_package( X11 )
				if ( GTK3_FOUND )
					set( GTK_FOUND ${GTK3_FOUND} )
					set( GTK_INCLUDE_DIRS ${GTK3_INCLUDE_DIRS} )
					set( GTK_LIBRARIES ${GTK3_LIBRARIES} )
				else ()
					set( GTK_FOUND FALSE )
				endif ()
			endif()

			#we look for wxWidgets
			if ( WIN32 )
				set( wxWidgets_IGNORE_VCPKG ON )
				find_package( wxWidgets QUIET COMPONENTS core base scintilla stc aui adv propgrid regex )

				if ( CASTOR_USE_DEPS_WXWIDGETS AND NOT wxWidgets_FOUND )
					set( wxWidgets_LIB_DIR ${DEPENDENCIES_DIR}/wxWidgets/lib CACHE PATH "wxWidgets libraries directory" )
					find_package( wxWidgets QUIET COMPONENTS core base scintilla stc aui adv propgrid regex )
				endif ()
			else ()
				find_package( wxWidgets COMPONENTS core base stc aui adv propgrid )
			endif ()
			if( wxWidgets_FOUND AND GTK_FOUND )
				set( Castor3DIncludeDirs
					${CASTOR_BINARY_DIR}/include/Core
					${CASTOR_SOURCE_DIR}/include/Core
				)
				if( CASTOR_BUILD_TOOL_GUICOMMON )
					if( CASTOR_BUILD_SAMPLE_CASTOR_TD )
						set( Build ${CTd} )
						add_subdirectory( CastorDvpTD )
						set( CPACK_PACKAGE_EXECUTABLES
							${CPACK_PACKAGE_EXECUTABLES}
							CastorDvpTD "CastorDvpTD"
							PARENT_SCOPE )
						set( CTd ${Build} PARENT_SCOPE )
					endif()
				elseif ( CASTOR_BUILD_SAMPLE_CASTOR_TD )
					message( WARNING "You need GuiCommon to be able to build CastorDvpTD" )
				endif()

				if( CASTOR_BUILD_SAMPLE_OCEAN_LIGHTING )
					set( Build ${Ocean} )
					add_subdirectory( OceanLighting )
					set( Ocean ${Build} PARENT_SCOPE )
					set( CPACK_PACKAGE_EXECUTABLES
						${CPACK_PACKAGE_EXECUTABLES}
						OceanLighting "OceanLighting"
						PARENT_SCOPE )
				endif()
			else()
				if( NOT wxWidgets_FOUND )
					set( CTestLcr "no (wxWidgets libraries not found)" PARENT_SCOPE )
					set( Ocean "no (wxWidgets libraries not found)" PARENT_SCOPE )
					set( CTd "no (wxWidgets libraries not found)" PARENT_SCOPE )
				else()
					set( CTestLcr "no (GTK libraries not found)" PARENT_SCOPE )
					set( Ocean "no (GTK libraries not found)" PARENT_SCOPE )
					set( CTd "no (GTK libraries not found)" PARENT_SCOPE )
				endif()
			endif()
		endif()
		if ( CASTOR_BUILD_SAMPLE_CASTOR_VIEWER_SHARP )
			if ( MSVC )
				if ( CASTOR_BUILDGRP_INTEROP AND CASTOR_BUILD_INTEROP_COM )
					set( Build ${CViewSh} )
					add_subdirectory( CastorViewerSharp )
					set( CPACK_PACKAGE_EXECUTABLES
						${CPACK_PACKAGE_EXECUTABLES}
						CastorViewerSharp "CastorViewer #"
						PARENT_SCOPE )
					set( CViewSh ${Build} PARENT_SCOPE )
				else ()
					set( CViewSh "no (ComCastor3D not built)" PARENT_SCOPE )
				endif ()
			endif ()
		endif ()

		set( CastorMinLibraries
			${CastorMinLibraries}
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
	endif ()
endfunction( SamplesBuild )
    
function( SamplesSummary )
	if( CASTOR_BUILDGRP_SAMPLE )
		set( msg "  Sample apps:" )
		if( CASTOR_BUILD_SAMPLE_OCEAN_LIGHTING )
			set( msg_tmp "${msg_tmp}\n    OceanLighting        ${Ocean}" )
		endif ()
		if( CASTOR_BUILD_SAMPLE_CASTOR_TD )
			set( msg_tmp "${msg_tmp}\n    Castor TD            ${CTd}" )
		endif ()
		if( CASTOR_BUILD_SAMPLE_CASTOR_VIEWER_SHARP )
			set( msg_tmp "${msg_tmp}\n    CastorViewerSharp    ${CViewSh}" )
		endif ()
		set( msg "${msg}${msg_tmp}" )
		message( STATUS "${msg}\n" )
	endif ()
endfunction( SamplesSummary )

function( SamplesSetup )
	if( CASTOR_BUILDGRP_SAMPLE )
		if( CASTOR_BUILD_TOOL_GUICOMMON )
			if( CASTOR_BUILD_SAMPLE_CASTOR_TD )
				cpack_add_component( CastorDvpTD
					DISPLAY_NAME "CastorDvpTD"
					DESCRIPTION "A small Tower Defense game, using Castor3D."
					GROUP Samples
					DEPENDS GuiCommon
					INSTALL_TYPES Full
				)
			endif ()
		endif ()
		if( CASTOR_BUILD_SAMPLE_OCEAN_LIGHTING )
			cpack_add_component( OceanLighting
				DISPLAY_NAME "OceanLighting"
				DESCRIPTION "Eric Bruneton's OceanLightingFFT demo adapted for Castor3D."
				GROUP Samples
				DEPENDS Castor3D
				INSTALL_TYPES Full
			)
		endif ()
		if( CASTOR_BUILD_SAMPLE_CASTOR_VIEWER_SHARP )
			if ( MSVC )
				cpack_add_component( CastorViewerSharp
					DISPLAY_NAME "CastorViewerSharp"
					DESCRIPTION "A simple scene viewer, using Castor3D and developed in C#/WPF."
					GROUP Samples
					DEPENDS Castor3D
					INSTALL_TYPES Full
				)
			endif ()
		endif ()
	endif ()
endfunction( SamplesSetup )

if ( CASTOR_BUILDGRP_SAMPLE AND CASTOR_BUILD_CASTOR3D )
	SamplesInit()
	SamplesBuild()
	SamplesSummary()
endif ()
