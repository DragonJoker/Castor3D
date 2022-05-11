option( CASTOR_BUILD_PLUGINS "Build Castor3D plugins" TRUE )

if ( ${CASTOR_BUILD_PLUGINS} )
	add_subdirectory( Importers )
	add_subdirectory( ToneMappings )
	add_subdirectory( PostEffects )
	add_subdirectory( Generic )
	add_subdirectory( Particles )
	add_subdirectory( Generators )
endif ()

macro( PluginsInit )
	if ( ${CASTOR_BUILD_PLUGINS} )
		ImportersInit()
		ToneMappingsInit()
		PostEffectsInit()
		GenericInit()
		ParticlesInit()
		GeneratorInit()
	endif ()
endmacro()

macro( PluginsBuild )
	if ( ${CASTOR_BUILD_PLUGINS} )
		ImportersBuild()
		ToneMappingsBuild()
		PostEffectsBuild()
		GenericBuild()
		ParticlesBuild()
		GeneratorBuild()
		set( CASTOR_INSTEXP_PLUGINS
			${CASTOR_INSTEXP_PLUGINS}
			PARENT_SCOPE
		)
		set( ADDITIONAL_DEPS_REL
			${ADDITIONAL_DEPS_REL}
			PARENT_SCOPE
		)
	endif ()
endmacro()

macro( PluginsSummary )
	if ( ${CASTOR_BUILD_PLUGINS} )
		ImportersSummary( ${msg} )
		ToneMappingsSummary( ${msg} )
		PostEffectsSummary( ${msg} )
		GenericSummary( ${msg} )
		ParticlesSummary( ${msg} )
		GeneratorSummary( ${msg} )
	endif ()
endmacro()

macro( PluginsSetup )
	if ( ${CASTOR_BUILD_PLUGINS} )
		ImportersSetup()
		ToneMappingsSetup()
		PostEffectsSetup()
		GenericSetup()
		ParticlesSetup()
		GeneratorSetup()
	endif ()
endmacro()

