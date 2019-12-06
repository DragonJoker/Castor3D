get_filename_component( CASTOR_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY )
get_filename_component( CASTOR_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR} DIRECTORY )

option( CASTOR_BUILDGRP_TOOL "Build Castoro3D tools." FALSE )
option( CASTOR_BUILD_TOOL_IMG_CONVERTER "Build ImgConverter (needs wxWidgets library)" TRUE )
option( CASTOR_BUILD_TOOL_DIFF_IMAGE "Build DiffImage" TRUE )
option( CASTOR_BUILD_TOOL_MESH_UPGRADER "Build CastorMeshUpgrader" TRUE )
option( CASTOR_BUILD_TOOL_MESH_CONVERTER "Build CastorMeshConverter" TRUE )
option( CASTOR_BUILD_TOOL_CASTOR_TEST_LAUNCHER "Build CastorTestLauncher" TRUE )

function( ToolsInit )
	set( ImgConv "no (Not wanted)" PARENT_SCOPE )
	set( DiffImg "no (Not wanted)" PARENT_SCOPE )
	set( MshUpgd "no (Not wanted)" PARENT_SCOPE )
	set( MshConv "no (Not wanted)" PARENT_SCOPE )
	set( TestLcr "no (Not wanted)" PARENT_SCOPE )
endfunction( ToolsInit )

function( ToolsBuild )
	if( ${CASTOR_BUILDGRP_TOOL} )
		if( ${CASTOR_BUILD_TOOL_IMG_CONVERTER}
			OR ${CASTOR_BUILD_TOOL_CASTOR_TEST_LAUNCHER} )
			if( APPLE )
				set( GTK2_FOUND TRUE )
				set( GTK2_LIBRARIES
					"-framework AppKit"
					"-framework QuartzCore"
				)				
			elseif( WIN32 )
				set( GTK2_FOUND TRUE )
			else()
				find_package( GTK2 )
			endif()

			#we look for wxWidgets
			find_package( wxWidgets COMPONENTS core base )

			if( wxWidgets_FOUND AND GTK2_FOUND )
				set( Castor3DIncludeDirs
					${Castor3DIncludeDirs}
					${CASTOR_BINARY_DIR}/include/Core
					${CASTOR_SOURCE_DIR}/include/Core
				)
				if ( APPLE )
					set( Castor3DDefinitions
						${Castor3DDefinitions}
						VK_USE_PLATFORM_MACOS_MVK
					)
				elseif ( WIN32 )
					set( Castor3DDefinitions
						${Castor3DDefinitions}
						VK_USE_PLATFORM_WIN32_KHR
					)
				elseif ( ANDROID )
					set( Castor3DDefinitions
						${Castor3DDefinitions}
						VK_USE_PLATFORM_ANDROID_KHR
					)
				else ()
					set( Castor3DDefinitions
						${Castor3DDefinitions}
						VK_USE_PLATFORM_XLIB_KHR
					)
				endif ()


				if( ${CASTOR_BUILD_TOOL_IMG_CONVERTER} )
					set( Build ${ImgConv} )
					add_subdirectory( ImgConverter )
					set( CPACK_PACKAGE_EXECUTABLES
						${CPACK_PACKAGE_EXECUTABLES}
						ImgConverter "ImgConverter"
						PARENT_SCOPE )
					set( ImgConv ${Build} PARENT_SCOPE )
				endif ()

				if( ${CASTOR_BUILD_TOOL_CASTOR_TEST_LAUNCHER} )
					set( Build ${TestLcr} )
					add_subdirectory( CastorTestLauncher )
					set( CPACK_PACKAGE_EXECUTABLES
						${CPACK_PACKAGE_EXECUTABLES}
						CastorTestLauncher "CastorTestLauncher"
						PARENT_SCOPE )
					set( TestLcr ${Build} PARENT_SCOPE )
				endif ()
			else()
				if( NOT wxWidgets_FOUND )
					set( ImgConv "no (wxWidgets libraries not found)" PARENT_SCOPE )
					set( TestLcr "no (wxWidgets libraries not found)" PARENT_SCOPE )
				else()
					set( ImgConv "no (GTK2 libraries not found)" PARENT_SCOPE )
					set( TestLcr "no (GTK2 libraries not found)" PARENT_SCOPE )
				endif()
			endif()
		endif()

		if( ${CASTOR_BUILD_TOOL_DIFF_IMAGE} )
			set( Build ${DiffImg} )
			add_subdirectory( DiffImage )
			set( CPACK_PACKAGE_EXECUTABLES
				${CPACK_PACKAGE_EXECUTABLES}
				DiffImage "DiffImage"
				PARENT_SCOPE )
			set( DiffImg ${Build} PARENT_SCOPE )
		endif()

		if( ${CASTOR_BUILD_TOOL_MESH_UPGRADER} )
			set( Build ${MshUpgd} )
			add_subdirectory( CastorMeshUpgrader )
			set( CPACK_PACKAGE_EXECUTABLES
				${CPACK_PACKAGE_EXECUTABLES}
				CastorMeshUpgrader "CastorMeshUpgrader"
				PARENT_SCOPE )
			set( MshUpgd ${Build} PARENT_SCOPE )
		endif()

		if( ${CASTOR_BUILD_TOOL_MESH_CONVERTER} )
			set( Build ${MshConv} )
			add_subdirectory( CastorMeshConverter )
			set( CPACK_PACKAGE_EXECUTABLES
				${CPACK_PACKAGE_EXECUTABLES}
				CastorMeshConverter "CastorMeshConverter"
				PARENT_SCOPE )
			set( MshConv ${Build} PARENT_SCOPE )
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

