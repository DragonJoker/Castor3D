option( CASTOR_BUILDGRP_POSTFX "Build Castor3D PostEffect plugins" TRUE )
option( CASTOR_BUILD_POSTFX_BLOOM "Build Bloom PostEffect plugin" TRUE )
option( CASTOR_BUILD_POSTFX_GRAYSCALE "Build GrayScale PostEffect plugin" TRUE )
option( CASTOR_BUILD_POSTFX_FXAA "Build FXAA PostEffect plugin" TRUE )

function( PostEffectsInit )
	set( Bloom "no (Not wanted)" PARENT_SCOPE )
	set( Grscl "no (Not wanted)" PARENT_SCOPE )
	set( Fxaa  "no (Not wanted)" PARENT_SCOPE )
endfunction( PostEffectsInit )

function( PostEffectsBuild )
	if ( ${CASTOR_BUILDGRP_POSTFX} )
		set( PostEffectLibraries ${CastorMinLibraries} )

		set( PostEffectIncludes
			${CMAKE_SOURCE_DIR}/Core/CastorUtils/Src
			${CMAKE_SOURCE_DIR}/Core/Castor3D/Src
			${CMAKE_BINARY_DIR}/Core/CastorUtils/Src
			${CMAKE_SOURCE_DIR}/Core/GlslWriter/Src
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

		set( CastorMinLibraries
			${CastorMinLibraries}
			PARENT_SCOPE
		)

		set( CastorBinsDependencies
			${CastorBinsDependencies}
			PARENT_SCOPE
		)
	endif ()
endfunction( PostEffectsBuild )

function( PostEffectsSummary msg msgtest )
	if ( ${CASTOR_BUILDGRP_POSTFX} )
		set( msg_tmp "\n\nPostEffects :" )
		if( ${CASTOR_BUILD_POSTFX_BLOOM} )
			set( msg_tmp "${msg_tmp}\n    Bloom               ${Bloom}" )
		endif ()
		if( ${CASTOR_BUILD_POSTFX_GRAYSCALE} )
			set( msg_tmp "${msg_tmp}\n    GrayScale           ${Grscl}" )
		endif ()
		if( ${CASTOR_BUILD_POSTFX_FXAA} )
			set( msg_tmp "${msg_tmp}\n    FXAA                ${Fxaa}" )
		endif ()
		set( msg "${msg}${msg_tmp}" PARENT_SCOPE )
	endif ()
endfunction( PostEffectsSummary )

function( PostEffectsSetup )
	if ( ${CASTOR_BUILDGRP_POSTFX} )
		cpack_add_component_group( PostEffects
			DISPLAY_NAME "PostEffects"
			DESCRIPTION "Post effect plugins usable in Castor3D."
			PARENT_GROUP Plugins
		)
		if( ${CASTOR_BUILD_POSTFX_BLOOM} )
			cpack_add_component( BloomPostEffect
				DISPLAY_NAME "BloomPostEffect"
				DESCRIPTION "Allows HDR Bloom post effect."
				GROUP PostEffects
				DEPENDS Castor3D
				INSTALL_TYPES Developer Full
			)
		endif()
		if( ${CASTOR_BUILD_POSTFX_GRAYSCALE} )
			cpack_add_component( GrayScalePostEffect
				DISPLAY_NAME "GrayScalePostEffect"
				DESCRIPTION "Allows GrayScale post effect."
				GROUP PostEffects
				DEPENDS Castor3D
				INSTALL_TYPES Developer Full
			)
		endif()
		if( ${CASTOR_BUILD_POSTFX_FXAA} )
			cpack_add_component( FxaaPostEffect
				DISPLAY_NAME "FxaaPostEffect"
				DESCRIPTION "Allows FXAA post effect."
				GROUP PostEffects
				DEPENDS Castor3D
				INSTALL_TYPES Developer Full
			)
		endif()
	endif ()
endfunction( PostEffectsSetup )
