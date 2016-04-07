option( CASTOR_BUILDGRP_POSTFX "Build Castor3D PostEffect plugins" TRUE )
option( CASTOR_BUILD_POSTFX_BLOOM "Build Bloom PostEffect plugin" TRUE )

function( PostEffectsInit )
	set( Bloom "no (Not wanted)" PARENT_SCOPE )
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
		if( ${CASTOR_BUILD_POSTFX_BLOOM} )
			set( msg_tmp "${msg_tmp}\n    Bloom               ${Bloom}" )
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
				GROUP Plugins
				DEPENDS Castor3D
				INSTALL_TYPES Full
			)
		endif()
	endif ()
endfunction( PostEffectsSetup )
