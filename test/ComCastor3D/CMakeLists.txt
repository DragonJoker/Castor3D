if ( MSVC )
	if ( TARGET ComCastor3D )
		project( ComCastor3DTest )

		set( ${PROJECT_NAME}_DESCRIPTION "${MAIN_PROJECT_NAME} COM binding test application" )
		set( ${PROJECT_NAME}_VERSION_MAJOR 0 )
		set( ${PROJECT_NAME}_VERSION_MINOR 3 )
		set( ${PROJECT_NAME}_VERSION_BUILD 0 )
		set( PROJECT_VERSION "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}" )

		set( ${PROJECT_NAME}_HDR_FILES
			${CASTOR_SOURCE_DIR}/test/ComCastor3D/ComCastor3DTestPrerequisites.hpp
			${CASTOR_SOURCE_DIR}/test/ComCastor3D/ComCastor3DTestPch.hpp
		)
		set( ${PROJECT_NAME}_SRC_FILES
			${CASTOR_SOURCE_DIR}/test/ComCastor3D/ComCastor3DTest.cpp
			${CASTOR_SOURCE_DIR}/test/ComCastor3D/ComCastor3DTestPch.cpp
		)
		source_group( "Header Files"
			FILES
				${${PROJECT_NAME}_HDR_FILES}
		)
		source_group( "Source Files"
			FILES
				${${PROJECT_NAME}_SRC_FILES}
		)
		set( PROJECTS_UNITY_BUILD OFF )
		set( PROJECTS_USE_PRECOMPILED_HEADERS OFF )
		add_target_min(
			${PROJECT_NAME}
			bin_dos
		)
		target_include_directories( ${PROJECT_NAME}
			PRIVATE
				${Castor3DIncludeDirs}
				${CASTOR_SOURCE_DIR}/test/ComCastor3D
				${CASTOR_SOURCE_DIR}/test/ComCastor3D
				${CASTOR_BINARY_DIR}/include/Interop
				${CASTOR_BINARY_DIR}/source/Interop
		)
		target_link_libraries( ${PROJECT_NAME}
			PRIVATE
				castor::ComCastor3D
		)

		# Copy tlb files into Test directory.
		add_custom_command( TARGET ${PROJECT_NAME}
			PRE_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CASTOR_BINARY_DIR}/source/Interop/ComCastor3D/Castor3D.tlb ${CMAKE_CURRENT_BINARY_DIR}
			VERBATIM
		)
		set_target_properties( ${PROJECT_NAME}
			PROPERTIES
				VERSION ${PROJECT_VERSION}
				CXX_STANDARD 20
				CXX_EXTENSIONS OFF
				FOLDER "Tests/Castor"
		)
		add_target_astyle( ${PROJECT_NAME} ".hpp;.inl;.cpp" )
		set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
	else ()
		set( Build "no  (missing ComCastor3D)" PARENT_SCOPE )
	endif ()
endif ()
