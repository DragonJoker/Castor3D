option( CASTOR_BUILD_GENERIC_CASTORGUI "Build CastorGui generic plugin" TRUE )

function( GenericInit )
	set( Cgui "no (Not wanted)" PARENT_SCOPE )
endfunction( GenericInit )

function( GenericBuild )
	if( ${CASTOR_BUILD_GENERIC_CASTORGUI} )
		set( Build ${Cgui} )
		add_subdirectory( Plugins/Generic/CastorGui )
		set( Cgui ${Build} PARENT_SCOPE )
	endif()

	set( CastorMinLibraries
		${CastorMinLibraries}
		PARENT_SCOPE
	)

	set( CastorBinsDependencies
		${CastorBinsDependencies}
		PARENT_SCOPE
	)
endfunction( GenericBuild )

function( GenericSummary msg )
	SET( msg_tmp "\n\nGeneric :" )
	SET( msg_tmp "${msg_tmp}\n    CastorGui           ${Cgui}" )
	SET( msg "${msg}${msg_tmp}" PARENT_SCOPE )
endfunction( GenericSummary )

function( GenericSetup )
	cpack_add_component_group( Generic
		DESCRIPTION "Generic plugins usable in Castor3D"
	)

	if( ${CASTOR_BUILD_POSTFX_SSAO} )
		cpack_add_component( CastorGui
			DISPLAY_NAME "CastorGui plugin"
			DESCRIPTION "Allows GUI creation using Castor3D"
			GROUP Generic
			DEPENDS Castor3D
			INSTALL_TYPES Full
		)
	endif()
endfunction( GenericSetup )
