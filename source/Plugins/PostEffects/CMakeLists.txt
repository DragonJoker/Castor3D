option( CASTOR_BUILDGRP_POSTFX "Build Castor3D PostEffect plugins" TRUE )
option( CASTOR_BUILD_POSTFX_BLOOM "Build Bloom PostEffect plugin" TRUE )
option( CASTOR_BUILD_POSTFX_LIGHTSTREAKS "Build LightStreaks PostEffect plugin" TRUE )
option( CASTOR_BUILD_POSTFX_GRAYSCALE "Build GrayScale PostEffect plugin" TRUE )
option( CASTOR_BUILD_POSTFX_FXAA "Build FXAA PostEffect plugin" TRUE )
option( CASTOR_BUILD_POSTFX_SMAA "Build SMAA PostEffect plugin" TRUE )
option( CASTOR_BUILD_POSTFX_FILMGRAIN "Build FilmGrain PostEffect plugin" TRUE )
option( CASTOR_BUILD_POSTFX_LINEARMOTIONBLUR "Build LinearMotionBlur PostEffect plugin" TRUE )
option( CASTOR_BUILD_POSTFX_DRAWEDGES "Build DrawEdgesPostEffect PostEffect plugin" TRUE )

function( PostEffectsInit )
	set( Bloom "no (Not wanted)" PARENT_SCOPE )
	set( Grscl "no (Not wanted)" PARENT_SCOPE )
	set( Fxaa  "no (Not wanted)" PARENT_SCOPE )
	set( Smaa  "no (Not wanted)" PARENT_SCOPE )
	set( Fgrn  "no (Not wanted)" PARENT_SCOPE )
	set( Ltsk  "no (Not wanted)" PARENT_SCOPE )
	set( MtBr  "no (Not wanted)" PARENT_SCOPE )
	set( DrEd  "no (Not wanted)" PARENT_SCOPE )
endfunction( PostEffectsInit )

function( PostEffectsBuild )
	if ( ${CASTOR_BUILDGRP_POSTFX} )
		set( PostEffectIncludes
			${CASTOR_SOURCE_DIR}/include/Core
			${CASTOR_BINARY_DIR}/include/Core
			${CASTOR_SOURCE_DIR}/source/Plugins/PostEffects
			${CASTOR_BINARY_DIR}/source/Plugins/PostEffects
		)

		if( ${CASTOR_BUILD_POSTFX_BLOOM} )
			set( Build ${Bloom} )
			add_subdirectory( Plugins/PostEffects/BloomPostEffect )
			set( Bloom ${Build} PARENT_SCOPE )
		endif()

		if( ${CASTOR_BUILD_POSTFX_GRAYSCALE} )
			set( Build ${Grscl} )
			add_subdirectory( Plugins/PostEffects/GrayScalePostEffect )
			set( Grscl ${Build} PARENT_SCOPE )
		endif()

		if( ${CASTOR_BUILD_POSTFX_FXAA} )
			set( Build ${Fxaa} )
			add_subdirectory( Plugins/PostEffects/FxaaPostEffect )
			set( Fxaa ${Build} PARENT_SCOPE )
		endif()

		if( ${CASTOR_BUILD_POSTFX_SMAA} )
			set( Build ${Smaa} )
			add_subdirectory( Plugins/PostEffects/SmaaPostEffect )
			set( Smaa ${Build} PARENT_SCOPE )
		endif()

		if( ${CASTOR_BUILD_POSTFX_FILMGRAIN} )
			set( Build ${Fxaa} )
			add_subdirectory( Plugins/PostEffects/FilmGrainPostEffect )
			set( Fgrn ${Build} PARENT_SCOPE )
		endif()

		if( ${CASTOR_BUILD_POSTFX_LIGHTSTREAKS} )
			set( Build ${Ltsk} )
			add_subdirectory( Plugins/PostEffects/LightStreaksPostEffect )
			set( Ltsk ${Build} PARENT_SCOPE )
		endif()

		if( ${CASTOR_BUILD_POSTFX_LINEARMOTIONBLUR} )
			set( Build ${MtBr} )
			add_subdirectory( Plugins/PostEffects/LinearMotionBlurPostEffect )
			set( MtBr ${Build} PARENT_SCOPE )
		endif()

		if( ${CASTOR_BUILD_POSTFX_DRAWEDGES} )
			set( Build ${DrEd} )
			add_subdirectory( Plugins/PostEffects/DrawEdgesPostEffect )
			set( DrEd ${Build} PARENT_SCOPE )
		endif()

		set( CastorMinLibraries
			${CastorMinLibraries}
			PARENT_SCOPE
		)
		set( Castor3DBinsDependencies
			${Castor3DBinsDependencies}
			PARENT_SCOPE
		)
		set( CASTOR_INSTEXP_LIST
			${CASTOR_INSTEXP_LIST}
			PARENT_SCOPE
		)
	endif ()
endfunction( PostEffectsBuild )

