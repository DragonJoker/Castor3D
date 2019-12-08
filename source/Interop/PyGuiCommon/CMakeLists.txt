project( PyGuiCommon )

set( ${PROJECT_NAME}_DESCRIPTION "GuiCommon Python binding DLL." )
set( ${PROJECT_NAME}_VERSION_MAJOR 0 )
set( ${PROJECT_NAME}_VERSION_MINOR 11 )
set( ${PROJECT_NAME}_VERSION_BUILD 0 )

#we look for wxWidgets
if (WIN32)
	find_package( wxWidgets COMPONENTS stc scintilla core base aui )
	set( GTK2_FOUND TRUE )
elseif (APPLE)
	find_package( wxWidgets COMPONENTS stc scintilla core base aui )
	set( GTK2_FOUND TRUE )
else ()
	find_package( wxWidgets COMPONENTS core base stc aui )
	find_package( GTK2 )
endif()

if ( wxWidgets_FOUND AND GTK2_FOUND )
	add_target( 
		${PROJECT_NAME}
		dll
		${CASTOR_SOURCE_DIR}/include/Interop/${PROJECT_NAME}
		${CASTOR_SOURCE_DIR}/source/Interop/${PROJECT_NAME}
	)
	add_library( castor::${PROJECT_NAME}
		ALIAS
		${PROJECT_NAME}
	)
	target_include_directories( ${PROJECT_NAME}
		PRIVATE
			${Castor3DIncludeDirs}
			${CASTOR_SOURCE_DIR}/samples
			${GTK2_INCLUDE_DIRS} )
			${wxWidgets_INCLUDE_DIR} )
	)
	target_link_libraries( ${PROJECT_NAME}
		PRIVATE
			${wxWidgets_LIBRARIES}
			${GTK2_LIBRARIES}
			castor::PyCastor3D
			castor::GuiCommon
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
