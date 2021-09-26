get_filename_component( CASTOR_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY )
get_filename_component( CASTOR_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR} DIRECTORY )

option( CASTOR_BUILDGRP_TOOL "Build Castoro3D tools." OFF )
option( CASTOR_BUILD_TOOL_IMG_CONVERTER "Build ImgConverter (needs wxWidgets library)" ON )
option( CASTOR_BUILD_TOOL_MESH_UPGRADER "Build CastorMeshUpgrader" ON )
option( CASTOR_BUILD_TOOL_MESH_CONVERTER "Build CastorMeshConverter" ON )
option( CASTOR_BUILD_TOOL_CASTOR_TEST_LAUNCHER "Build CastorTestLauncher" ON )
option( CASTOR_BUILD_TOOL_HGT_MAP_TO_NML_MAP "Build HeightMapToNormalMap" ON )
option( CASTOR_BUILD_TOOL_GUICOMMON "Build GuiCommon library (needs wxWidgets library)" TRUE )
option( CASTOR_BUILD_TOOL_CASTOR_VIEWER "Build CastorViewer (needs GuiCommon library)" TRUE )
option( CASTOR_TOOL_CASTOR_VIEWER_RECORDS "Allow video recording from CastorViewer" FALSE )

function( ToolsInit )
	set( GuiCmn "no (Not wanted)" PARENT_SCOPE )
	set( CViewer "no (Not wanted)" PARENT_SCOPE )
	set( ImgConv "no (Not wanted)" PARENT_SCOPE )
	set( MshUpgd "no (Not wanted)" PARENT_SCOPE )
	set( MshConv "no (Not wanted)" PARENT_SCOPE )
	set( TestLcr "no (Not wanted)" PARENT_SCOPE )
	set( HgtNml "no (Not wanted)" PARENT_SCOPE )
endfunction( ToolsInit )

