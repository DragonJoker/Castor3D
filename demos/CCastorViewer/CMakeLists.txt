if ( VCPKG_TOOLCHAIN )
	set( glfw3_DIR ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/share/glfw3 )
	set( tinyfiledialogs_DIR ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/share/tinyfiledialogs )
	find_package( glfw3 CONFIG )
	find_package( tinyfiledialogs CONFIG )
else ()
	find_package( glfw3 )
	find_package( tinyfiledialogs )
endif ()

if ( glfw3_FOUND AND tinyfiledialogs_FOUND )
	project( CCastorViewer )

	set( ${PROJECT_NAME}_DESCRIPTION "C CastorViewer" )
	set( ${PROJECT_NAME}_VERSION_MAJOR 0 )
	set( ${PROJECT_NAME}_VERSION_MINOR 1 )
	set( ${PROJECT_NAME}_VERSION_BUILD 0 )

	set( ${PROJECT_NAME}_HDR_FILES
		${CASTOR_SOURCE_DIR}/demos/${PROJECT_NAME}/MainWindow.h
		${CASTOR_SOURCE_DIR}/demos/${PROJECT_NAME}/NodeState.h
	)
	set( ${PROJECT_NAME}_SRC_FILES
		${CASTOR_SOURCE_DIR}/demos/${PROJECT_NAME}/App.c
		${CASTOR_SOURCE_DIR}/demos/${PROJECT_NAME}/MainWindow.c
		${CASTOR_SOURCE_DIR}/demos/${PROJECT_NAME}/NodeState.c
	)
	source_group( "Header Files"
		FILES
			${${PROJECT_NAME}_HDR_FILES}
	)
	source_group( "Source Files"
		FILES
			${${PROJECT_NAME}_SRC_FILES}
	)
	add_target_min(
		${PROJECT_NAME}
		bin_dos
	)
	if ( CMAKE_CXX_COMPILER_ID MATCHES "MSVC" )
		target_compile_options( ${PROJECT_NAME} PRIVATE "$<$<CONFIG:Release>:/Zi>" )
		target_link_options( ${PROJECT_NAME} PRIVATE "$<$<CONFIG:Release>:/DEBUG>" )
		target_link_options( ${PROJECT_NAME} PRIVATE "$<$<CONFIG:Release>:/OPT:REF>" )
		target_link_options( ${PROJECT_NAME} PRIVATE "$<$<CONFIG:Release>:/OPT:ICF>" )
	endif ()
	target_link_libraries( ${PROJECT_NAME}
		PRIVATE
			castor::CCastor3D
			glfw
			tinyfiledialogs::tinyfiledialogs
	)
	set_target_properties( ${PROJECT_NAME}
		PROPERTIES
			FOLDER "Demos"
	)
	install_target_ex( ${PROJECT_NAME}
		Castor3D
		${PROJECT_NAME}
		bin_dos
		${CASTOR_SOURCE_DIR}/demos/${PROJECT_NAME}
	)
	set( CASTOR_INSTEXP_LIST
		${CASTOR_INSTEXP_LIST}
		${PROJECT_NAME}
		PARENT_SCOPE
	)
	set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
	add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
elseif ( NOT glfw3_FOUND )
	set( Build "no (missing GLFW)" PARENT_SCOPE )
elseif ( NOT tinyfiledialogs_FOUND )
	set( Build "no (missing tinyfiledialogs)" PARENT_SCOPE )
endif ()
