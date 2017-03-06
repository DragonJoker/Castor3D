#include "CpuParticleSystem.hpp"

#include "ParticleSystem.hpp"

using namespace Castor;

namespace Castor3D
{
	CpuParticleSystem::CpuParticleSystem( ParticleSystem & p_parent )
		: ParticleSystemImpl{ ParticleSystemImpl::Type::eCpu, p_parent }
	{
	}

	CpuParticleSystem::~CpuParticleSystem()
	{
	}

	bool CpuParticleSystem::Initialise()
	{
		m_particles.reserve( m_parent.GetMaxParticlesCount() );
		auto & l_defaultValues = m_parent.GetDefaultValues();

		for ( auto i = 0u; i < m_parent.GetMaxParticlesCount(); ++i )
		{
			m_particles.emplace_back( m_inputs, l_defaultValues );
		}

		return true;
	}

	void CpuParticleSystem::Cleanup()
	{
		m_particles.clear();
	}

	void CpuParticleSystem::AddParticleVariable( Castor::String const & p_name, ElementType p_type, Castor::String const & p_defaultValue )
	{
		m_inputs.push_back( BufferElementDeclaration{ p_name, 0u, p_type, m_inputs.stride() } );
	}
}
