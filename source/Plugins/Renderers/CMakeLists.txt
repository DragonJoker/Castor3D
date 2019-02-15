option( CASTOR_BUILDGRP_RENDERER "Build Castor3D Renderer plugins" TRUE )
option( CASTOR_BUILD_RENDERER_OGL4 "Build OpenGL 4.2 Renderer plugin" TRUE )
option( CASTOR_BUILD_RENDERER_OGL3 "Build OpenGL 3.2 Renderer plugin" TRUE )
option( CASTOR_BUILD_RENDERER_VKN "Build Vulkan Renderer plugin" TRUE )
option( CASTOR_BUILD_RENDERER_OGL_ES3 "Build OpenGL ES 3.0 Renderer plugin" TRUE )
if ( WIN32 )
	option( CASTOR_BUILD_RENDERER_D3D11 "Build Direct3D 11 Renderer plugin" TRUE )
endif ()

function( RenderersInit )
	set( Ogl4 "no (Not wanted)" PARENT_SCOPE )
	set( Ogl3 "no (Not wanted)" PARENT_SCOPE )
	set( OglES3 "no (Not wanted)" PARENT_SCOPE )
	set( Vkn "no (Not wanted)" PARENT_SCOPE )
	set( OglT "no (Not wanted)" PARENT_SCOPE )
	if ( WIN32 )
		set( D3d11 "no (Not wanted)" PARENT_SCOPE )
	endif ()
	set( Trs "no (Not wanted)" PARENT_SCOPE )
endfunction( RenderersInit )

function( RenderersBuild )
	if ( ${CASTOR_BUILD_PLUGINS} AND ${CASTOR_BUILDGRP_RENDERER} )
		if ( ANDROID )
			if ( ${CASTOR_BUILD_RENDERER_OGL_ES3} )
				set( Build ${OglES3} )
				add_subdirectory( Plugins/Renderers/GlES3RenderSystem )
				set( OglES3 ${Build} PARENT_SCOPE )
			endif()
		else ()
			if ( ${CASTOR_BUILD_RENDERER_OGL4} )
				set( Build ${Ogl4} )
				add_subdirectory( Plugins/Renderers/Gl4RenderSystem )
				set( Ogl4 ${Build} PARENT_SCOPE )
			endif()
			if ( ${CASTOR_BUILD_RENDERER_OGL3} )
				set( Build ${Ogl3} )
				add_subdirectory( Plugins/Renderers/Gl3RenderSystem )
				set( Ogl3 ${Build} PARENT_SCOPE )
			endif()
			if ( ${CASTOR_BUILD_RENDERER_VKN} )
				set( Build ${Vkn} )
				add_subdirectory( Plugins/Renderers/VkRenderSystem )
				set( Vkn ${Build} PARENT_SCOPE )
			endif()
			if ( WIN32 )
				if ( ${CASTOR_BUILD_RENDERER_D3D11} )
					set( Build ${D3d11} )
					add_subdirectory( Plugins/Renderers/D3D11RenderSystem )
					set( D3d11 ${Build} PARENT_SCOPE )
				endif()
			endif ()
		endif ()
	endif ()

	if ( ${CASTOR_BUILDGRP_TEST} )
		set( Build ${Trs} )
		add_subdirectory( Plugins/Renderers/TestRenderSystem )
		set( Trs ${Build} PARENT_SCOPE )
	endif()

	set( CastorMinLibraries
		${CastorMinLibraries}
		PARENT_SCOPE
	)

	set( Castor3DBinsDependencies
		${Castor3DBinsDependencies}
		PARENT_SCOPE
	)
endfunction( RenderersBuild )

