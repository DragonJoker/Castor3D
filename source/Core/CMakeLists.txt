function( CoreInit )
	set( Cutils "no (Not wanted)" PARENT_SCOPE )
	set( C3D "no (Not wanted)" PARENT_SCOPE )
endfunction( CoreInit )

function( CoreBuild )
	set( Build ${Cutils} )
	add_subdirectory( Core/CastorUtils )
	set( Cutils ${Build} PARENT_SCOPE )

	set( Build ${C3D} )
	add_subdirectory( Core/Castor3D )
	set( C3D ${Build} PARENT_SCOPE )

	set( CastorMinLibraries
		${CastorMinLibraries}
		PARENT_SCOPE
	)

	set( CastorBinsDependencies
		${CastorBinsDependencies}
		PARENT_SCOPE
	)
endfunction( CoreBuild )

function( CoreSummary msg )
	set( msg_tmp "\nCore :" )
	set( msg_tmp "${msg_tmp}\n    CastorUtils         ${Cutils}" )
	set( msg_tmp "${msg_tmp}\n    Castor3D            ${C3D}" )
	SET( msg "${msg}${msg_tmp}" PARENT_SCOPE )
endfunction( CoreSummary )