function( ToolsBuild )
	if( CASTOR_BUILDGRP_TOOL )
		set( Castor3DIncludeDirs
			${Castor3DIncludeDirs}
			${CASTOR_BINARY_DIR}/include/Core
			${CASTOR_SOURCE_DIR}/include/Core
		)

		if( CASTOR_BUILD_TOOL_IMG_CONVERTER
			OR CASTOR_BUILD_TOOL_CASTOR_TEST_LAUNCHER
			OR CASTOR_BUILD_TOOL_GUICOMMON
			OR CASTOR_BUILD_TOOL_CASTOR_VIEWER )
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
				find_package( GTK3 QUIET )
				find_package( X11 QUIET )
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
				find_package( wxWidgets COMPONENTS core base adv aui scintilla stc propgrid regex )

				if ( CASTOR_USE_DEPS_WXWIDGETS AND NOT wxWidgets_FOUND )
					set( wxWidgets_LIB_DIR ${DEPENDENCIES_DIR}/wxWidgets/lib CACHE PATH "wxWidgets libraries directory" )
					find_package( wxWidgets QUIET COMPONENTS core base adv aui scintilla stc propgrid regex )
				endif ()
			else ()
				find_package( wxWidgets COMPONENTS core base stc aui adv propgrid )
			endif ()

			if( wxWidgets_FOUND AND GTK_FOUND )
				unset( CastorToolsDefinitions )
				if ( APPLE )
					set( CastorToolsDefinitions
						${CastorToolsDefinitions}
						VK_USE_PLATFORM_MACOS_MVK
					)
				elseif ( WIN32 )
					set( CastorToolsDefinitions
						${CastorToolsDefinitions}
						VK_USE_PLATFORM_WIN32_KHR
					)
				elseif ( ANDROID )
					set( CastorToolsDefinitions
						${CastorToolsDefinitions}
						VK_USE_PLATFORM_ANDROID_KHR
					)
				else ()
					set( CastorToolsDefinitions
						${CastorToolsDefinitions}
						VK_USE_PLATFORM_XLIB_KHR
					)
				endif ()

				if( CASTOR_BUILD_TOOL_IMG_CONVERTER )
					set( Build ${ImgConv} )
					add_subdirectory( ImgConverter )
					set( CPACK_PACKAGE_EXECUTABLES
						${CPACK_PACKAGE_EXECUTABLES}
						ImgConverter "ImgConverter"
						PARENT_SCOPE )
					set( ImgConv ${Build} PARENT_SCOPE )
				endif ()

				if( CASTOR_BUILD_TOOL_HGT_MAP_TO_NML_MAP )
					set( Build ${HgtNml} )
					add_subdirectory( HeightMapToNormalMap )
					set( CPACK_PACKAGE_EXECUTABLES
						${CPACK_PACKAGE_EXECUTABLES}
						HeightMapToNormalMap "HeightMapToNormalMap"
						PARENT_SCOPE )
					set( HgtNml ${Build} PARENT_SCOPE )
				endif ()

				if( CASTOR_BUILD_CASTOR3D )
					if( CASTOR_BUILD_TOOL_GUICOMMON )
						set( Build ${GuiCmn} )
						add_subdirectory( GuiCommon )
						set( GuiCmn ${Build} PARENT_SCOPE )

						if( CASTOR_BUILD_TOOL_CASTOR_VIEWER )
							set( Build ${CViewer} )
							add_subdirectory( CastorViewer )
							set( CPACK_PACKAGE_EXECUTABLES
								${CPACK_PACKAGE_EXECUTABLES}
								CastorViewer "CastorViewer"
								PARENT_SCOPE )
							set( CViewer ${Build} PARENT_SCOPE )
						endif()
					elseif ( CASTOR_BUILD_TOOL_CASTOR_VIEWER )
						message( WARNING "You need GuiCommon to be able to build CastorViewer" )
					endif()
					if ( CASTOR_BUILD_TOOL_CASTOR_TEST_LAUNCHER )
						set( Build ${TestLcr} )
						add_subdirectory( CastorTestLauncher )
						set( CPACK_PACKAGE_EXECUTABLES
							${CPACK_PACKAGE_EXECUTABLES}
							CastorTestLauncher "CastorTestLauncher"
							PARENT_SCOPE )
						set( TestLcr ${Build} PARENT_SCOPE )
					endif ()
				endif ()
			else()
				if( NOT wxWidgets_FOUND )
					set( GuiCmn "no (wxWidgets libraries not found)" PARENT_SCOPE )
					set( CViewer "no (wxWidgets libraries not found)" PARENT_SCOPE )
					set( ImgConv "no (wxWidgets libraries not found)" PARENT_SCOPE )
					set( TestLcr "no (wxWidgets libraries not found)" PARENT_SCOPE )
				else()
					set( GuiCmn "no (GTK libraries not found)" PARENT_SCOPE )
					set( CViewer "no (GTK libraries not found)" PARENT_SCOPE )
					set( ImgConv "no (GTK libraries not found)" PARENT_SCOPE )
					set( TestLcr "no (GTK libraries not found)" PARENT_SCOPE )
				endif()
			endif()
		endif()

		if( CASTOR_BUILD_CASTOR3D )
			if ( CASTOR_BUILD_TOOL_MESH_UPGRADER )
				set( Build ${MshUpgd} )
				add_subdirectory( CastorMeshUpgrader )
				set( CPACK_PACKAGE_EXECUTABLES
					${CPACK_PACKAGE_EXECUTABLES}
					CastorMeshUpgrader "CastorMeshUpgrader"
					PARENT_SCOPE )
				set( MshUpgd ${Build} PARENT_SCOPE )
			endif()

			if( CASTOR_BUILD_TOOL_MESH_CONVERTER )
				set( Build ${MshConv} )
				add_subdirectory( CastorMeshConverter )
				set( CPACK_PACKAGE_EXECUTABLES
					${CPACK_PACKAGE_EXECUTABLES}
					CastorMeshConverter "CastorMeshConverter"
					PARENT_SCOPE )
				set( MshConv ${Build} PARENT_SCOPE )
			endif()
		endif()

		set( CastorMinLibraries
			${CastorMinLibraries}
			PARENT_SCOPE
		)

		set( Castor3DBinsDependencies
			${Castor3DBinsDependencies}
			PARENT_SCOPE
		)
	endif ()
endfunction( ToolsBuild )

