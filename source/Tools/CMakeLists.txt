option( CASTOR_BUILDGRP_TOOL "Build Castoro3D tools." FALSE )
option( CASTOR_BUILD_TOOL_IMG_CONVERTER "Build ImgConverter (needs wxWidgets library)" TRUE )

function( ToolsInit )
	set( ImgConv "no (Not wanted)" PARENT_SCOPE )
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
				set( ImgConv ${Build} PARENT_SCOPE )
			else()
				if( NOT wxWidgets_FOUND )
					set( ImgConv "no (wxWidgets libraries not found)" PARENT_SCOPE )
				else()
					set( ImgConv "no (GTK2 libraries not found)" PARENT_SCOPE )
				endif()
			endif()
		endif()

		set( CastorMinLibraries
			${CastorMinLibraries}
			PARENT_SCOPE
		)

		set( CastorBinsDependencies
			${CastorBinsDependencies}
			PARENT_SCOPE
		)
	endif ()
endfunction( ToolsBuild )

function( ToolsSummary msg )
	set( msg_tmp "\n\nTools :" )
	set( msg_tmp "${msg_tmp}\n    ImgConverter        ${ImgConv}" )
	SET( msg "${msg}${msg_tmp}" PARENT_SCOPE )
endfunction( ToolsSummary )

function( ToolsSetup )
	if( ${CASTOR_BUILDGRP_TOOL} )
		cpack_add_component_group( Tools
			EXPANDED
			DESCRIPTION "Some tools"
		)

		if( ${CASTOR_BUILD_TOOL_IMG_CONVERTER} )
			cpack_add_component( ImgConverter
				DISPLAY_NAME "ImgConverter application"
				DESCRIPTION "A small image converter, to convert from any image format to ICO or XPM"
				GROUP Tools
				INSTALL_TYPES Full
			)
		endif()

		if( ${CASTOR_BUILD_TOOL_TESTING} )
			cpack_add_component( CastorUtilsTest
				DISPLAY_NAME "CastorUtilsTest application"
				DESCRIPTION "A test application, to test CastorUtils classes and functions"
				GROUP Tools
				DEPENDS CastorUtils CastorUtils_dev
				INSTALL_TYPES Full Developer
			)
		endif()
	endif ()
endfunction( ToolsSetup )
