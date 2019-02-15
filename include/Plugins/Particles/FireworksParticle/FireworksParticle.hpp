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
		explicit ParticleSystem( castor3d::ParticleSystem & parent );
		virtual ~ParticleSystem();
		static castor3d::CpuParticleSystemUPtr create( castor3d::ParticleSystem & parent );
		void emitParticle( float type
			, castor::Point3f const & position
			, castor::Point3f const & velocity
			, float age );
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

	public:
		static castor::String const Type;
		static castor::String const Name;

	private:
		uint32_t m_firstUnused{ 1u };
		castor3d::ParticleDeclaration::iterator m_type;
		castor3d::ParticleDeclaration::iterator m_position;
		castor3d::ParticleDeclaration::iterator m_velocity;
		castor3d::ParticleDeclaration::iterator m_age;
	};
}

#endif
