/*
See LICENSE file in root folder
*/
#ifndef ___C3D_FireworksParticle_H___
#define ___C3D_FireworksParticle_H___

#include <Castor3D/Scene/ParticleSystem/CpuParticleSystem.hpp>
#include <Castor3D/Scene/ParticleSystem/ParticleEmitter.hpp>
#include <Castor3D/Scene/ParticleSystem/ParticleUpdater.hpp>

namespace fireworks
{
	class ParticleSystem
		: public c3d::CpuParticleSystem
	{
	public:
		explicit ParticleSystem( c3d::ParticleSystem & parent );

		static c3d::CpuParticleSystemUPtr create( c3d::ParticleSystem & parent );

	private:
		/**
		 *\copydoc		c3d::CpuParticleSystem::doInitialise
		 */
		bool doInitialise()override;
		/**
		 *\copydoc		c3d::CpuParticleSystem::doPackParticles
		 */
		void doPackParticles()override;

	public:
		static c3d::String const Type;
		static c3d::MbString const Name;
	};
}

#endif
