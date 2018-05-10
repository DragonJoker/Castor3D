option( CASTOR_BUILDGRP_TOOL "Build Castoro3D tools." FALSE )
option( CASTOR_BUILD_TOOL_IMG_CONVERTER "Build ImgConverter (needs wxWidgets library)" TRUE )
option( CASTOR_BUILD_TOOL_DIFF_IMAGE "Build DiffImage" TRUE )
option( CASTOR_BUILD_TOOL_MESH_UPGRADER "Build CastorMeshUpgrader" TRUE )
option( CASTOR_BUILD_TOOL_MESH_CONVERTER "Build CastorMeshConverter" TRUE )

function( ToolsInit )
	set( ImgConv "no (Not wanted)" PARENT_SCOPE )
	set( DiffImg "no (Not wanted)" PARENT_SCOPE )
	set( MshUpgd "no (Not wanted)" PARENT_SCOPE )
	set( MshConv "no (Not wanted)" PARENT_SCOPE )
endfunction( ToolsInit )

function( ToolsBuild )
	if( ${CASTOR_BUILDGRP_TOOL} )
		if( ${CASTOR_BUILD_TOOL_IMG_CONVERTER} )
			if( NOT WIN32 )
				find_package( GTK2 )
				include_directories( ${GTK2_INCLUDE_DIRS} )
			else()
				set( GTK2_FOUND TRUE )
			endif()

			#we look for wxWidgets
			find_package( wxWidgets COMPONENTS core base )

			if( wxWidgets_FOUND AND GTK2_FOUND )
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

				set( Build ${ImgConv} )
				add_subdirectory( Tools/ImgConverter )
				set( CPACK_PACKAGE_EXECUTABLES
					${CPACK_PACKAGE_EXECUTABLES}
					ImgConverter "ImgConverter"
					PARENT_SCOPE )
				set( ImgConv ${Build} PARENT_SCOPE )
			else()
				if( NOT wxWidgets_FOUND )
					set( ImgConv "no (wxWidgets libraries not found)" PARENT_SCOPE )
				else()
					set( ImgConv "no (GTK2 libraries not found)" PARENT_SCOPE )
				endif()
			endif()
		endif()

		if( ${CASTOR_BUILD_TOOL_DIFF_IMAGE} )
			set( Build ${DiffImg} )
			add_subdirectory( Tools/DiffImage )
			set( CPACK_PACKAGE_EXECUTABLES
				${CPACK_PACKAGE_EXECUTABLES}
				DiffImage "DiffImage"
				PARENT_SCOPE )
			set( DiffImg ${Build} PARENT_SCOPE )
		endif()

		if( ${CASTOR_BUILD_TOOL_MESH_UPGRADER} )
			set( Build ${MshUpgd} )
			add_subdirectory( Tools/CastorMeshUpgrader )
			set( CPACK_PACKAGE_EXECUTABLES
				${CPACK_PACKAGE_EXECUTABLES}
				CastorMeshUpgrader "CastorMeshUpgrader"
				PARENT_SCOPE )
			set( MshUpgd ${Build} PARENT_SCOPE )
		endif()

		if( ${CASTOR_BUILD_TOOL_MESH_CONVERTER} )
			set( Build ${MshConv} )
			add_subdirectory( Tools/CastorMeshConverter )
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
		set( msg_tmp "\n\nTools :" )
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

		if( ${CASTOR_BUILD_TOOL_TESTING} )
			cpack_add_component( CastorUtilsTest
				DISPLAY_NAME "CastorUtilsTest application"
				DESCRIPTION "A test application, to test CastorUtils classes and functions."
				GROUP Tools
				DEPENDS CastorUtils CastorUtils_dev
				INSTALL_TYPES Full Developer
			)
		endif()
	endif ()
endfunction( ToolsSetup )
