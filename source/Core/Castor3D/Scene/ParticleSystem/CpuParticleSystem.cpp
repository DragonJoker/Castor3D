#include "Castor3D/Scene/ParticleSystem/CpuParticleSystem.hpp"

#include "Castor3D/Scene/ParticleSystem/ParticleEmitter.hpp"
#include "Castor3D/Scene/ParticleSystem/ParticleUpdater.hpp"
#include "Castor3D/Scene/ParticleSystem/ParticleSystem.hpp"

using namespace castor;

namespace castor3d
{
	CpuParticleSystem::CpuParticleSystem( ParticleSystem & parent )
		: ParticleSystemImpl{ ParticleSystemImpl::Type::eCpu, parent }
	{
	}

	CpuParticleSystem::~CpuParticleSystem()
	{
	}

	bool CpuParticleSystem::initialise()
	{
		m_particles.reserve( m_parent.getMaxParticlesCount() );
		auto & defaultValues = m_parent.getDefaultValues();

		for ( auto i = 0u; i < m_parent.getMaxParticlesCount(); ++i )
		{
			m_particles.emplace_back( m_inputs, defaultValues );
		}

		return doInitialise();
	}

	void CpuParticleSystem::cleanup()
	{
		doCleanup();
		m_particles.clear();
		m_emitters.clear();
		m_updaters.clear();
	}

	void CpuParticleSystem::addParticleVariable( castor::String const & name, ParticleFormat type, castor::String const & defaultValue )
	{
		m_inputs.push_back( ParticleElementDeclaration{ name, 0u, type, m_inputs.stride() } );
	}

	void CpuParticleSystem::onEmit( Particle const & particle )
	{
		doOnEmit( particle );
	}

	ParticleEmitter * CpuParticleSystem::addEmitter( ParticleEmitterUPtr emitter )
	{
		m_emitters.emplace_back( std::move( emitter ) );
		auto result = m_emitters.back().get();

		if ( result )
		{
			m_onEmits.emplace_back( result->onEmit.connect( [this]( castor3d::Particle const & particle )
				{
					onEmit( particle );
				} ) );
		}

		return result;
	}

	ParticleUpdater * CpuParticleSystem::addUpdater( ParticleUpdaterUPtr updater )
	{
		m_updaters.emplace_back( std::move( updater ) );
		return m_updaters.back().get();
	}
}
