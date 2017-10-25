/*
See LICENSE file in root folder
*/
#ifndef ___C3D_FireworksParticle_H___
#define ___C3D_FireworksParticle_H___

#include <Scene/ParticleSystem/CpuParticleSystem.hpp>

namespace Fireworks
{
	class ParticleSystem
		: public castor3d::CpuParticleSystem
	{
	public:
		explicit ParticleSystem( castor3d::ParticleSystem & p_parent );
		virtual ~ParticleSystem();
		static castor3d::CpuParticleSystemUPtr create( castor3d::ParticleSystem & p_parent );
		void emitParticle( float p_type, castor::Point3f const & p_position, castor::Point3f const & p_velocity, float p_age );
		/**
		 *\copydoc		castor3d::CpuParticleSystem::update
		 */
		uint32_t update( castor::Milliseconds const & p_time
			, castor::Milliseconds const & p_totalTime )override;

	private:
		/**
		 *\copydoc		castor3d::CpuParticleSystem::doInitialise
		 */
		bool doInitialise()override;
		/**
		 *\copydoc		castor3d::CpuParticleSystem::doCleanup
		 */
		void doCleanup()override;

	public:
		static castor::String const Type;
		static castor::String const Name;

	private:
		uint32_t m_firstUnused{ 1u };
	};
}

#endif
