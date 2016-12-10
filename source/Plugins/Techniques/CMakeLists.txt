option( CASTOR_BUILDGRP_TECHNIQUE "Build Castor3D RenderTechnique plugins" TRUE )
option( CASTOR_BUILD_TECHNIQUE_DEFERRED "Build Deferred Lighting RenderTechnique plugin" TRUE )
option( CASTOR_BUILD_TECHNIQUE_FORWARD_MSAA "Build Forward MSAA RenderTechnique plugin" TRUE )
option( CASTOR_BUILD_TECHNIQUE_DEFERRED_MSAA "Build Deferred Lighting & MSAA RenderTechnique plugin" TRUE )

function( TechniquesInit )
	set( Frwd "no (Not wanted)" PARENT_SCOPE )
	set( Defr "no (Not wanted)" PARENT_SCOPE )
	set( FwMs "no (Not wanted)" PARENT_SCOPE )
	set( DrMs "no (Not wanted)" PARENT_SCOPE )
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
	add_subdirectory( Plugins/Techniques/ForwardTechnique )
	set( Drct ${Build} PARENT_SCOPE )

	if ( ${CASTOR_BUILDGRP_TECHNIQUE} )
		if( ${CASTOR_BUILD_TECHNIQUE_DEFERRED} )
			set( Build ${Defr} )
			add_subdirectory( Plugins/Techniques/DeferredTechnique )
			set( Defr ${Build} PARENT_SCOPE )
		endif()

		if( ${CASTOR_BUILD_TECHNIQUE_FORWARD_MSAA} )
			set( Build ${Msaa} )
			add_subdirectory( Plugins/Techniques/ForwardMsaaTechnique )
			set( Msaa ${Build} PARENT_SCOPE )
		endif()

		if( ${CASTOR_BUILD_TECHNIQUE_DEFERRED_MSAA} )
			set( Build ${DrMs} )
			add_subdirectory( Plugins/Techniques/DeferredMsaaTechnique )
			set( DrMs ${Build} PARENT_SCOPE )
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

function( TechniquesSummary msg msgtest )
	set( msg_tmp "\n\nRenderTechniques :" )
	set( msg_tmp "${msg_tmp}\n    Forward             ${Drct}" )
  
	if ( ${CASTOR_BUILDGRP_TECHNIQUE} )
		if( ${CASTOR_BUILD_TECHNIQUE_DEFERRED} )
			set( msg_tmp "${msg_tmp}\n    Deferred Lighting   ${Defr}" )
		endif()
		if( ${CASTOR_BUILD_TECHNIQUE_FORWARD_MSAA} )
			set( msg_tmp "${msg_tmp}\n    Forward MSAA        ${Msaa}" )
		endif()
		if( ${CASTOR_BUILD_TECHNIQUE_DEFERRED_MSAA} )
			set( msg_tmp "${msg_tmp}\n    Deferred & MSAA     ${DrMs}" )
		endif()
	endif()
	set( msg "${msg}${msg_tmp}" PARENT_SCOPE )
endfunction( TechniquesSummary )

function( TechniquesSetup )
	cpack_add_component_group( Techniques
		DISPLAY_NAME "Techniques"
		DESCRIPTION "Render technique plugins usable in Castor3D."
		PARENT_GROUP Plugins
	)
	cpack_add_component( ForwardTechnique
		DISPLAY_NAME "ForwardTechnique"
		DESCRIPTION "Allows rendering with Forward technique"
		GROUP Techniques
		DEPENDS Castor3D
		INSTALL_TYPES Minimal Full
	)

	if ( ${CASTOR_BUILDGRP_TECHNIQUE} )
		if( ${CASTOR_BUILD_TECHNIQUE_DEFERRED} )
			cpack_add_component( DeferredTechnique
				DISPLAY_NAME "DeferredTechnique"
				DESCRIPTION "Allows rendering with Deferred Lighting technique."
				GROUP Techniques
				DEPENDS Castor3D
				INSTALL_TYPES Developer Full
			)
		endif()
		if( ${CASTOR_BUILD_TECHNIQUE_FORWARD_MSAA} )
			cpack_add_component( ForwardMsaaTechnique
				DISPLAY_NAME "ForwardMsaaTechnique"
				DESCRIPTION "Allows rendering with Forward MSAA technique."
				GROUP Techniques
				DEPENDS Castor3D
				INSTALL_TYPES Developer Full
			)
		endif()
		if( ${CASTOR_BUILD_TECHNIQUE_DEFERRED_MSAA} )
			cpack_add_component( DeferredMsaaTechnique
				DISPLAY_NAME "DeferredMsaaTechnique"
				DESCRIPTION "Allows rendering with Deferred Lighting and MSAA technique."
				GROUP Techniques
				DEPENDS Castor3D
				INSTALL_TYPES Developer Full
			)
		endif()
	endif ()
endfunction( TechniquesSetup )
