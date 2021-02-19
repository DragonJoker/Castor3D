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
		/**
		 *\~english
		 *\brief		Updates a particle.
		 *\param[in]	time		The time elapsed since last update.
		 *\param[in]	particle	The particle.
		 *\~french
		 *\brief		Met à jour une particle.
		 *\param[in]	time		Le temps écoulé depuis la denière mise à jour.
		 *\param[in]	particle	La particule.
		 */
		C3D_API virtual void update( castor::Milliseconds const & time
			, Particle & particle );

	protected:
		ParticleSystem const & m_system;
		ParticleDeclaration const & m_inputs;
		ParticleEmitterArray & m_emitters;
	};
}

#endif
