option( CASTOR_BUILD_RENDERER_OGL "Build OpenGL Renderer plugin" TRUE )

if( WIN32 )
    option( CASTOR_BUILD_RENDERER_D3D11 "Build Direct3D11 Renderer plugin (needs Direct3D 11 SDK)" TRUE )
endif()

function( RenderersInit )
	set( Ogl "no (Not wanted)" PARENT_SCOPE )
	if ( WIN32 )
		set( D3d11 "no (Not wanted)" PARENT_SCOPE )
	else()
		set( D3d11 "no (Not available)" PARENT_SCOPE )
	endif()
endfunction( RenderersInit )

function( RenderersBuild )
	if( ${CASTOR_BUILD_RENDERER_OGL} )
		set( Build ${Ogl} )
		add_subdirectory( Renderers/GlRenderSystem )
		set( Ogl ${Build} PARENT_SCOPE )
	endif()

	if( WIN32 )
		if( ${CASTOR_BUILD_RENDERER_D3D11} )
			set( Build ${D3d11} )
			add_subdirectory( Renderers/Dx11RenderSystem )
			set( D3d11 ${Build} PARENT_SCOPE )
		endif()
	endif()

	set( CastorMinLibraries
		${CastorMinLibraries}
		PARENT_SCOPE
	)

	set( CastorBinsDependencies
		${CastorBinsDependencies}
		PARENT_SCOPE
	)
endfunction( RenderersBuild )

function( RenderersSummary msg )
	set( msg_tmp "\n\nRenderers :" )
	set( msg_tmp "${msg_tmp}\n    OpenGL              ${Ogl}" )
	set( msg_tmp "${msg_tmp}\n    Direct3D 11         ${D3d11}" )
	SET( msg "${msg}${msg_tmp}" PARENT_SCOPE )
endfunction( RenderersSummary )

function( RenderersSetup )
	cpack_add_component_group( Renderers
		DESCRIPTION "Renderer plugins usable in Castor3D"
	)

	if( ${CASTOR_BUILD_RENDERER_OGL} )
		cpack_add_component( GlRenderSystem
			DISPLAY_NAME "GlRenderSystem plugin"
			DESCRIPTION "Enables OpenGL rendering API in Castor3D"
			REQUIRED
			GROUP Renderers
			DEPENDS Castor3D
			INSTALL_TYPES Minimal Full
		)
	endif ()
	if( WIN32 )
		if( ${CASTOR_BUILD_RENDERER_D3D11} )
			cpack_add_component( Dx11RenderSystem
				DISPLAY_NAME "Dx11RenderSystem plugin"
				DESCRIPTION "Enables Direct3D11 rendering API in Castor3D"
				GROUP Renderers
				DEPENDS Castor3D
				INSTALL_TYPES Full
			)
		endif ()
	endif ()
endfunction( RenderersSetup )
