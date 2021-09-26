#include "Castor3D/Scene/ParticleSystem/ParticleUpdater.hpp"

CU_ImplementCUSmartPtr( castor3d, ParticleUpdater )

namespace castor3d
{
	ParticleUpdater::ParticleUpdater( ParticleSystem const & system
		, ParticleDeclaration const & inputs
		, ParticleEmitterArray & emitters )
		: m_system{ system }
		, m_inputs{ inputs }
		, m_emitters{ emitters }
	{
	}

	void ParticleUpdater::update( castor::Milliseconds const & time
		, Particle & particle )
	{
	}
}
