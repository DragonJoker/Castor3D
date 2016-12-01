#include "ParticleSystemImpl.hpp"

using namespace Castor;

namespace Castor3D
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
