#include "Castor3D/Scene/ParticleSystem/ParticleSystemImpl.hpp"

#include "Castor3D/Scene/ParticleSystem/ParticleSystem.hpp"

namespace c3d
{
	ParticleSystemImpl::ParticleSystemImpl( Type type, ParticleSystem & parent )
		: m_parent{ parent }
		, m_type{ type }
	{
	}
}
