option( CASTOR_BUILD_POSTFX_SSAO "Build SSAO PostEffect plugin" TRUE )

function( PostEffectsInit )
	set( Ssao "no (Not wanted)" PARENT_SCOPE )
endfunction( PostEffectsInit )

function( PostEffectsBuild )
	if( ${CASTOR_BUILD_IMPORTER_ASSIMP} )
		set( Build ${Ssao} )
		add_subdirectory( PostEffects/SsaoPostEffect )
		set( Ssao ${Build} PARENT_SCOPE )
	endif()

	set( CastorMinLibraries
		${CastorMinLibraries}
		PARENT_SCOPE
	)

	set( CastorBinsDependencies
		${CastorBinsDependencies}
		PARENT_SCOPE
	)
endfunction( PostEffectsBuild )

function( PostEffectsSummary msg )
	SET( msg_tmp "\n\nPostEffects :" )
	SET( msg_tmp "${msg_tmp}\n    SSAO              ${Ssao}" )
	SET( msg "${msg}${msg_tmp}" PARENT_SCOPE )
endfunction( PostEffectsSummary )
