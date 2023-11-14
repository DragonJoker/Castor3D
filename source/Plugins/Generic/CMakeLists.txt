option( CASTOR_BUILDGRP_GENERIC "Build Castor3D Generic plugins" ON )
option( CASTOR_BUILD_GENERIC_TOONMATERIAL "Build Toon Material generic plugin" ON )
option( CASTOR_BUILD_GENERIC_WATERMATERIAL "Build Water Material plugin" ON )
option( CASTOR_BUILD_GENERIC_WATERRENDERING "Build water rendering plugin" ON )
option( CASTOR_BUILD_GENERIC_OCEANRENDERING "Build ocean rendering plugin" ON )
option( CASTOR_BUILD_GENERIC_FFTOCEANRENDERING "Build FFT ocean rendering plugin" ON )
option( CASTOR_BUILD_GENERIC_ATMOSPHERESCATTERING "Build Atmosphere scattering plugin" ON )

function( GenericInit )
	set( Toon "no (Not wanted)" PARENT_SCOPE )
	set( WatM "no (Not wanted)" PARENT_SCOPE )
	set( Watr "no (Not wanted)" PARENT_SCOPE )
	set( Ocen "no (Not wanted)" PARENT_SCOPE )
	set( FftO "no (Not wanted)" PARENT_SCOPE )
	set( AtmS "no (Not wanted)" PARENT_SCOPE )

	if( ${CASTOR_BUILD_GENERIC_TOONMATERIAL} )
		set( Castor3D_MATERIAL_DIRS
			${Castor3D_MATERIAL_DIRS}
			${CASTOR_SOURCE_DIR}/source/Plugins/Generic/ToonMaterial
		)
		set( Castor3D_MATERIAL_DEPS
			${Castor3D_MATERIAL_DEPS}
			castor::ToonMaterial
		)
	endif()

	if( ${CASTOR_BUILD_GENERIC_WATERMATERIAL} )
		set( Castor3D_MATERIAL_DIRS
			${Castor3D_MATERIAL_DIRS}
			${CASTOR_SOURCE_DIR}/source/Plugins/Generic/WaterMaterial
		)
		set( Castor3D_MATERIAL_DEPS
			${Castor3D_MATERIAL_DEPS}
			castor::WaterMaterial
		)
	endif()

	set( Castor3D_MATERIAL_DIRS
		${Castor3D_MATERIAL_DIRS}
		PARENT_SCOPE
	)
	set( Castor3D_MATERIAL_DEPS
		${Castor3D_MATERIAL_DEPS}
		PARENT_SCOPE
	)
endfunction( GenericInit )

function( GenericBuild )
	if ( ${CASTOR_BUILDGRP_GENERIC} )
		set( GenericIncludes
			${CASTOR_SOURCE_DIR}/include/Core
			${CASTOR_BINARY_DIR}/include/Core
			${CASTOR_SOURCE_DIR}/source/Plugins/Generic
			${CASTOR_BINARY_DIR}/source/Plugins/Generic
		)

		if( ${CASTOR_BUILD_GENERIC_TOONMATERIAL} )
			set( Build ${Toon} )
			add_subdirectory( Plugins/Generic/ToonMaterial )
			set( Toon ${Build} PARENT_SCOPE )
		endif()

		if( ${CASTOR_BUILD_GENERIC_WATERMATERIAL} )
			set( Build ${WatM} )
			add_subdirectory( Plugins/Generic/WaterMaterial )
			set( WatM ${Build} PARENT_SCOPE )
		endif()

		if( ${CASTOR_BUILD_GENERIC_WATERRENDERING} )
			set( Build ${Watr} )
			add_subdirectory( Plugins/Generic/WaterRendering )
			set( Watr ${Build} PARENT_SCOPE )
		endif()

		if( ${CASTOR_BUILD_GENERIC_OCEANRENDERING} )
			set( Build ${Ocen} )
			add_subdirectory( Plugins/Generic/OceanRendering )
			set( Ocen ${Build} PARENT_SCOPE )
		endif()

		if( ${CASTOR_BUILD_GENERIC_FFTOCEANRENDERING} )
			set( Build ${FftO} )
			add_subdirectory( Plugins/Generic/FFTOceanRendering )
			set( FftO ${Build} PARENT_SCOPE )
		endif()

		if( ${CASTOR_BUILD_GENERIC_ATMOSPHERESCATTERING} )
			set( Build ${AtmS} )
			add_subdirectory( Plugins/Generic/AtmosphereScattering )
			set( AtmS ${Build} PARENT_SCOPE )
		endif()

		set( CastorMinLibraries
			${CastorMinLibraries}
			PARENT_SCOPE
		)
		set( Castor3DBinsDependencies
			${Castor3DBinsDependencies}
			PARENT_SCOPE
		)
		set( Castor3D_MATERIAL_DIRS
			${Castor3D_MATERIAL_DIRS}
			PARENT_SCOPE
		)
		set( CASTOR_INSTEXP_LIST
			${CASTOR_INSTEXP_LIST}
			PARENT_SCOPE
		)
		set( CASTOR_INSTEXP_PLUGINS
			${CASTOR_INSTEXP_PLUGINS}
			PARENT_SCOPE
		)
	endif ()
