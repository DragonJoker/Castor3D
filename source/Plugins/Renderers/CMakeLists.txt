option( CASTOR_BUILD_RENDERER_OGL "Build OpenGL Renderer plugin" TRUE )

function( RenderersInit )
	set( Ogl "no (Not wanted)" PARENT_SCOPE )
endfunction( RenderersInit )

function( RenderersBuild )
	if( ${CASTOR_BUILD_RENDERER_OGL} )
		set( Build ${Ogl} )
		add_subdirectory( Plugins/Renderers/GlRenderSystem )
		set( Ogl ${Build} PARENT_SCOPE )
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

function( RenderersSummary msg_in )
	set( msg_tmp "\n\nRenderers :" )
	set( msg_tmp "${msg_tmp}\n    OpenGL              ${Ogl}" )
	SET( msg "${msg_in}${msg_tmp}" PARENT_SCOPE )
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
endfunction( RenderersSetup )
