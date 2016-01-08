option( CASTOR_BUILD_POSTFX_SSAO "Build SSAO PostEffect plugin" TRUE )
option( CASTOR_BUILD_POSTFX_BLOOM "Build Bloom PostEffect plugin" TRUE )

function( PostEffectsInit )
	set( Ssao "no (Not wanted)" PARENT_SCOPE )
	set( Bloom "no (Not wanted)" PARENT_SCOPE )
endfunction( PostEffectsInit )

function( PostEffectsBuild )
	set( PostEffectLibraries ${CastorMinLibraries} )

	if( ${CASTOR_BUILD_RENDERER_OGL} )
		set( PostEffectLibraries
			${PostEffectLibraries}
			${OpenGlLibraries}
			GlRenderSystem
		)
	endif ()

	if( ${CASTOR_BUILD_RENDERER_D3D11} )
		set( PostEffectLibraries
			${PostEffectLibraries}
			${D3dLibraries}
			Dx11RenderSystem
		)
	endif ()

	if( ${CASTOR_BUILD_POSTFX_SSAO} )
		set( Build ${Ssao} )
		add_subdirectory( Plugins/PostEffects/SsaoPostEffect )
		set( Ssao ${Build} PARENT_SCOPE )
	endif()

	if( ${CASTOR_BUILD_POSTFX_BLOOM} )
		set( Build ${Bloom} )
		add_subdirectory( Plugins/PostEffects/BloomPostEffect )
		set( Bloom ${Build} PARENT_SCOPE )
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
	SET( msg_tmp "${msg_tmp}\n    SSAO                ${Ssao}" )
	SET( msg_tmp "${msg_tmp}\n    Bloom               ${Bloom}" )
	SET( msg "${msg}${msg_tmp}" PARENT_SCOPE )
endfunction( PostEffectsSummary )

function( PostEffectsSetup )
	cpack_add_component_group( PostEffects
		DESCRIPTION "PostEffects plugins usable in Castor3D"
	)

	if( ${CASTOR_BUILD_POSTFX_SSAO} )
		cpack_add_component( SsaoPostEffect
			DISPLAY_NAME "SsaoPostEffect plugin"
			DESCRIPTION "Allows Screen Space Ambiant Occlusion post effect"
			GROUP PostEffects
			DEPENDS Castor3D
			INSTALL_TYPES Full
		)
	endif()

	if( ${CASTOR_BUILD_POSTFX_BLOOM} )
		cpack_add_component( BloomPostEffect
			DISPLAY_NAME "BloomPostEffect plugin"
			DESCRIPTION "Allows Bloom post effect"
			GROUP PostEffects
			DEPENDS Castor3D
			INSTALL_TYPES Full
		)
	endif()
endfunction( PostEffectsSetup )
