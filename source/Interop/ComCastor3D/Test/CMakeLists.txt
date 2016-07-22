if ( MSVC )
	project( ComCastor3DTest )

	set( ${PROJECT_NAME}_VERSION_MAJOR	0 )
	set( ${PROJECT_NAME}_VERSION_MINOR	9 )
	set( ${PROJECT_NAME}_VERSION_BUILD	0 )

	include_directories( ${CMAKE_CURRENT_BINARY_DIR} )
	include_directories( ${CMAKE_CURRENT_BINARY_DIR}/Src )
	include_directories( ${CMAKE_CURRENT_BINARY_DIR}/Src/Win32 )

	add_target( 
		ComCastor3DTest
		bin_dos
		"ComCastor3D"
		"ComCastor3D"
		"ComCastor3DTestPch.hpp"
		"ComCastor3DTestPch.cpp"
	)

	# Copy tlb files into Test directory.
	add_custom_command( TARGET ${PROJECT_NAME}
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_BINARY_DIR}/../Castor3D.tlb ${CMAKE_CURRENT_BINARY_DIR}/Src/
		VERBATIM
	)

	set_property( TARGET ComCastor3DTest PROPERTY FOLDER "Test" )
	add_target_astyle( ComCastor3DTest ".hpp;.inl;.cpp" )
	set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
endif ()
