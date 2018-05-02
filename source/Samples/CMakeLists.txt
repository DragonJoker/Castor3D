option( CASTOR_BUILDGRP_SAMPLE "Build Castor3D samples." FALSE )
option( CASTOR_BUILD_SAMPLE_GUICOMMON "Build GuiCommon library (needs wxWidgets library)" TRUE )
option( CASTOR_BUILD_SAMPLE_CASTOR_VIEWER "Build CastorViewer (needs GuiCommon library)" TRUE )
option( CASTOR_BUILD_SAMPLE_CASTOR_TEST_LAUNCHER "Build CastorTestLauncher" FALSE )
option( CASTOR_BUILD_SAMPLE_OCEAN_LIGHTING "Build Ocean Lighting FFT sample (needs wxWidgets library)" FALSE )
option( CASTOR_BUILD_SAMPLE_CASTOR_TD "Build Castor Tower Defense (needs GuiCommon library)" TRUE )
if ( MSVC )
	option( CASTOR_BUILD_SAMPLE_CASTOR_VIEWER_SHARP "Build CastorViewerSharp (needs ComCastor3D library)" FALSE )
endif ()
option( CASTOR_SAMPLE_CASTOR_VIEWER_RECORDS "Allow video recording from CastorViewer" FALSE )

function( SamplesInit )
	set( GuiCmn "no (Not wanted)" PARENT_SCOPE )
	set( CViewer "no (Not wanted)" PARENT_SCOPE )
	set( CTestLcr "no (Not wanted)" PARENT_SCOPE )
	set( ImgConv "no (Not wanted)" PARENT_SCOPE )
	set( Ocean "no (Not wanted)" PARENT_SCOPE )
	set( CTd "no (Not wanted)" PARENT_SCOPE )
	if ( MSVC )
		set( CViewSh "no (Not wanted)" PARENT_SCOPE )
	else ()
		set( CViewSh "no (Not available)" PARENT_SCOPE )
	endif ()
endfunction( SamplesInit )

