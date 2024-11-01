option( CASTOR_BUILD_GENERIC_TOONMATERIAL "Build Toon Material generic plugin" ON )
option( CASTOR_BUILD_GENERIC_ANISOTROPICMATERIAL "Build Anisotropic Material plugin" ON )
option( CASTOR_BUILD_GENERIC_FFTOCEANRENDERING "Build FFT ocean rendering plugin" ON )
option( CASTOR_BUILD_GENERIC_ATMOSPHERESCATTERING "Build Atmosphere scattering plugin" ON )
option( CASTOR_BUILD_GENERIC_WATERMATERIAL "Build Water Material plugin" ON )
option( CASTOR_BUILD_GENERIC_WAVESRENDERING "Build waves rendering plugin" ON )
option( CASTOR_BUILD_GENERIC_DISNEYBRDF "Build Disney BRDF plugin" ON )

function( GenericInit )
	set( AtmS "no (Not wanted)" PARENT_SCOPE )
	set( FftO "no (Not wanted)" PARENT_SCOPE )
	set( Toon "no (Not wanted)" PARENT_SCOPE )
	set( WatM "no (Not wanted)" PARENT_SCOPE )
	set( WatR "no (Not wanted)" PARENT_SCOPE )
	set( Anis "no (Not wanted)" PARENT_SCOPE )
	set( Disn "no (Not wanted)" PARENT_SCOPE )

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

	if( ${CASTOR_BUILD_GENERIC_ANISOTROPICMATERIAL} )
		set( Castor3D_MATERIAL_DIRS
			${Castor3D_MATERIAL_DIRS}
			${CASTOR_SOURCE_DIR}/source/Plugins/Generic/AnisotropicMaterial
		)
		set( Castor3D_MATERIAL_DEPS
			${Castor3D_MATERIAL_DEPS}
			castor::AnisotropicMaterial
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

		if( ${CASTOR_BUILD_GENERIC_ATMOSPHERESCATTERING} )
			set( Build ${AtmS} )
			add_subdirectory( Plugins/Generic/AtmosphereScattering )
			set( AtmS ${Build} PARENT_SCOPE )
		endif()

		if( ${CASTOR_BUILD_GENERIC_FFTOCEANRENDERING} )
			set( Build ${FftO} )
			add_subdirectory( Plugins/Generic/FFTOceanRendering )
			set( FftO ${Build} PARENT_SCOPE )
		endif()

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

		if( ${CASTOR_BUILD_GENERIC_WAVESRENDERING} )
			set( Build ${WatR} )
			add_subdirectory( Plugins/Generic/WavesRendering )
			set( WatR ${Build} PARENT_SCOPE )
		endif()

		if( ${CASTOR_BUILD_GENERIC_ANISOTROPICMATERIAL} )
			set( Build ${Anis} )
			add_subdirectory( Plugins/Generic/AnisotropicMaterial )
			set( Anis ${Build} PARENT_SCOPE )
		endif()

		if( ${CASTOR_BUILD_GENERIC_DISNEYBRDF} )
			set( Build ${Disn} )
			add_subdirectory( Plugins/Generic/DisneyBRDF )
			set( Disn ${Build} PARENT_SCOPE )
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
		if( ${CASTOR_BUILD_GENERIC_ATMOSPHERESCATTERING} )
			set( msg_tmp "${msg_tmp}\n    AtmosphereScattering ${AtmS}" )
		endif ()
		if( ${CASTOR_BUILD_GENERIC_FFTOCEANRENDERING} )
			set( msg_tmp "${msg_tmp}\n    FFTOceanRendering    ${FftO}" )
		endif ()
		if( ${CASTOR_BUILD_GENERIC_TOONMATERIAL} )
			set( msg_tmp "${msg_tmp}\n    ToonMaterial         ${Toon}" )
		endif ()
		if( ${CASTOR_BUILD_GENERIC_WATERMATERIAL} )
			set( msg_tmp "${msg_tmp}\n    WaterMaterial        ${WatM}" )
		endif ()
		if( ${CASTOR_BUILD_GENERIC_WAVESRENDERING} )
			set( msg_tmp "${msg_tmp}\n    WavesRendering       ${WatR}" )
		endif ()
		if( ${CASTOR_BUILD_GENERIC_ANISOTROPICMATERIAL} )
			set( msg_tmp "${msg_tmp}\n    AnisotropicMaterial  ${Anis}" )
		endif ()
		if( ${CASTOR_BUILD_GENERIC_DISNEYBRDF} )
			set( msg_tmp "${msg_tmp}\n    DisneyBRDF           ${Disn}" )
		endif ()
		set( msg "${msg}${msg_tmp}" PARENT_SCOPE )
	endif ()
endfunction( GenericSummary )

function( GenericSetup )
	if ( ${CASTOR_BUILDGRP_GENERIC} )
		if( ${CASTOR_BUILD_GENERIC_ATMOSPHERESCATTERING} )
			cpack_add_component( AtmosphereScattering
				DISPLAY_NAME "AtmosphereScattering"
				DESCRIPTION "Allows Atmosphere Scattering in Castor3D."
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
				DESCRIPTION "Defines a water material in Castor3D."
				GROUP Plugins
				DEPENDS Castor3D
			)
		endif()
		if( ${CASTOR_BUILD_GENERIC_WAVESRENDERING} )
			cpack_add_component( WavesRendering
				DISPLAY_NAME "WavesRendering"
				DESCRIPTION "Allows waves rendering in Castor3D."
				GROUP Plugins
				DEPENDS Castor3D
			)
		endif()
		if( ${CASTOR_BUILD_GENERIC_ANISOTROPICMATERIAL} )
			cpack_add_component( AnisotropicMaterial
				DISPLAY_NAME "AnisotropicMaterial"
				DESCRIPTION "Defines an anisotropic material in Castor3D."
				GROUP Plugins
				DEPENDS Castor3D
			)
		endif()
		if( ${CASTOR_BUILD_GENERIC_DISNEYBRDF} )
			cpack_add_component( DisneyBRDF
				DISPLAY_NAME "DisneyBRDF"
				DESCRIPTION "Defines an implementation of Disney's BRDF in Castor3D."
				GROUP Plugins
				DEPENDS Castor3D
			)
		endif()
	endif ()
endfunction( GenericSetup )
