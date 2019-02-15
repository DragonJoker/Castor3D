#include "Castor3D/Scene/ParticleSystem/ParticleSystemImpl.hpp"

using namespace castor;

namespace castor3d
{
	ParticleSystemImpl::ParticleSystemImpl( Type p_type, ParticleSystem & p_parent )
		: m_parent{ p_parent }
		, m_type{ p_type }
	{
	}

	ParticleSystemImpl::~ParticleSystemImpl()
	{
	}
}
