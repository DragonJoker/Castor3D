#include "Castor3D/Scene/ParticleSystem/ParticleUpdater.hpp"

CU_ImplementSmartPtr( c3d, ParticleUpdater )

namespace c3d
{
	ParticleUpdater::ParticleUpdater( ParticleSystem const & system
		, ParticleDeclaration const & inputs
		, ParticleEmitterArray & emitters )
		: m_system{ system }
		, m_inputs{ inputs }
		, m_emitters{ emitters }
	{
	}

	void ParticleUpdater::update( Milliseconds const &
		, Particle & )
	{
	}
}
