if ( MSVC )
	project( CastorViewerSharp )

	set( CVS_VERSION_MAJOR 0 )
	set( CVS_VERSION_MINOR 7 )
	set( CVS_VERSION_BUILD 0 )

	configure_file(
		${CMAKE_CURRENT_SOURCE_DIR}/Src/CastorViewerSharp.csproj.in
		${CMAKE_CURRENT_SOURCE_DIR}/Src/CastorViewerSharp.csproj
		@ONLY
	)

	include_external_msproject(
		CastorViewerSharp 
		${CMAKE_CURRENT_SOURCE_DIR}/Src/CastorViewerSharp.csproj
		TYPE FAE04EC0-301F-11D3-BF4B-00C04F79EFBC
	)
	set( Build "yes (version ${CVS_VERSION_MAJOR}.${CVS_VERSION_MINOR}.${CVS_VERSION_BUILD})" PARENT_SCOPE )
	set_property( TARGET CastorViewerSharp PROPERTY FOLDER "Samples" )
endif ()