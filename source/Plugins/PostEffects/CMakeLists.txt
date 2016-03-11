option( CASTOR_BUILDGRP_POSTFX "Build Castor3D PostEffect plugins" TRUE )
option( CASTOR_BUILD_POSTFX_HDRBLOOM "Build HDR Bloom PostEffect plugin" TRUE )

function( PostEffectsInit )
	set( HdrBloom "no (Not wanted)" PARENT_SCOPE )
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

		if( ${CASTOR_BUILD_POSTFX_HDRBLOOM} )
			set( Build ${HdrBloom} )
			add_subdirectory( Plugins/PostEffects/HdrBloomPostEffect )
			set( HdrBloom ${Build} PARENT_SCOPE )
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

function( PostEffectsSummary msg )
	if ( ${CASTOR_BUILDGRP_POSTFX} )
		set( msg_tmp "\n\nPostEffects :" )
		if( ${CASTOR_BUILD_POSTFX_HDRBLOOM} )
			set( msg_tmp "${msg_tmp}\n    HDR Bloom           ${HdrBloom}" )
		endif ()
		set( msg "${msg}${msg_tmp}" PARENT_SCOPE )
	endif ()
endfunction( PostEffectsSummary )

function( PostEffectsSetup )
	if ( ${CASTOR_BUILDGRP_POSTFX} )
		if( ${CASTOR_BUILD_POSTFX_HDRBLOOM} )
			cpack_add_component( HdrBloomPostEffect
				DISPLAY_NAME "HdrBloomPostEffect"
				DESCRIPTION "Allows HDR Bloom post effect."
				GROUP Plugins
				DEPENDS Castor3D
				INSTALL_TYPES Full
			)
		endif()
	endif ()
endfunction( PostEffectsSetup )
