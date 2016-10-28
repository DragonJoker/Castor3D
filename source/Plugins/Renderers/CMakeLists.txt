option( CASTOR_BUILD_RENDERER_OGL "Build OpenGL Renderer plugin" TRUE )
option( CASTOR_BUILD_RENDERER_TEST_OGL "Build OpenGL Renderer Test application" TRUE )

function( RenderersInit )
	set( Ogl "no (Not wanted)" PARENT_SCOPE )
	set( OglT "no (Not wanted)" PARENT_SCOPE )
	set( Trs "no (Not wanted)" PARENT_SCOPE )
endfunction( RenderersInit )

function( RenderersBuild )
	if ( ${CASTOR_BUILD_RENDERER_OGL} )
		set( Build ${Ogl} )
		add_subdirectory( Plugins/Renderers/GlRenderSystem )
		set( Ogl ${Build} PARENT_SCOPE )

		if ( ${CASTOR_BUILDGRP_TEST} AND ${CASTOR_BUILD_RENDERER_TEST_OGL} )
			set( Build ${OglT} )
			add_subdirectory( Plugins/Renderers/GlRenderSystem/Test )
			set( OglT ${Build} PARENT_SCOPE )
		endif()
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

function( RenderersSummary msg_in msgtest_in )
	set( msg_tmp "\n\nRenderers :" )
	set( msg_tmp "${msg_tmp}\n    OpenGL              ${Ogl}" )
	if ( ${CASTOR_BUILDGRP_TEST} )
		set( msg_tmp "${msg_tmp}\n    Test                ${Trs}" )
		set( msgtest_tmp "${msgtest_tmp}\n    OpenGL Test         ${OglT}" )
	endif ()
	SET( msg "${msg_in}${msg_tmp}" PARENT_SCOPE )
	SET( msgtest "${msgtest_in}${msgtest_tmp}" PARENT_SCOPE )
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
			DEPENDS GlRenderSystem CastorUtils_dev Castor3D_dev
			INSTALL_TYPES Developer Full
		)
	endif ()
	if( ${CASTOR_BUILDGRP_TEST} )
		cpack_add_component( TestRenderSystem
			DISPLAY_NAME "TestRenderSystem"
			DESCRIPTION "Enables Test rendering API in Castor3D."
			GROUP Test
			DEPENDS Castor3D
			INSTALL_TYPES Test Full
		)
		cpack_add_component( TestRenderSystem_dev
			DISPLAY_NAME "TestRenderSystem SDK"
			DESCRIPTION "Development files for TestRenderSystem."
			GROUP Test
			DEPENDS TestRenderSystem CastorUtils_dev Castor3D_dev
			INSTALL_TYPES Developer Test Full
		)
	endif ()
endfunction( RenderersSetup )
