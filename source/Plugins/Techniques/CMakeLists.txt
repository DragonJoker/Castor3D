option( CASTOR_BUILDGRP_TECHNIQUE "Build Castor3D RenderTechnique plugins" TRUE )
option( CASTOR_BUILD_TECHNIQUE_DEFERRED "Build Deferred Lighting RenderTechnique plugin" TRUE )
option( CASTOR_BUILD_TECHNIQUE_MSAA "Build MSAA RenderTechnique plugin" TRUE )
option( CASTOR_BUILD_TECHNIQUE_SSAA "Build SSAA RenderTechnique plugin" TRUE )

function( TechniquesInit )
	set( Drct "no (Not wanted)" PARENT_SCOPE )
	set( Defr "no (Not wanted)" PARENT_SCOPE )
	set( Msaa "no (Not wanted)" PARENT_SCOPE )
	set( Ssaa "no (Not wanted)" PARENT_SCOPE )
endfunction( TechniquesInit )

function( TechniquesBuild )
	set( TechniqueLibraries ${CastorMinLibraries} )

	set( TechniqueIncludes
		${CMAKE_SOURCE_DIR}/Core/CastorUtils/Src
		${CMAKE_SOURCE_DIR}/Core/Castor3D/Src
		${CMAKE_BINARY_DIR}/Core/CastorUtils/Src
		${CMAKE_SOURCE_DIR}/Core/GlslWriter/Src
	)

	set( Build ${Drct} )
	add_subdirectory( Plugins/Techniques/DirectTechnique )
	set( Drct ${Build} PARENT_SCOPE )

	if ( ${CASTOR_BUILDGRP_TECHNIQUE} )
		if( ${CASTOR_BUILD_TECHNIQUE_DEFERRED} )
			set( Build ${Defr} )
			add_subdirectory( Plugins/Techniques/DeferredTechnique )
			set( Defr ${Build} PARENT_SCOPE )
		endif()

		if( ${CASTOR_BUILD_TECHNIQUE_MSAA} )
			set( Build ${Msaa} )
			add_subdirectory( Plugins/Techniques/MsaaTechnique )
			set( Msaa ${Build} PARENT_SCOPE )
		endif()

		if( ${CASTOR_BUILD_TECHNIQUE_SSAA} )
			set( Build ${Ssaa} )
			add_subdirectory( Plugins/Techniques/SsaaTechnique )
			set( Ssaa ${Build} PARENT_SCOPE )
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
endfunction( TechniquesBuild )

function( TechniquesSummary msg )
	set( msg_tmp "\n\nRenderTechniques :" )
	set( msg_tmp "${msg_tmp}\n    Direct              ${Drct}" )
  
	if ( ${CASTOR_BUILDGRP_TECHNIQUE} )
		if( ${CASTOR_BUILD_TECHNIQUE_DEFERRED} )
			set( msg_tmp "${msg_tmp}\n    Deferred Lighting   ${Defr}" )
		endif()
		if( ${CASTOR_BUILD_TECHNIQUE_MSAA} )
			set( msg_tmp "${msg_tmp}\n    MSAA                ${Msaa}" )
		endif()
		if( ${CASTOR_BUILD_TECHNIQUE_SSAA} )
			set( msg_tmp "${msg_tmp}\n    SSAA                ${Ssaa}" )
		endif()
	endif()
	set( msg "${msg}${msg_tmp}" PARENT_SCOPE )
endfunction( TechniquesSummary )

function( TechniquesSetup )
	cpack_add_component( DirectTechnique
		DISPLAY_NAME "DirectTechnique"
		DESCRIPTION "Default direct rendering technique"
		GROUP Techniques
		DEPENDS Castor3D
		INSTALL_TYPES Full
	)

	if ( ${CASTOR_BUILDGRP_TECHNIQUE} )
		if( ${CASTOR_BUILD_TECHNIQUE_DEFERRED} )
			cpack_add_component( DeferredTechnique
				DISPLAY_NAME "DeferredTechnique"
				DESCRIPTION "Allows rendering with Deferred Lighting technique."
				GROUP Plugins
				DEPENDS Castor3D
				INSTALL_TYPES Full
			)
		endif()
		if( ${CASTOR_BUILD_TECHNIQUE_MSAA} )
			cpack_add_component( MsaaTechnique
				DISPLAY_NAME "MsaaTechnique"
				DESCRIPTION "Allows rendering with MSAA technique."
				GROUP Plugins
				DEPENDS Castor3D
				INSTALL_TYPES Full
			)
		endif()
		if( ${CASTOR_BUILD_TECHNIQUE_SSAA} )
			cpack_add_component( SsaaTechnique
				DISPLAY_NAME "SsaaTechnique"
				DESCRIPTION "Allows rendering with SSAA technique."
				GROUP Plugins
				DEPENDS Castor3D
				INSTALL_TYPES Full
			)
		endif()
	endif ()
endfunction( TechniquesSetup )