function( SamplesBuild )
	if( ${CASTOR_BUILDGRP_SAMPLE} )
		set( CPACK_DEBIAN_PACKAGE_DEPENDS "${CPACK_DEBIAN_PACKAGE_DEPENDS}libwxgtk3.0 (>= 3.0)" )
		if( ${CASTOR_BUILD_SAMPLE_GUICOMMON}
			OR ${CASTOR_BUILD_SAMPLE_CASTOR_VIEWER}
			OR ${CASTOR_BUILD_SAMPLE_CASTOR_TEST_LAUNCHER}
			OR ${CASTOR_BUILD_SAMPLE_OCEAN_LIGHTING}
			OR ${CASTOR_BUILD_SAMPLE_CASTOR_TD} )
			if( NOT WIN32 )
				find_package( GTK2 )
			else()
				set( GTK2_FOUND TRUE )
			endif()

			set( GUICOMMON_C_FLAGS "" )
			set( GUICOMMON_CXX_FLAGS "" )
			set( GUICOMMON_LINKS "" )

			if ( ${CASTOR_SAMPLE_CASTOR_VIEWER_RECORDS} )
				find_package( FFMPEG COMPONENTS avcodec avformat avutil swscale swresample )

				if( FFMPEG_FOUND )
					include_directories( ${FFMPEG_INCLUDE_DIR} )
					set( GUICOMMON_C_FLAGS "${GUICOMMON_C_FLAGS} -DGUICOMMON_RECORDS -DGUICOMMON_FFMPEG")
					set( GUICOMMON_CXX_FLAGS "${GUICOMMON_CXX_FLAGS} -DGUICOMMON_RECORDS -DGUICOMMON_FFMPEG")
					message( STATUS "+ Found FFMPEG" )

					#FFMPEG Libs
					set( FFMpegLibraries "" )
					foreach(Lib ${FFMPEG_LIBRARIES})
						if (FFMpegLibraries)
							set(FFMpegLibraries "${FFMpegLibraries}|${Lib}")
						else()
							set(FFMpegLibraries "${Lib}")
						endif()
					endforeach()
				endif()
			endif ()

			#we look for wxWidgets
			if (WIN32)
				find_package( wxWidgets COMPONENTS stc scintilla core base aui adv propgrid )
			else ()
				find_package( wxWidgets COMPONENTS core base stc aui adv propgrid )
			endif()

			if( wxWidgets_FOUND AND GTK2_FOUND )
				message( STATUS "+ Found GTK2" )
				message( STATUS "+ Found wxWidgets" )
				include_directories( ${GTK2_INCLUDE_DIRS} )
				include_directories( ${wxWidgets_INCLUDE_DIRS} )
				#wxWidgets Libs
				set( wxWidgetsLibraries "" )
				foreach( Lib ${wxWidgets_LIBRARIES} )
					if( wxWidgetsLibraries )
						set( wxWidgetsLibraries "${wxWidgetsLibraries}|${Lib}" )
					else()
						set( wxWidgetsLibraries "${Lib}" )
					endif()
				endforeach()

				foreach( Lib ${GTK2_LIBRARIES} )
					if( wxWidgetsLibraries )
						set( wxWidgetsLibraries "${wxWidgetsLibraries}|${Lib}" )
					else()
						set( wxWidgetsLibraries "${Lib}" )
					endif()
				endforeach()

				#Few more configuration for wxWidgets
				string( COMPARE EQUAL "${wxWidgets_USE_DEBUG}" "ON" IsWxDebug )
				set( wxWidgets_CXXFLAGS "${wxWidgets_CXX_FLAGS} -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES" )

				if( NOT WIN32 )
					set( wxWidgets_CXXFLAGS "${wxWidgets_CXXFLAGS} -D__WXGTK__" )
				endif()

				if( IsWxDebug )
					set( wxWidgets_CXXFLAGS "${wxWidgets_CXXFLAGS} -D__WXDEBUG__" )
				endif()

				if( ${CASTOR_BUILD_SAMPLE_CASTOR_TEST_LAUNCHER} )
					set( Build ${CTestLcr} )
					add_subdirectory( Samples/CastorTestLauncher )
					set( CTestLcr ${Build} PARENT_SCOPE )
				endif()

				if( ${CASTOR_BUILD_SAMPLE_GUICOMMON} )
					set( Build ${GuiCmn} )
					add_subdirectory( Samples/GuiCommon )
					set( GuiCmn ${Build} PARENT_SCOPE )

					if( ${CASTOR_BUILD_SAMPLE_CASTOR_VIEWER} )
						set( Build ${CViewer} )
						add_subdirectory( Samples/CastorViewer )
						set( CPACK_PACKAGE_EXECUTABLES
							${CPACK_PACKAGE_EXECUTABLES}
							CastorViewer "CastorViewer"
							PARENT_SCOPE )
						set( CViewer ${Build} PARENT_SCOPE )
					endif()

					if( ${CASTOR_BUILD_SAMPLE_CASTOR_TD} )
						set( Build ${CTd} )
						add_subdirectory( Samples/CastorDvpTD )
						set( CPACK_PACKAGE_EXECUTABLES
							${CPACK_PACKAGE_EXECUTABLES}
							CastorDvpTD "CastorDvpTD"
							PARENT_SCOPE )
						set( CTd ${Build} PARENT_SCOPE )
					endif()
				elseif ( ${CASTOR_BUILD_SAMPLE_CASTOR_VIEWER} )
					message( WARNING "You need GuiCommon to be able to build CastorViewer" )
				elseif ( ${CASTOR_BUILD_SAMPLE_CASTOR_TD} )
					message( WARNING "You need GuiCommon to be able to build CastorDvpTD" )
				endif()

				if( ${CASTOR_BUILD_SAMPLE_OCEAN_LIGHTING} )
					set( Build ${Ocean} )
					add_subdirectory( Samples/OceanLighting )
					set( Ocean ${Build} PARENT_SCOPE )
					set( CPACK_PACKAGE_EXECUTABLES
						${CPACK_PACKAGE_EXECUTABLES}
						OceanLighting "OceanLighting"
						PARENT_SCOPE )
				endif()
			else()
				if( NOT wxWidgets_FOUND )
					set( GuiCmn "no (wxWidgets libraries not found)" PARENT_SCOPE )
					set( CViewer "no (wxWidgets libraries not found)" PARENT_SCOPE )
					set( CTestLcr "no (wxWidgets libraries not found)" PARENT_SCOPE )
					set( Ocean "no (wxWidgets libraries not found)" PARENT_SCOPE )
					set( CTd "no (wxWidgets libraries not found)" PARENT_SCOPE )
				else()
					set( GuiCmn "no (GTK2 libraries not found)" PARENT_SCOPE )
					set( CViewer "no (GTK2 libraries not found)" PARENT_SCOPE )
					set( CTestLcr "no (GTK2 libraries not found)" PARENT_SCOPE )
					set( Ocean "no (GTK2 libraries not found)" PARENT_SCOPE )
					set( CTd "no (GTK2 libraries not found)" PARENT_SCOPE )
				endif()
			endif()
		endif()
		if ( ${CASTOR_BUILD_SAMPLE_CASTOR_VIEWER_SHARP} )
			if ( MSVC )
				if ( ${CASTOR_BUILDGRP_INTEROP} AND ${CASTOR_BUILD_INTEROP_COM} )
					set( Build ${CViewSh} )
					add_subdirectory( Samples/CastorViewerSharp )
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

		set( CastorBinsDependencies
			${CastorBinsDependencies}
			PARENT_SCOPE
		)
	endif ()
