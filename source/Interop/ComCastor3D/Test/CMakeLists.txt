if ( MSVC )
    project( ComCastor3DTest )

	set( CMCT_VERSION_MAJOR 0 )
	set( CMCT_VERSION_MINOR 7 )
	set( CMCT_VERSION_BUILD 0 )

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

	set_property( TARGET ComCastor3DTest PROPERTY FOLDER "Interop/ComCastor3D" )
	add_target_astyle( ComCastor3DTest ".hpp;.inl;.cpp" )
	set( Build "yes (version ${CMCT_VERSION_MAJOR}.${CMCT_VERSION_MINOR}.${CMCT_VERSION_BUILD})" PARENT_SCOPE )
endif ()
