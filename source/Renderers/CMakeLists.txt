option( CASTOR_BUILD_RENDERER_OGL "Build OpenGL Renderer plugin" TRUE )

if( WIN32 )
    option( CASTOR_BUILD_RENDERER_D3D9 "Build Direct3D9 Renderer plugin (needs Direct3D 9 SDK)" TRUE )
    option( CASTOR_BUILD_RENDERER_D3D10 "Build Direct3D10 Renderer plugin (needs Direct3D 10 SDK)" TRUE )
    option( CASTOR_BUILD_RENDERER_D3D11 "Build Direct3D11 Renderer plugin (needs Direct3D 11 SDK)" TRUE )
endif()

function( RenderersInit )
	set( Ogl "no (Not wanted)" PARENT_SCOPE )
	if ( WIN32 )
		set( D3d9 "no (Not wanted)" PARENT_SCOPE )
		set( D3d10 "no (Not wanted)" PARENT_SCOPE )
		set( D3d11 "no (Not wanted)" PARENT_SCOPE )
	else()
		set( D3d9 "no (Not available)" PARENT_SCOPE )
		set( D3d10 "no (Not available)" PARENT_SCOPE )
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
		if( ${CASTOR_BUILD_RENDERER_D3D9} )
			set( Build ${D3d9} )
			add_subdirectory( Renderers/Dx9RenderSystem )
			set( D3d9 ${Build} PARENT_SCOPE )
		endif()

		if( ${CASTOR_BUILD_RENDERER_D3D10} )
			set( Build ${D3d10} )
			add_subdirectory( Renderers/Dx10RenderSystem )
			set( D3d10 ${Build} PARENT_SCOPE )
		endif()

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
	set( msg_tmp "${msg_tmp}\n    Direct3D 9          ${D3d9}" )
	set( msg_tmp "${msg_tmp}\n    Direct3D 10         ${D3d10}" )
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
		if( ${CASTOR_BUILD_RENDERER_D3D9} )
			cpack_add_component( Dx9RenderSystem
				DISPLAY_NAME "Dx9RenderSystem plugin"
				DESCRIPTION "Enables Direct3D9 rendering API in Castor3D"
				GROUP Renderers
				DEPENDS Castor3D
				INSTALL_TYPES Full
			)
		endif ()
		if( ${CASTOR_BUILD_RENDERER_D3D10} )
			cpack_add_component( Dx10RenderSystem
				DISPLAY_NAME "Dx10RenderSystem plugin"
				DESCRIPTION "Enables Direct3D10 rendering API in Castor3D"
				GROUP Renderers
				DEPENDS Castor3D
				INSTALL_TYPES Full
			)
		endif ()
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
