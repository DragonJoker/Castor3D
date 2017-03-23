option( CASTOR_BUILDGRP_PARTICLES "Build Castor3D Particles plugins" TRUE )
option( CASTOR_BUILD_PARTICLE_FIREWORKS "Build Fireworks Particle plugin" TRUE )

function( ParticlesInit )
	set( Frwks "no (Not wanted)" PARENT_SCOPE )
endfunction( ParticlesInit )

function( ParticlesBuild )
	if ( ${CASTOR_BUILDGRP_PARTICLES} )
		set( ParticlesLibraries ${CastorMinLibraries} )

		set( ParticlesIncludes
			${CMAKE_SOURCE_DIR}/Core/CastorUtils/Src
			${CMAKE_SOURCE_DIR}/Core/Castor3D/Src
			${CMAKE_BINARY_DIR}/Core/CastorUtils/Src
			${CMAKE_SOURCE_DIR}/Core/GlslWriter/Src
		)

		if( ${CASTOR_BUILD_PARTICLE_FIREWORKS} )
			set( Build ${Frwks} )
			add_subdirectory( Plugins/Particles/FireworksParticle )
			set( Frwks ${Build} PARENT_SCOPE )
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
endfunction( ParticlesBuild )

function( ParticlesSummary msg msgtest )
	if ( ${CASTOR_BUILDGRP_PARTICLES} )
		set( msg_tmp "\n\nParticles :" )
		if( ${CASTOR_BUILD_PARTICLE_FIREWORKS} )
			set( msg_tmp "${msg_tmp}\n    Fireworks           ${Frwks}" )
		endif ()
		set( msg "${msg}${msg_tmp}" PARENT_SCOPE )
	endif ()
endfunction( ParticlesSummary )

function( ParticlesSetup )
	if ( ${CASTOR_BUILDGRP_PARTICLES} )
		cpack_add_component_group( Particles
			DISPLAY_NAME "Particles"
			DESCRIPTION "Particle plug-ins usable in Castor3D."
			PARENT_GROUP Plugins
		)
		if( ${CASTOR_BUILD_PARTICLE_FIREWORKS} )
			cpack_add_component( FireworksParticle
				DISPLAY_NAME "FireworksParticle"
				DESCRIPTION "Allows HDR Fireworks particles."
				GROUP Particles
				DEPENDS Castor3D
				INSTALL_TYPES Developer Full
			)
		endif()
	endif ()
endfunction( ParticlesSetup )
