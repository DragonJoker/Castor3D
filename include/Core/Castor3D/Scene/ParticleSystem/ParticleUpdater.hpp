/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ParticleUpdater_H___
#define ___C3D_ParticleUpdater_H___

#include "ParticleModule.hpp"

namespace castor3d
{
	class ParticleUpdater
	{
	public:
		C3D_API ParticleUpdater( ParticleSystem const & system
			, ParticleDeclaration const & inputs
			, ParticleEmitterArray & emitters );
		C3D_API virtual ~ParticleUpdater() = default;
		C3D_API virtual void update( castor::Milliseconds const & time
			, Particle & particle );

	protected:
		ParticleSystem const & m_system;
		ParticleDeclaration const & m_inputs;
		ParticleEmitterArray & m_emitters;
	};
}

#endif