function( RenderersSummary msg_in )
	set( msg_tmp "\n\nRenderers :" )

	if ( ${CASTOR_BUILD_PLUGINS} AND ${CASTOR_BUILDGRP_RENDERER} )
		set( msg_tmp "${msg_tmp}\n    OpenGL 4.X           ${Ogl4}" )
		set( msg_tmp "${msg_tmp}\n    OpenGL 3.X           ${Ogl3}" )
		set( msg_tmp "${msg_tmp}\n    Vulkan               ${Vkn}" )
		if ( WIN32 )
			set( msg_tmp "${msg_tmp}\n    Direct3D 11          ${D3d11}" )
		endif ()
		set( msg_tmp "${msg_tmp}\n    Test                 ${Trs}" )
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
	if ( ${CASTOR_BUILD_PLUGINS} AND ${CASTOR_BUILDGRP_RENDERER} )
		if( ${CASTOR_BUILD_RENDERER_OGL4} )
			cpack_add_component( Gl4RenderSystem
				DISPLAY_NAME "Gl4RenderSystem"
				DESCRIPTION "Enables OpenGL >= 4.3 rendering API in Castor3D."
				REQUIRED
				GROUP Renderers
				DEPENDS CastorUtils Castor3D Ashes
				INSTALL_TYPES Minimal Full
			)
			cpack_add_component( Gl$RenderSystem_dev
				DISPLAY_NAME "Gl4RenderSystem SDK"
				DESCRIPTION "Development files for Gl4RenderSystem."
				GROUP Development
				DEPENDS Gl4RenderSystem CastorUtils_dev Castor3D_dev Ashes_dev
				INSTALL_TYPES Developer Full
			)
		endif ()
		if( ${CASTOR_BUILD_RENDERER_OGL3} )
			cpack_add_component( Gl3RenderSystem
				DISPLAY_NAME "Gl3RenderSystem"
				DESCRIPTION "Enables OpenGL >= 3.3 rendering API in Castor3D."
				REQUIRED
				GROUP Renderers
				DEPENDS CastorUtils Castor3D Ashes
				INSTALL_TYPES Minimal Full
			)
			cpack_add_component( Gl3RenderSystem_dev
				DISPLAY_NAME "Gl3RenderSystem SDK"
				DESCRIPTION "Development files for Gl3RenderSystem."
				GROUP Development
				DEPENDS Gl3RenderSystem CastorUtils_dev Castor3D_dev Ashes_dev
				INSTALL_TYPES Developer Full
			)
		endif ()
		if( ${CASTOR_BUILD_RENDERER_VKN} )
			cpack_add_component( VkRenderSystem
				DISPLAY_NAME "VkRenderSystem"
				DESCRIPTION "Enables Vulkan rendering API in Castor3D."
				REQUIRED
				GROUP Renderers
				DEPENDS CastorUtils Castor3D Ashes
				INSTALL_TYPES Minimal Full
			)
			cpack_add_component( VkRenderSystem_dev
				DISPLAY_NAME "VkRenderSystem SDK"
				DESCRIPTION "Development files for VkRenderSystem."
				GROUP Development
				DEPENDS VkRenderSystem CastorUtils_dev Castor3D_dev Ashes_dev
				INSTALL_TYPES Developer Full
			)
		endif ()
		if( WIN32 AND ${CASTOR_BUILD_RENDERER_D3D11} )
			cpack_add_component( D3D11RenderSystem
				DISPLAY_NAME "D3D11RenderSystem"
				DESCRIPTION "Enables Direct3D 11 rendering API in Castor3D."
				REQUIRED
				GROUP Renderers
				DEPENDS CastorUtils Castor3D Ashes
				INSTALL_TYPES Minimal Full
			)
			cpack_add_component( D3D11RenderSystem_dev
				DISPLAY_NAME "D3D11RenderSystem SDK"
				DESCRIPTION "Development files for D3D11RenderSystem."
				GROUP Development
				DEPENDS D3D11RenderSystem CastorUtils_dev Castor3D_dev Ashes_dev
				INSTALL_TYPES Developer Full
			)
		endif ()
	endif ()
	if( ${CASTOR_BUILDGRP_TEST} )
		cpack_add_component( TestRenderSystem
			DISPLAY_NAME "TestRenderSystem"
			DESCRIPTION "Enables Test rendering API in Castor3D."
			GROUP Test
			DEPENDS CastorUtils Castor3D Ashes
			INSTALL_TYPES Developer Full
		)
		cpack_add_component( TestRenderSystem_dev
			DISPLAY_NAME "TestRenderSystem SDK"
			DESCRIPTION "Development files for TestRenderSystem."
			GROUP Test
			DEPENDS TestRenderSystem CastorUtils_dev Castor3D_dev Ashes_dev
			INSTALL_TYPES Developer Full
		)
	endif ()
endfunction( RenderersSetup )
