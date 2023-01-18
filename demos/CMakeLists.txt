get_filename_component( CASTOR_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY )
get_filename_component( CASTOR_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR} DIRECTORY )

option( CASTOR_BUILD_DEMO_CASTOR_TD "Build Castor Tower Defense (needs GuiCommon library)" TRUE )
if ( MSVC )
	option( CASTOR_BUILD_DEMO_CASTOR_VIEWER_SHARP "Build CastorViewerSharp (needs ComCastor3D library)" FALSE )
endif ()

function( DemosInit )
	set( CTd "no (Not wanted)" PARENT_SCOPE )
	if ( MSVC )
		set( CViewSh "no (Not wanted)" PARENT_SCOPE )
	else ()
		set( CViewSh "no (Not available)" PARENT_SCOPE )
	endif ()
endfunction( DemosInit )

function( DemosBuild )
	if( CASTOR_BUILDGRP_DEMO )
		set( CPACK_DEBIAN_PACKAGE_DEPENDS "${CPACK_DEBIAN_PACKAGE_DEPENDS}libwxgtk3.0 (>= 3.0)" )
		set( DEMOS_TARGETS )
		if( CASTOR_BUILD_DEMO_CASTOR_TD )
			if( APPLE )
				set( GTK_FOUND TRUE )
				set( GTK2_FOUND TRUE )
				set( GTK_LIBRARIES
					"-framework AppKit"
					"-framework QuartzCore"
					"-framework Metal"
				)
				set( GTK2_LIBRARIES ${GTK_LIBRARIES} )
			elseif ( WIN32 )
				set( GTK_FOUND TRUE )
				set( GTK2_FOUND TRUE )
			else()
				find_package( GTK3 QUIET )
				find_package( X11 QUIET )
				if ( GTK3_FOUND )
					set( GTK_FOUND ${GTK3_FOUND} )
					set( GTK_INCLUDE_DIRS ${GTK3_INCLUDE_DIRS} )
					set( GTK_LIBRARIES ${GTK3_LIBRARIES} )
					set( GTK2_FOUND ${GTK_FOUND} )
					set( GTK2_INCLUDE_DIRS ${GTK_INCLUDE_DIRS} )
					set( GTK2_LIBRARIES ${GTK_LIBRARIES} )
				else ()
					set( GTK_FOUND FALSE )
					set( GTK2_FOUND FALSE )
				endif ()
			endif()
			if ( WIN32 AND CASTOR_USE_DEPS_WXWIDGETS )
				find_package( wxWidgets CONFIG COMPONENTS core base adv aui stc propgrid )
			elseif ( CASTOR_USE_SYSTEM_WXWIDGETS )
				find_package( wxWidgets REQUIRED COMPONENTS core base stc aui adv propgrid )
			else ()
				find_package( wxWidgets CONFIG COMPONENTS core base stc aui adv propgrid )
			endif ()

			if( wxWidgets_FOUND AND GTK_FOUND )
				set( Castor3DIncludeDirs
					${CASTOR_BINARY_DIR}/include/Core
					${CASTOR_SOURCE_DIR}/include/Core
				)
				if( CASTOR_BUILD_TOOL_GUICOMMON )
					if( CASTOR_BUILD_DEMO_CASTOR_TD )
						set( Build ${CTd} )
						add_subdirectory( CastorDvpTD )
						set( CPACK_PACKAGE_EXECUTABLES
							${CPACK_PACKAGE_EXECUTABLES}
							CastorDvpTD "CastorDvpTD"
							PARENT_SCOPE )
						set( CTd ${Build} PARENT_SCOPE )
					endif()
				elseif ( CASTOR_BUILD_DEMO_CASTOR_TD )
					message( WARNING "You need GuiCommon to be able to build CastorDvpTD" )
				endif()
			else()
				if( NOT wxWidgets_FOUND )
					set( CTestLcr "no (wxWidgets libraries not found)" PARENT_SCOPE )
					set( CTd "no (wxWidgets libraries not found)" PARENT_SCOPE )
				else()
					set( CTestLcr "no (GTK libraries not found)" PARENT_SCOPE )
					set( CTd "no (GTK libraries not found)" PARENT_SCOPE )
				endif()
			endif()
		endif()
		if ( CASTOR_BUILD_DEMO_CASTOR_VIEWER_SHARP )
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
endfunction( DemosBuild )
    
function( DemosSummary )
	if( CASTOR_BUILDGRP_DEMO )
		set( msg "  Demo apps:" )
		if( CASTOR_BUILD_DEMO_CASTOR_TD )
			set( msg_tmp "${msg_tmp}\n    Castor TD            ${CTd}" )
		endif ()
		if( CASTOR_BUILD_DEMO_CASTOR_VIEWER_SHARP )
			set( msg_tmp "${msg_tmp}\n    CastorViewerSharp    ${CViewSh}" )
		endif ()
		set( msg "${msg}${msg_tmp}" )
		message( STATUS "${msg}\n" )
	endif ()
endfunction( DemosSummary )

function( DemosSetup )
	if( CASTOR_BUILDGRP_DEMO )
		if( CASTOR_BUILD_TOOL_GUICOMMON )
			if( CASTOR_BUILD_DEMO_CASTOR_TD )
				cpack_add_component( CastorDvpTD
					DISPLAY_NAME "CastorDvpTD"
					DESCRIPTION "A small Tower Defense game, using Castor3D."
					GROUP Demos
					DEPENDS GuiCommon
				)
			endif ()
		endif ()
		if( CASTOR_BUILD_DEMO_CASTOR_VIEWER_SHARP )
			if ( MSVC )
				cpack_add_component( CastorViewerSharp
					DISPLAY_NAME "CastorViewerSharp"
					DESCRIPTION "A simple scene viewer, using Castor3D and developed in C#/WPF."
					GROUP Demos
					DEPENDS Castor3D
				)
			endif ()
		endif ()
	endif ()
endfunction( DemosSetup )

if ( CASTOR_BUILDGRP_DEMO AND CASTOR_BUILD_CASTOR3D )
	DemosInit()
	DemosBuild()
	DemosSummary()
	DemosSetup()
endif ()