function( PostEffectsSummary msg )
	if ( ${CASTOR_BUILDGRP_POSTFX} )
		set( msg_tmp "\n\n  PostEffect plugins:" )
		if( ${CASTOR_BUILD_POSTFX_BLOOM} )
			set( msg_tmp "${msg_tmp}\n    Bloom                ${Bloom}" )
		endif ()
		if( ${CASTOR_BUILD_POSTFX_GRAYSCALE} )
			set( msg_tmp "${msg_tmp}\n    GrayScale            ${Grscl}" )
		endif ()
		if( ${CASTOR_BUILD_POSTFX_FXAA} )
			set( msg_tmp "${msg_tmp}\n    FXAA                 ${Fxaa}" )
		endif ()
		if( ${CASTOR_BUILD_POSTFX_SMAA} )
			set( msg_tmp "${msg_tmp}\n    SMAA                 ${Smaa}" )
		endif ()
		if( ${CASTOR_BUILD_POSTFX_FILMGRAIN} )
			set( msg_tmp "${msg_tmp}\n    FilmGrain            ${Fgrn}" )
		endif ()
		if( ${CASTOR_BUILD_POSTFX_LIGHTSTREAKS} )
			set( msg_tmp "${msg_tmp}\n    LightStreaks         ${Ltsk}" )
		endif ()
		if( ${CASTOR_BUILD_POSTFX_LINEARMOTIONBLUR} )
			set( msg_tmp "${msg_tmp}\n    LinearMotionBlur     ${MtBr}" )
		endif ()
		if( ${CASTOR_BUILD_POSTFX_DRAWEDGES} )
			set( msg_tmp "${msg_tmp}\n    DrawEdgesPostEffect  ${DrEd}" )
		endif ()
		set( msg "${msg}${msg_tmp}" PARENT_SCOPE )
	endif ()
endfunction( PostEffectsSummary )

function( PostEffectsSetup )
	if ( ${CASTOR_BUILDGRP_POSTFX} )
		if( ${CASTOR_BUILD_POSTFX_BLOOM} )
			cpack_add_component( BloomPostEffect
				DISPLAY_NAME "BloomPostEffect"
				DESCRIPTION "Allows HDR Bloom post effect."
				GROUP Plugins
				DEPENDS Castor3D
				INSTALL_TYPES Developer Full
			)
		endif()
		if( ${CASTOR_BUILD_POSTFX_GRAYSCALE} )
			cpack_add_component( GrayScalePostEffect
				DISPLAY_NAME "GrayScalePostEffect"
				DESCRIPTION "Allows GrayScale post effect."
				GROUP Plugins
				DEPENDS Castor3D
				INSTALL_TYPES Developer Full
			)
		endif()
		if( ${CASTOR_BUILD_POSTFX_FXAA} )
			cpack_add_component( FxaaPostEffect
				DISPLAY_NAME "FxaaPostEffect"
				DESCRIPTION "Allows FXAA post effect."
				GROUP Plugins
				DEPENDS Castor3D
				INSTALL_TYPES Developer Full
			)
		endif()
		if( ${CASTOR_BUILD_POSTFX_SMAA} )
			cpack_add_component( SmaaPostEffect
				DISPLAY_NAME "SmaaPostEffect"
				DESCRIPTION "Allows SMAA post effect."
				GROUP Plugins
				DEPENDS Castor3D
				INSTALL_TYPES Developer Full
			)
		endif()
		if( ${CASTOR_BUILD_POSTFX_FILMGRAIN} )
			cpack_add_component( FilmGrainPostEffect
				DISPLAY_NAME "FilmGrainPostEffect"
				DESCRIPTION "Allows FilmGrain post effect."
				GROUP Plugins
				DEPENDS Castor3D
				INSTALL_TYPES Developer Full
			)
		endif()
		if( ${CASTOR_BUILD_POSTFX_LIGHTSTREAKS} )
			cpack_add_component( LightStreaksPostEffect
				DISPLAY_NAME "LightStreaksPostEffect"
				DESCRIPTION "Allows Light Streaks post effect."
				GROUP Plugins
				DEPENDS Castor3D
				INSTALL_TYPES Developer Full
			)
		endif()
		if( ${CASTOR_BUILD_POSTFX_LINEARMOTIONBLUR} )
			cpack_add_component( LinearMotionBlurPostEffect
				DISPLAY_NAME "LinearMotionBlurPostEffect"
				DESCRIPTION "Allows Linear Motion Blur post effect."
				GROUP Plugins
				DEPENDS Castor3D
				INSTALL_TYPES Developer Full
			)
		endif()
		if( ${CASTOR_BUILD_POSTFX_DRAWEDGES} )
			cpack_add_component( DrawEdgesPostEffect
				DISPLAY_NAME "DrawEdgesPostEffect"
				DESCRIPTION "Allows edges drawing post effect."
				GROUP Plugins
				DEPENDS Castor3D
				INSTALL_TYPES Developer Full
			)
		endif()
	endif ()
endfunction( PostEffectsSetup )
