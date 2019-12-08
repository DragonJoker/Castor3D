option( CASTOR_BUILDGRP_RENDERER "Build Castor3D Renderer plugins" ON )
option( CASTOR_BUILD_RENDERER_VKN "Build Vulkan Renderer plugin" ON )
if ( ANDROID )
	option( CASTOR_BUILD_RENDERER_OGL_ES3 "Build OpenGL ES 3.0 Renderer plugin" OFF )
elseif ( NOT APPLE )
	option( CASTOR_BUILD_RENDERER_OGL4 "Build OpenGL 4.2 Renderer plugin" ON )
	option( CASTOR_BUILD_RENDERER_OGL3 "Build OpenGL 3.2 Renderer plugin" ON )
	if ( WIN32 )
		option( CASTOR_BUILD_RENDERER_D3D11 "Build Direct3D 11 Renderer plugin" ON )
	endif ()
endif ()

function( RenderersInit )
	set( Vkn "no (Not wanted)" PARENT_SCOPE )
	if ( ANDROID )
		set( OglES3 "no (Not wanted)" PARENT_SCOPE )
	elseif ( NOT APPLE )
		set( Ogl4 "no (Not wanted)" PARENT_SCOPE )
		set( Ogl3 "no (Not wanted)" PARENT_SCOPE )
	endif ()
	set( Trs "no (Not wanted)" PARENT_SCOPE )
	if ( WIN32 )
		set( D3d11 "no (Not wanted)" PARENT_SCOPE )
	endif ()
	set( Trs "no (Not wanted)" PARENT_SCOPE )
endfunction( RenderersInit )

function( RenderersBuild )
	if ( ${CASTOR_BUILD_PLUGINS} AND ${CASTOR_BUILDGRP_RENDERER} )
		set( RendererIncludes
			${CASTOR_SOURCE_DIR}/include/Core
			${CASTOR_BINARY_DIR}/include/Core
			${CASTOR_SOURCE_DIR}/source/Plugins/Renderers
			${CASTOR_BINARY_DIR}/source/Plugins/Renderers
		)

		if ( ${CASTOR_BUILD_RENDERER_VKN} )
			set( Build ${Vkn} )
			add_subdirectory( Plugins/Renderers/VkRenderSystem )
			set( Vkn ${Build} PARENT_SCOPE )
		endif()
		if ( ANDROID )
			if ( ${CASTOR_BUILD_RENDERER_OGL_ES3} )
				set( Build ${OglES3} )
				add_subdirectory( Plugins/Renderers/GlES3RenderSystem )
				set( OglES3 ${Build} PARENT_SCOPE )
			endif()
		elseif ( NOT APPLE )
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
			if ( WIN32 )
				if ( ${CASTOR_BUILD_RENDERER_D3D11} )
					set( Build ${D3d11} )
					add_subdirectory( Plugins/Renderers/D3D11RenderSystem )
					set( D3d11 ${Build} PARENT_SCOPE )
				endif()
			endif ()
		endif ()
	elseif ( ${CASTOR_BUILDGRP_TEST} )
		set( RendererIncludes
			${CASTOR_SOURCE_DIR}/include/Core
			${CASTOR_BINARY_DIR}/include/Core
			${CASTOR_SOURCE_DIR}/source/Plugins/Renderers
			${CASTOR_BINARY_DIR}/source/Plugins/Renderers
		)
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
	if ( ${CASTOR_BUILD_PLUGINS} AND ${CASTOR_BUILDGRP_RENDERER} )
		set( msg_tmp "\n\nRenderer plugins :" )

		set( msg_tmp "${msg_tmp}\n    Vulkan               ${Vkn}" )
		if ( NOT APPLE )
			set( msg_tmp "${msg_tmp}\n    OpenGL 4.X           ${Ogl4}" )
			set( msg_tmp "${msg_tmp}\n    OpenGL 3.X           ${Ogl3}" )
			if ( WIN32 )
				set( msg_tmp "${msg_tmp}\n    Direct3D 11          ${D3d11}" )
			endif ()
		endif ()
		set( msg_tmp "${msg_tmp}\n    Test                 ${Trs}" )

		set( msg "${msg_in}${msg_tmp}" PARENT_SCOPE )
		set( msgtest "${msgtest_in}${msgtest_tmp}" PARENT_SCOPE )
	endif ()
endfunction( RenderersSummary )

function( RenderersSetup )
	cpack_add_component_group( Renderers
		DISPLAY_NAME "Renderers"
		DESCRIPTION "Renderer plugins usable in Castor3D."
		PARENT_GROUP Plugins
	)
	if ( ${CASTOR_BUILD_PLUGINS} AND ${CASTOR_BUILDGRP_RENDERER} )
		if( ${CASTOR_BUILD_RENDERER_VKN} )
			cpack_add_component( VkRenderSystem
				DISPLAY_NAME "VkRenderSystem"
				DESCRIPTION "Enables Vulkan rendering API in Castor3D."
				REQUIRED
				GROUP Renderers
				DEPENDS CastorUtils Castor3D Ashes
				INSTALL_TYPES Minimal Full
			)
		endif ()
		if ( ANDROID )
			if( ${CASTOR_BUILD_RENDERER_OGL_ES3} )
				cpack_add_component( GlES3RenderSystem
					DISPLAY_NAME "GlES3RenderSystem"
					DESCRIPTION "Enables OpenGL ES >= 3 rendering API in Castor3D."
					REQUIRED
					GROUP Renderers
					DEPENDS CastorUtils Castor3D Ashes
					INSTALL_TYPES Minimal Full
				)
			endif ()
		elseif ( NOT APPLE )
			if( ${CASTOR_BUILD_RENDERER_OGL4} )
				cpack_add_component( Gl4RenderSystem
					DISPLAY_NAME "Gl4RenderSystem"
					DESCRIPTION "Enables OpenGL >= 4.3 rendering API in Castor3D."
					REQUIRED
					GROUP Renderers
					DEPENDS CastorUtils Castor3D Ashes
					INSTALL_TYPES Minimal Full
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
			endif ()
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