endfunction( SamplesBuild )
    
function( SamplesSummary msg )
	if( ${CASTOR_BUILDGRP_SAMPLE} )
		set( msg_tmp "\n\nSamples :" )
		if( ${CASTOR_BUILD_SAMPLE_GUICOMMON} )
			set( msg_tmp "${msg_tmp}\n    GuiCommon            ${GuiCmn}" )
		endif ()
		if( ${CASTOR_BUILD_SAMPLE_CASTOR_VIEWER} )
			set( msg_tmp "${msg_tmp}\n    CastorViewer         ${CViewer}" )
		endif ()
		if( ${CASTOR_BUILD_SAMPLE_CASTOR_TEST_LAUNCHER} )
			set( msg_tmp "${msg_tmp}\n    CastorTestLauncher   ${CTestLcr}" )
		endif ()
		if( ${CASTOR_BUILD_SAMPLE_OCEAN_LIGHTING} )
			set( msg_tmp "${msg_tmp}\n    OceanLighting        ${Ocean}" )
		endif ()
		if( ${CASTOR_BUILD_SAMPLE_CASTOR_TD} )
			set( msg_tmp "${msg_tmp}\n    Castor TD            ${CTd}" )
		endif ()
		if( ${CASTOR_BUILD_SAMPLE_CASTOR_VIEWER_SHARP} )
			set( msg_tmp "${msg_tmp}\n    CastorViewerSharp    ${CViewSh}" )
		endif ()
		set( msg "${msg}${msg_tmp}" PARENT_SCOPE )
	endif ()
endfunction( SamplesSummary )

function( SamplesSetup )
	if( ${CASTOR_BUILDGRP_SAMPLE} )
		if( ${CASTOR_BUILD_SAMPLE_GUICOMMON} )
			cpack_add_component( GuiCommon
				DISPLAY_NAME "GuiCommon"
				DESCRIPTION "Common classes and functions for CastorViewer and CastorShape."
				GROUP Samples
				DEPENDS Castor3D
				INSTALL_TYPES Developer Full
			)
			cpack_add_component( GuiCommon_dev
				DISPLAY_NAME "GuiCommon SDK"
				DESCRIPTION "Common classes and functions for CastorViewer and CastorShape."
				GROUP Development
				DEPENDS GuiCommon
				INSTALL_TYPES Developer Full
			)
			if( ${CASTOR_BUILD_SAMPLE_CASTOR_VIEWER} )
				cpack_add_component( CastorViewer
					DISPLAY_NAME "CastorViewer"
					DESCRIPTION "A simple scene viewer using Castor3D."
					GROUP Samples
					DEPENDS GuiCommon
					INSTALL_TYPES Full
				)
			endif ()
			if( ${CASTOR_BUILD_SAMPLE_CASTOR_TD} )
				cpack_add_component( CastorDvpTD
					DISPLAY_NAME "CastorDvpTD"
					DESCRIPTION "A small Tower Defense game, using Castor3D."
					GROUP Samples
					DEPENDS GuiCommon
					INSTALL_TYPES Full
				)
			endif ()
		endif ()
		if( ${CASTOR_BUILD_SAMPLE_OCEAN_LIGHTING} )
			cpack_add_component( OceanLighting
				DISPLAY_NAME "OceanLighting"
				DESCRIPTION "Eric Bruneton's OceanLightingFFT demo adapted for Castor3D."
				GROUP Samples
				DEPENDS Castor3D
				INSTALL_TYPES Full
			)
		endif ()
		if( ${CASTOR_BUILD_SAMPLE_CASTOR_VIEWER_SHARP} )
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
