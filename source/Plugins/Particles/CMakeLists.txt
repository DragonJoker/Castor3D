option( CASTOR_BUILDGRP_PARTICLES "Build Castor3D Particles plugins" TRUE )
option( CASTOR_BUILD_PARTICLE_FIREWORKS "Build Fireworks Particle plugin" TRUE )

function( ParticlesInit )
	set( Frwks "no (Not wanted)" PARENT_SCOPE )
endfunction( ParticlesInit )

function( ParticlesBuild )
	if ( ${CASTOR_BUILDGRP_PARTICLES} )
		set( ParticlesIncludes
			${CASTOR_SOURCE_DIR}/include/Core
			${CASTOR_BINARY_DIR}/include/Core
			${CASTOR_SOURCE_DIR}/source/Plugins/Particles
			${CASTOR_BINARY_DIR}/source/Plugins/Particles
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
		set( Castor3DBinsDependencies
			${Castor3DBinsDependencies}
			PARENT_SCOPE
		)
		set( CASTOR_INSTEXP_LIST
			${CASTOR_INSTEXP_LIST}
			PARENT_SCOPE
		)
	endif ()
endfunction( ParticlesBuild )

function( ParticlesSummary msg )
	if ( ${CASTOR_BUILDGRP_PARTICLES} )
		set( msg_tmp "\n\n  Particle plugins:" )
		if( ${CASTOR_BUILD_PARTICLE_FIREWORKS} )
			set( msg_tmp "${msg_tmp}\n    Fireworks            ${Frwks}" )
		endif ()
		set( msg "${msg}${msg_tmp}" PARENT_SCOPE )
	endif ()
endfunction( ParticlesSummary )

function( ParticlesSetup )
	if ( ${CASTOR_BUILDGRP_PARTICLES} )
		if( ${CASTOR_BUILD_PARTICLE_FIREWORKS} )
			cpack_add_component( FireworksParticle
				DISPLAY_NAME "FireworksParticle"
				DESCRIPTION "Allows HDR Fireworks particles."
				GROUP Plugins
				DEPENDS Castor3D
				INSTALL_TYPES Developer Full
			)
		endif()
	endif ()
endfunction( ParticlesSetup )
