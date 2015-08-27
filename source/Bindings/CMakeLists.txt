option( CASTOR_BUILD_BINDING_PYTHON "Build Python Castor3D binding" TRUE )
option( CASTOR_BUILD_BINDING_COM "Build COM Castor3D binding" TRUE )

function( BindingsInit )
	set( PyC3D "no (Not wanted)" PARENT_SCOPE )
	if ( MSVC )
		set( ComC3D "no (Not wanted)" PARENT_SCOPE )
		set( ComC3DTest "no (Not wanted)" PARENT_SCOPE )
	else ()
		set( ComC3D "no (Not available)" PARENT_SCOPE )
		set( ComC3DTest "no (Not available)" PARENT_SCOPE )
	endif ()
endfunction( BindingsInit )

function( BindingsBuild )
	if( ${CASTOR_BUILD_BINDING_PYTHON} )
		set( Build ${PyC3D} )
		add_subdirectory( Bindings/PyCastor3D )
		set( PyC3D ${Build} PARENT_SCOPE )
	endif()

	if( ${CASTOR_BUILD_BINDING_COM} )
		set( Build ${ComC3D} )
		add_subdirectory( Bindings/ComCastor3D )
		set( ComC3D ${Build} PARENT_SCOPE )
		set( Build ${ComC3DTest} )
		add_subdirectory( Bindings/ComCastor3D/Test )
		set( ComC3DTest ${Build} PARENT_SCOPE )
	endif()

	set( CastorMinLibraries
		${CastorMinLibraries}
		PARENT_SCOPE
	)

	set( CastorBinsDependencies
		${CastorBinsDependencies}
		PARENT_SCOPE
	)
endfunction( BindingsBuild )

function( BindingsSummary msg )
	set( msg_tmp "\n\nBindings :" )
	set( msg_tmp "${msg_tmp}\n    Python              ${PyC3D}" )
	set( msg_tmp "${msg_tmp}\n    COM                 ${ComC3D}" )
	set( msg_tmp "${msg_tmp}\n    COM Test            ${ComC3DTest}" )
	SET( msg "${msg}${msg_tmp}" PARENT_SCOPE )
endfunction( BindingsSummary )