function( ToolsSummary )
	if( CASTOR_BUILDGRP_TOOL )
		set( msg "  Tool apps:" )
		if( CASTOR_BUILD_TOOL_IMG_CONVERTER )
			set( msg_tmp "${msg_tmp}\n    ImgConverter         ${ImgConv}" )
		endif ()
		if( CASTOR_BUILD_TOOL_HGT_MAP_TO_NML_MAP )
			set( msg_tmp "${msg_tmp}\n    HeightMapToNormalMap ${HgtNml}" )
		endif ()
		if( CASTOR_BUILD_CASTOR3D )
			if( CASTOR_BUILD_TOOL_GUICOMMON )
				set( msg_tmp "${msg_tmp}\n    GuiCommon            ${GuiCmn}" )
			endif ()
			if( CASTOR_BUILD_TOOL_CASTOR_VIEWER )
				set( msg_tmp "${msg_tmp}\n    CastorViewer         ${CViewer}" )
			endif ()
			if( CASTOR_BUILD_TOOL_MESH_UPGRADER )
				set( msg_tmp "${msg_tmp}\n    CastorMeshUpgrader   ${MshUpgd}" )
			endif ()
			if( CASTOR_BUILD_TOOL_MESH_CONVERTER )
				set( msg_tmp "${msg_tmp}\n    CastorMeshConverter  ${MshConv}" )
			endif ()
			if( CASTOR_BUILD_TOOL_CASTOR_TEST_LAUNCHER )
				set( msg_tmp "${msg_tmp}\n    CastorTestLauncher   ${TestLcr}" )
			endif ()
		endif ()
		set( msg "${msg}${msg_tmp}" )
		message( STATUS "${msg}\n" )
	endif ()
endfunction( ToolsSummary )

function( ToolsSetup )
	if( CASTOR_BUILDGRP_TOOL )
		if( CASTOR_BUILD_TOOL_IMG_CONVERTER )
			cpack_add_component( ImgConverter
				DISPLAY_NAME "ImgConverter application"
				DESCRIPTION "A small image converter, to convert from any image format to ICO or XPM."
				GROUP Tools
				INSTALL_TYPES Full Developer
			)
		endif()

		if( CASTOR_BUILD_TOOL_HGT_MAP_TO_NML_MAP )
			cpack_add_component( HeightMapToNormalMap
				DISPLAY_NAME "HeightMapToNormalMap application"
				DESCRIPTION "Application used to generate a normal map from a height/bump map."
				GROUP Tools
				INSTALL_TYPES Full Developer
			)
		endif()

		if( CASTOR_BUILD_CASTOR3D )
			if( CASTOR_BUILD_TOOL_GUICOMMON )
				cpack_add_component( GuiCommon
					DISPLAY_NAME "GuiCommon"
					DESCRIPTION "Common classes and functions for CastorViewer and CastorShape."
					GROUP Tools
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

				if( CASTOR_BUILD_TOOL_CASTOR_VIEWER )
					cpack_add_component( CastorViewer
						DISPLAY_NAME "CastorViewer"
						DESCRIPTION "A simple scene viewer using Castor3D."
						GROUP Tools
						DEPENDS GuiCommon
						INSTALL_TYPES Full
					)
				endif ()
			endif ()

			if( CASTOR_BUILD_TOOL_MESH_UPGRADER )
				cpack_add_component( CastorMeshUpgrader
					DISPLAY_NAME "CastorMeshUpgrader application"
					DESCRIPTION "A mesh upgrader, to convert Castor3D mesh to the latest version."
					GROUP Tools
					INSTALL_TYPES Minimal Full Developer
				)
			endif()

			if( CASTOR_BUILD_TOOL_MESH_CONVERTER )
				cpack_add_component( CastorMeshConverter
					DISPLAY_NAME "CastorMeshConverter application"
					DESCRIPTION "A mesh converter, to convert from any mesh format to Castor3D mesh."
					GROUP Tools
					INSTALL_TYPES Minimal Full Developer
				)
			endif()

			if( CASTOR_BUILD_TOOL_CASTOR_TEST_LAUNCHER )
				cpack_add_component( CastorTestLauncher
					DISPLAY_NAME "CastorTestLauncher application"
					DESCRIPTION "An application used to run Castor3D render tests."
					GROUP Tools
					INSTALL_TYPES Full Developer
				)
			endif()
		endif()
	endif ()
endfunction( ToolsSetup )

if( ${CASTOR_BUILDGRP_TOOL} )
	ToolsInit()
	ToolsBuild()
	ToolsSummary()
endif()