endfunction( GenericBuild )

function( GenericSummary msg )
	if ( ${CASTOR_BUILDGRP_GENERIC} )
		set( msg_tmp "\n\n  Generic plugins:" )
		if( ${CASTOR_BUILD_GENERIC_TOONMATERIAL} )
			set( msg_tmp "${msg_tmp}\n    ToonMaterial         ${Toon}" )
		endif ()
		if( ${CASTOR_BUILD_GENERIC_WATERMATERIAL} )
			set( msg_tmp "${msg_tmp}\n    WaterMaterial        ${WatM}" )
		endif ()
		if( ${CASTOR_BUILD_GENERIC_WATERRENDERING} )
			set( msg_tmp "${msg_tmp}\n    WaterRendering       ${Watr}" )
		endif ()
		if( ${CASTOR_BUILD_GENERIC_OCEANRENDERING} )
			set( msg_tmp "${msg_tmp}\n    OceanRendering       ${Ocen}" )
		endif ()
		if( ${CASTOR_BUILD_GENERIC_FFTOCEANRENDERING} )
			set( msg_tmp "${msg_tmp}\n    FFTOceanRendering    ${FftO}" )
		endif ()
		if( ${CASTOR_BUILD_GENERIC_ATMOSPHERESCATTERING} )
			set( msg_tmp "${msg_tmp}\n    AtmosphereScattering ${AtmS}" )
		endif ()
		set( msg "${msg}${msg_tmp}" PARENT_SCOPE )
	endif ()
endfunction( GenericSummary )

function( GenericSetup )
	if ( ${CASTOR_BUILDGRP_GENERIC} )
		if( ${CASTOR_BUILD_GENERIC_TOONMATERIAL} )
			cpack_add_component( ToonMaterial
				DISPLAY_NAME "ToonMaterial"
				DESCRIPTION "Defines a toon material in Castor3D."
				GROUP Plugins
				DEPENDS Castor3D
			)
		endif()
		if( ${CASTOR_BUILD_GENERIC_WATERMATERIAL} )
			cpack_add_component( WaterMaterial
				DISPLAY_NAME "WaterMaterial"
				DESCRIPTION "Defines & water material in Castor3D."
				GROUP Plugins
				DEPENDS Castor3D
			)
		endif()
		if( ${CASTOR_BUILD_GENERIC_WATERRENDERING} )
			cpack_add_component( WaterRendering
				DISPLAY_NAME "WaterRendering"
				DESCRIPTION "Allows water rendering in Castor3D."
				GROUP Plugins
				DEPENDS Castor3D
			)
		endif()
		if( ${CASTOR_BUILD_GENERIC_OCEANRENDERING} )
			cpack_add_component( OceanRendering
				DISPLAY_NAME "OceanRendering"
				DESCRIPTION "Allows ocean rendering in Castor3D."
				GROUP Plugins
				DEPENDS Castor3D
			)
		endif()
		if( ${CASTOR_BUILD_GENERIC_FFTOCEANRENDERING} )
			cpack_add_component( FFTOceanRendering
				DISPLAY_NAME "FFTOceanRendering"
				DESCRIPTION "Allows FFT ocean rendering in Castor3D."
				GROUP Plugins
				DEPENDS Castor3D
			)
		endif()
		if( ${CASTOR_BUILD_GENERIC_ATMOSPHERESCATTERING} )
			cpack_add_component( AtmosphereScattering
				DISPLAY_NAME "AtmosphereScattering"
				DESCRIPTION "Allows Atmosphere Scattering in Castor3D."
				GROUP Plugins
				DEPENDS Castor3D
			)
		endif()
	endif ()
endfunction( GenericSetup )
