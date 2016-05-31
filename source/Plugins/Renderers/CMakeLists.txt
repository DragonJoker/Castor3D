option( CASTOR_BUILD_RENDERER_OGL "Build OpenGL Renderer plugin" TRUE )

function( RenderersInit )
	set( Ogl "no (Not wanted)" PARENT_SCOPE )
	set( Trs "no (Not wanted)" PARENT_SCOPE )
endfunction( RenderersInit )

function( RenderersBuild )
	if ( ${CASTOR_BUILD_RENDERER_OGL} )
		set( Build ${Ogl} )
		add_subdirectory( Plugins/Renderers/GlRenderSystem )
		set( Ogl ${Build} PARENT_SCOPE )
	endif()

	if ( ${CASTOR_BUILDGRP_TEST} )
		set( Build ${Trs} )
		add_subdirectory( Plugins/Renderers/TestRenderSystem )
		set( Trs ${Build} PARENT_SCOPE )
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
	if ( ${CASTOR_BUILDGRP_TEST} )
		set( msg_tmp "${msg_tmp}\n    Test                ${Trs}" )
	endif ()
	SET( msg "${msg_in}${msg_tmp}" PARENT_SCOPE )
endfunction( RenderersSummary )

function( RenderersSetup )
	cpack_add_component_group( Renderers
		DISPLAY_NAME "Renderers"
		DESCRIPTION "Renderer plugins usable in Castor3D."
		PARENT_GROUP Plugins
	)
	if( ${CASTOR_BUILD_RENDERER_OGL} )
		cpack_add_component( GlRenderSystem
			DISPLAY_NAME "GlRenderSystem"
			DESCRIPTION "Enables OpenGL rendering API in Castor3D."
			REQUIRED
			GROUP Renderers
			DEPENDS Castor3D
			INSTALL_TYPES Minimal Full
		)
		cpack_add_component( GlRenderSystem_dev
			DISPLAY_NAME "GlRenderSystem SDK"
			DESCRIPTION "Development files for GlRenderSystem."
			GROUP Development
			DEPENDS Castor3D CastorUtils_dev Castor3D Castor3D_dev
			INSTALL_TYPES Developer Full
		)
	endif ()
	if( ${CASTOR_BUILDGRP_TEST} )
		cpack_add_component( TestRenderSystem
			DISPLAY_NAME "TestRenderSystem"
			DESCRIPTION "Enables Test rendering API in Castor3D."
			REQUIRED
			GROUP Renderers
			DEPENDS Castor3D
			INSTALL_TYPES Test Full
		)
	endif ()
endfunction( RenderersSetup )
