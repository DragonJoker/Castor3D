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
		: public castor3d::CpuParticleSystem
	{
	public:
		explicit ParticleSystem( castor3d::ParticleSystem & parent );
		virtual ~ParticleSystem();
		static castor3d::CpuParticleSystemUPtr create( castor3d::ParticleSystem & parent );
		/**
		 *\copydoc		castor3d::CpuParticleSystem::update
		 */
		uint32_t update( castor3d::RenderPassTimer & timer
			, castor::Milliseconds const & time
			, castor::Milliseconds const & totalTime
			, uint32_t index )override;

	private:
		/**
		 *\copydoc		castor3d::CpuParticleSystem::doInitialise
		 */
		bool doInitialise()override;
		/**
		 *\copydoc		castor3d::CpuParticleSystem::doCleanup
		 */
		void doCleanup()override;
		/**
		 *\copydoc		castor3d::CpuParticleSystem::doOnEmit
		 */
		void doOnEmit( castor3d::Particle const & particle )override;

	public:
		static castor::String const Type;
		static castor::String const Name;

	private:
		uint32_t m_firstUnused{ 1u };
	};
}

#endif
