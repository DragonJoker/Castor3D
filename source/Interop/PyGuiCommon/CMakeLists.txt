project( PyGuiCommon )

set( ${PROJECT_NAME}_DESCRIPTION "GuiCommon Python binding DLL." )
set( ${PROJECT_NAME}_VERSION_MAJOR 0 )
set( ${PROJECT_NAME}_VERSION_MINOR 11 )
set( ${PROJECT_NAME}_VERSION_BUILD 0 )

#we look for wxWidgets
if (WIN32)
	find_package( wxWidgets COMPONENTS stc scintilla core base aui )
	set( GTK2_FOUND TRUE )
else ()
	find_package( wxWidgets COMPONENTS core base stc aui )
	find_package( GTK2 )
endif()

if ( wxWidgets_FOUND AND GTK2_FOUND )
	include_directories( ${Castor3DIncludeDirs} )
	include_directories( ${CMAKE_SOURCE_DIR}/Samples/GuiCommon/Src )
	include_directories( ${Boost_INCLUDE_DIR} )
	include_directories( ${PYTHON_INCLUDE_DIR} )
	include_directories( ${GTK2_INCLUDE_DIRS} )
	include_directories( ${wxWidgets_INCLUDE_DIR} )

	link_directories( ${Boost_LIBRARY_DIR} )
	link_directories( ${Boost_LIBRARY_DIR_DEBUG} )
	link_directories( ${Boost_LIBRARY_DIR_RELEASE} )

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
	add_definitions(
		-D_FILE_OFFSET_BITS=64
		-D_LARGE_FILES
	)

	if( NOT WIN32 )
		add_definitions(
			-D__WXGTK__
		)
	endif()

	if( IsWxDebug )
		add_definitions(
			-D__WXDEBUG__
		)
	endif()
	
	link_directories( ${Boost_LIBRARY_DIR} )
	#link_directories( ${Boost_LIBRARY_DIR_RELEASE} )
	#link_directories( ${Boost_LIBRARY_DIR_DEBUG} )

	add_target( 
		${PROJECT_NAME}
		dll
		"CastorUtils;Castor3D;GuiCommon"
		"CastorUtils;Castor3D;GuiCommon;${PythonLibraries};${wxWidgetsLibraries}"
	)

	set_target_properties( ${PROJECT_NAME}
		PROPERTIES
			PREFIX ""
			POSTFIX ""
			LIBRARY_OUTPUT_NAME_DEBUG castor_wx
			RUNTIME_OUTPUT_NAME_DEBUG castor_wx
			ARCHIVE_OUTPUT_NAME_DEBUG castor_wx
			LIBRARY_OUTPUT_NAME_RELEASE castor_wx
			RUNTIME_OUTPUT_NAME_RELEASE castor_wx
			ARCHIVE_OUTPUT_NAME_RELEASE castor_wx
			FOLDER "Interop"
	)

	add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
	set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
else ()
	set( Build "no (missing Boost.Python library" PARENT_SCOPE )
endif ()
