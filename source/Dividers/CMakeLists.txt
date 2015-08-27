option( CASTOR_BUILD_DIVIDER_LOOP "Build Loop Divider plugin" TRUE )
option( CASTOR_BUILD_DIVIDER_PNTRIANGLES "Build PN-Triangles Divider plugin" TRUE )

function( DividersInit )
	set( PNTri "no (Not wanted)" PARENT_SCOPE )
	set( Loop "no (Not wanted)" PARENT_SCOPE )
endfunction( DividersInit )

function( DividersBuild )
	if( ${CASTOR_BUILD_DIVIDER_LOOP} )
		set( Build ${PNTri} )
		add_subdirectory( Dividers/LoopDivider )
		set( PNTri ${Build} PARENT_SCOPE )
	endif()
	
	if( ${CASTOR_BUILD_DIVIDER_PNTRIANGLES} )
		set( Build ${Loop} )
		add_subdirectory( Dividers/PnTrianglesDivider )
		set( Loop ${Build} PARENT_SCOPE )
	endif()

	set( CastorMinLibraries
		${CastorMinLibraries}
		PARENT_SCOPE
	)

	set( CastorBinsDependencies
		${CastorBinsDependencies}
		PARENT_SCOPE
	)
endfunction( DividersBuild )

function( DividersSummary msg )
	set( msg_tmp "\n\nSubdividers :" )
	set( msg_tmp "${msg_tmp}\n    PN-Triangles        ${PNTri}" )
	set( msg_tmp "${msg_tmp}\n    Loop                ${Loop}" )
	SET( msg "${msg}${msg_tmp}" PARENT_SCOPE )
endfunction( DividersSummary )