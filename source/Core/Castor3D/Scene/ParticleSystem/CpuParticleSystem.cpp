#include "Castor3D/Scene/ParticleSystem/CpuParticleSystem.hpp"

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
	}

	void CpuParticleSystem::addParticleVariable( castor::String const & name, ParticleFormat type, castor::String const & defaultValue )
	{
		m_inputs.push_back( ParticleElementDeclaration{ name, 0u, type, m_inputs.stride() } );
	}
}
