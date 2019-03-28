if ( MSVC )
	project( ComCastor3DTest )

	set( ${PROJECT_NAME}_VERSION_MAJOR	0 )
	set( ${PROJECT_NAME}_VERSION_MINOR 11 )
	set( ${PROJECT_NAME}_VERSION_BUILD	0 )

	set( ${PROJECT_NAME}_DESCRIPTION "${MAIN_PROJECT_NAME} COM binding test application" )
	set( ${PROJECT_NAME}_VERSION_MAJOR	0 )
	set( ${PROJECT_NAME}_VERSION_MINOR 11 )
	set( ${PROJECT_NAME}_VERSION_BUILD	0 )

	include_directories(
		${Castor3DIncludeDirs}
		${CASTOR_BINARY_DIR}/include/Interop
		${CASTOR_BINARY_DIR}/source/Interop
	)

	add_target( 
		ComCastor3DTest
		bin_dos
		${CASTOR_SOURCE_DIR}/test/ComCastor3D
		${CASTOR_SOURCE_DIR}/test/ComCastor3D
		"ComCastor3D"
		"ComCastor3D"
		${CASTOR_SOURCE_DIR}/test/ComCastor3D/${PROJECT_NAME}Pch.hpp
		${CASTOR_SOURCE_DIR}/test/ComCastor3D/${PROJECT_NAME}Pch.cpp
	)

	# Copy tlb files into Test directory.
	add_custom_command( TARGET ${PROJECT_NAME}
		PRE_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CASTOR_BINARY_DIR}/source/Interop/ComCastor3D/Castor3D.tlb ${CMAKE_CURRENT_BINARY_DIR}
		VERBATIM
	)

	set_property( TARGET ComCastor3DTest PROPERTY FOLDER "Tests/Castor" )
	add_target_astyle( ComCastor3DTest ".hpp;.inl;.cpp" )
	set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
endif ()