function( ToolsSummary msg )
	if( ${CASTOR_BUILDGRP_TOOL} )
		if( ${CASTOR_BUILD_TOOL_IMG_CONVERTER} )
			set( msg_tmp "${msg_tmp}\n    ImgConverter         ${ImgConv}" )
		endif ()
		if( ${CASTOR_BUILD_TOOL_DIFF_IMAGE} )
			set( msg_tmp "${msg_tmp}\n    DiffImage            ${DiffImg}" )
		endif ()
		if( ${CASTOR_BUILD_TOOL_MESH_UPGRADER} )
			set( msg_tmp "${msg_tmp}\n    CastorMeshUpgrader   ${MshUpgd}" )
		endif ()
		if( ${CASTOR_BUILD_TOOL_MESH_CONVERTER} )
			set( msg_tmp "${msg_tmp}\n    CastorMeshConverter  ${MshConv}" )
		endif ()
		if( ${CASTOR_BUILD_TOOL_CASTOR_TEST_LAUNCHER} )
			set( msg_tmp "${msg_tmp}\n    CastorTestLauncher   ${TestLcr}" )
		endif ()
		set( msg "${msg}${msg_tmp}" PARENT_SCOPE )
	endif ()
endfunction( ToolsSummary )

function( ToolsSetup )
	if( ${CASTOR_BUILDGRP_TOOL} )
		if( ${CASTOR_BUILD_TOOL_IMG_CONVERTER} )
			cpack_add_component( ImgConverter
				DISPLAY_NAME "ImgConverter application"
				DESCRIPTION "A small image converter, to convert from any image format to ICO or XPM."
				GROUP Tools
				INSTALL_TYPES Full
			)
		endif()

		if( ${CASTOR_BUILD_TOOL_MESH_UPGRADER} )
			cpack_add_component( CastorMeshUpgrader
				DISPLAY_NAME "CastorMeshUpgrader application"
				DESCRIPTION "A mesh upgrader, to convert Castor3D mesh to the latest version."
				GROUP Tools
				INSTALL_TYPES Full
			)
		endif()

		if( ${CASTOR_BUILD_TOOL_MESH_CONVERTER} )
			cpack_add_component( CastorMeshConverter
				DISPLAY_NAME "CastorMeshConverter application"
				DESCRIPTION "A mesh converter, to convert from any mesh format to Castor3D mesh."
				GROUP Tools
				INSTALL_TYPES Full
			)
		endif()

		if( ${CASTOR_BUILD_TOOL_CASTOR_TEST_LAUNCHER} )
			cpack_add_component( CastorTestLauncher
				DISPLAY_NAME "CastorTestLauncher application"
				DESCRIPTION "An application used to run Castor3D render tests."
				GROUP Tools
				INSTALL_TYPES Full Developer
			)
		endif()
	endif ()
endfunction( ToolsSetup )

if( ${CASTOR_BUILDGRP_TOOL} )
	ToolsInit()
	ToolsBuild()
	set( msg "\n\nFollowing tool apps will be built" )
	ToolsSummary( ${msg} )
	message( STATUS "${msg}\n" )
endif()
