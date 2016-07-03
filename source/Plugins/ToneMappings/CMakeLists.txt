option( CASTOR_BUILDGRP_TONEMAPS "Build Castor3D Tone mapping plug-ins" TRUE )
option( CASTOR_BUILD_TONEMAP_HAARM "Build Haarm Pieter Duiker Tone mapping plug-in" TRUE )
option( CASTOR_BUILD_TONEMAP_HEJL "Build Hejl Burgess Dawson Tone mapping plug-in" TRUE )
option( CASTOR_BUILD_TONEMAP_REINHARD "Build Reinhard Tone mapping plug-in" TRUE )

function( ToneMappingsInit )
	set( Linear "no (Not wanted)" PARENT_SCOPE )
	set( Haarm "no (Not wanted)" PARENT_SCOPE )
	set( Hejl "no (Not wanted)" PARENT_SCOPE )
	set( Reinhard "no (Not wanted)" PARENT_SCOPE )
endfunction( ToneMappingsInit )

function( ToneMappingsBuild )
	if ( ${CASTOR_BUILDGRP_TONEMAPS} )
		set( ToneMappingLibraries ${CastorMinLibraries} )

		set( ToneMappingIncludes
			${CMAKE_SOURCE_DIR}/Core/CastorUtils/Src
			${CMAKE_SOURCE_DIR}/Core/Castor3D/Src
			${CMAKE_BINARY_DIR}/Core/CastorUtils/Src
			${CMAKE_SOURCE_DIR}/Core/GlslWriter/Src
		)

		set( Build ${Linear} )
		add_subdirectory( Plugins/ToneMappings/LinearToneMapping )
		set( Linear ${Build} PARENT_SCOPE )

		if( ${CASTOR_BUILD_TONEMAP_HAARM} )
			set( Build ${Haarm} )
			add_subdirectory( Plugins/ToneMappings/HaarmPieterDuikerToneMapping )
			set( Haarm ${Build} PARENT_SCOPE )
		endif()

		if( ${CASTOR_BUILD_TONEMAP_HEJL} )
			set( Build ${Hejl} )
			add_subdirectory( Plugins/ToneMappings/HejlBurgessDawsonToneMapping )
			set( Hejl ${Build} PARENT_SCOPE )
		endif()

		if( ${CASTOR_BUILD_TONEMAP_REINHARD} )
			set( Build ${Reinhard} )
			add_subdirectory( Plugins/ToneMappings/ReinhardToneMapping )
			set( Reinhard ${Build} PARENT_SCOPE )
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
endfunction( ToneMappingsBuild )

function( ToneMappingsSummary msg )
	if ( ${CASTOR_BUILDGRP_TONEMAPS} )
		set( msg_tmp "\n\nToneMappings :" )
			set( msg_tmp "${msg_tmp}\n    Linear              ${Linear}" )
		if( ${CASTOR_BUILD_TONEMAP_HAARM} )
			set( msg_tmp "${msg_tmp}\n    Haarm Pieter Duiker ${Haarm}" )
		endif ()
		if( ${CASTOR_BUILD_TONEMAP_HEJL} )
			set( msg_tmp "${msg_tmp}\n    Hejl Burgess Dawson ${Hejl}" )
		endif ()
		if( ${CASTOR_BUILD_TONEMAP_REINHARD} )
			set( msg_tmp "${msg_tmp}\n    Reinhard            ${Reinhard}" )
		endif ()
		set( msg "${msg}${msg_tmp}" PARENT_SCOPE )
	endif ()
endfunction( ToneMappingsSummary )

function( ToneMappingsSetup )
	if ( ${CASTOR_BUILDGRP_TONEMAPS} )
		cpack_add_component_group( ToneMappings
			DISPLAY_NAME "ToneMappings"
			DESCRIPTION "Tone mappings plugins usable in Castor3D."
			PARENT_GROUP Plugins
		)
		cpack_add_component( LinearToneMapping
			DISPLAY_NAME "Linear ToneMapping"
			DESCRIPTION "Allows Linear Tone mapping."
			GROUP Plugins
			DEPENDS Castor3D
			INSTALL_TYPES Full
		)
		if( ${CASTOR_BUILD_TONEMAP_HAARM} )
			cpack_add_component( HaarmPeterDuikerToneMapping
				DISPLAY_NAME "Haarm Pieter Duiker ToneMapping"
				DESCRIPTION "Allows Haarm Pieter Duiker Tone mapping."
				GROUP Plugins
				DEPENDS Castor3D
				INSTALL_TYPES Full
			)
		endif()
		if( ${CASTOR_BUILD_TONEMAP_HEJL} )
			cpack_add_component( HejlBurgessDawsonToneMapping
				DISPLAY_NAME "Hejl Burgess Dawson ToneMapping"
				DESCRIPTION "Allows Haarm Peter Duiker Tone mapping."
				GROUP Plugins
				DEPENDS Castor3D
				INSTALL_TYPES Full
			)
		endif()
		if( ${CASTOR_BUILD_TONEMAP_REINHARD} )
			cpack_add_component( ReinhardToneMapping
				DISPLAY_NAME "Reinhard ToneMapping"
				DESCRIPTION "Allows Reinhard Tone mapping."
				GROUP Plugins
				DEPENDS Castor3D
				INSTALL_TYPES Full
			)
		endif()
	endif ()
endfunction( ToneMappingsSetup )
