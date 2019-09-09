#include "Castor3D/Scene/ParticleSystem/ParticleSystemImpl.hpp"

#include "Castor3D/Scene/ParticleSystem/ParticleSystem.hpp"

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

	RenderDevice const & getCurrentRenderDevice( ParticleSystem const & object )
	{
		return getCurrentRenderDevice( static_cast< MovableObject const & >( object ) );
	}
}
