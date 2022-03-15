#include "Castor3D/Buffer/UniformBufferPools.hpp"

#include "Castor3D/Render/RenderSystem.hpp"

#include <ashespp/Buffer/StagingBuffer.hpp>
#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Core/Device.hpp>

#include <algorithm>

CU_ImplementCUSmartPtr( castor3d, UniformBufferPools )

namespace castor3d
{
	UniformBufferPools::UniformBufferPools( RenderSystem & renderSystem
		, RenderDevice const & device )
		: castor::OwnedBy< RenderSystem >{ renderSystem }
		, m_pools
		{
			UniformBufferPool{ renderSystem, device, "Generic" },
		}
	{
	}

	UniformBufferPools::~UniformBufferPools()
	{
		for ( auto & pool : m_pools )
		{
			pool.cleanup();
		}
	}

	void UniformBufferPools::upload( ashes::CommandBuffer const & cb )const
	{
		for ( auto & pool : m_pools )
		{
			pool.upload( cb );
		}
	}
}
