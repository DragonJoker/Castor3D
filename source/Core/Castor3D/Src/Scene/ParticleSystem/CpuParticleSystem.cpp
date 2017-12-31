#include "CpuParticleSystem.hpp"

#include "ParticleSystem.hpp"

using namespace castor;

namespace castor3d
{
	CpuParticleSystem::CpuParticleSystem( ParticleSystem & p_parent )
		: ParticleSystemImpl{ ParticleSystemImpl::Type::eCpu, p_parent }
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

		return true;
	}

	void CpuParticleSystem::cleanup()
	{
		m_particles.clear();
	}

	void CpuParticleSystem::addParticleVariable( castor::String const & p_name, ElementType p_type, castor::String const & p_defaultValue )
	{
		m_inputs.push_back( BufferElementDeclaration{ p_name, 0u, p_type, m_inputs.stride() } );
	}
}
