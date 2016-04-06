if ( MSVC AND ${CASTOR_BUILD_INTEROP_COM} )
	project( CastorViewerSharp )

	set( CVS_VERSION_MAJOR 0 )
	set( CVS_VERSION_MINOR 7 )
	set( CVS_VERSION_BUILD 0 )

	configure_file(
		${CMAKE_CURRENT_SOURCE_DIR}/Src/${PROJECT_NAME}.csproj.in
		${CMAKE_CURRENT_SOURCE_DIR}/Src/${PROJECT_NAME}.csproj
		@ONLY
	)

	include_external_msproject(
		${PROJECT_NAME}
		${CMAKE_CURRENT_SOURCE_DIR}/Src/${PROJECT_NAME}.csproj
		TYPE FAE04EC0-301F-11D3-BF4B-00C04F79EFBC
	)
	add_dependencies( ${PROJECT_NAME}
		ComCastor3D
	)
	set( Build "yes (version ${CVS_VERSION_MAJOR}.${CVS_VERSION_MINOR}.${CVS_VERSION_BUILD})" PARENT_SCOPE )
	set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Samples" )
endif ()