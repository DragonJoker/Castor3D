option( CASTOR_BUILD_TECHNIQUE_DEFERRED "Build Deferred Lighting RenderTechnique plugin" TRUE )
option( CASTOR_BUILD_TECHNIQUE_MSAA "Build MSAA RenderTechnique plugin" TRUE )
option( CASTOR_BUILD_TECHNIQUE_SSAA "Build SSAA RenderTechnique plugin" TRUE )

function( TechniquesInit )
	set( Drct "no (Not wanted)" PARENT_SCOPE )
	set( Defr "no (Not wanted)" PARENT_SCOPE )
	set( Msaa "no (Not wanted)" PARENT_SCOPE )
	set( Ssaa "no (Not wanted)" PARENT_SCOPE )
endfunction( TechniquesInit )

function( TechniquesBuild )
	set( TechniqueLibraries ${CastorMinLibraries} )
	
	if( ${CASTOR_BUILD_RENDERER_OGL} )
		set( TechniqueLibraries
			${TechniqueLibraries}
			${OpenGlLibraries}
			GlRenderSystem
		)
	endif ()

	if( ${CASTOR_BUILD_RENDERER_D3D11} )
		set( TechniqueLibraries
			${TechniqueLibraries}
			${D3dLibraries}
			Dx11RenderSystem
		)
	endif ()
	
	set( Build ${Drct} )
	add_subdirectory( Techniques/DirectTechnique )
	set( Drct ${Build} PARENT_SCOPE )
	
	if( ${CASTOR_BUILD_TECHNIQUE_DEFERRED} )
		set( Build ${Defr} )
		add_subdirectory( Techniques/DeferredTechnique )
		set( Defr ${Build} PARENT_SCOPE )
	endif()
	
	if( ${CASTOR_BUILD_TECHNIQUE_MSAA} )
		set( Build ${Msaa} )
		add_subdirectory( Techniques/MsaaTechnique )
		set( Msaa ${Build} PARENT_SCOPE )
	endif()
	
	if( ${CASTOR_BUILD_TECHNIQUE_SSAA} )
		set( Build ${Ssaa} )
		add_subdirectory( Techniques/SsaaTechnique )
		set( Ssaa ${Build} PARENT_SCOPE )
	endif()

	set( CastorMinLibraries
		${CastorMinLibraries}
		PARENT_SCOPE
	)

	set( CastorBinsDependencies
		${CastorBinsDependencies}
		PARENT_SCOPE
	)
endfunction( TechniquesBuild )

function( TechniquesSummary msg )
	set( msg_tmp "\n\nRenderTechniques :" )
	set( msg_tmp "${msg_tmp}\n    Direct              ${Drct}" )
	set( msg_tmp "${msg_tmp}\n    Deferred Lighting   ${Defr}" )
	set( msg_tmp "${msg_tmp}\n    MSAA                ${Msaa}" )
	set( msg_tmp "${msg_tmp}\n    SSAA                ${Ssaa}" )
	SET( msg "${msg}${msg_tmp}" PARENT_SCOPE )
endfunction( TechniquesSummary )

function( TechniquesSetup )
	cpack_add_component_group( Techniques
		DESCRIPTION "RenderTechnique plugins usable in Castor3D"
	)

	if( ${CASTOR_BUILD_TECHNIQUE_DEFERRED} )
		cpack_add_component( DeferredTechnique
			DISPLAY_NAME "DeferredTechnique plugin"
			DESCRIPTION "Allows rendering with Deferred Lighting technique"
			GROUP Techniques
			DEPENDS Castor3D
			INSTALL_TYPES Full
		)
	endif()
	if( ${CASTOR_BUILD_TECHNIQUE_MSAA} )
		cpack_add_component( MsaaTechnique
			DISPLAY_NAME "MsaaTechnique plugin"
			DESCRIPTION "Allows rendering with MSAA technique"
			GROUP Techniques
			DEPENDS Castor3D
			INSTALL_TYPES Full
		)
	endif()
	if( ${CASTOR_BUILD_TECHNIQUE_SSAA} )
		cpack_add_component( SsaaTechnique
			DISPLAY_NAME "SsaaTechnique plugin"
			DESCRIPTION "Allows rendering with SSAA technique"
			GROUP Techniques
			DEPENDS Castor3D
			INSTALL_TYPES Full
		)
	endif()
endfunction( TechniquesSetup )
