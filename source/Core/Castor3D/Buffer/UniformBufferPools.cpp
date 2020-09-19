#include "Castor3D/Buffer/UniformBufferPools.hpp"

#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"

#include <ashespp/Buffer/StagingBuffer.hpp>
#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Core/Device.hpp>

#include <algorithm>

namespace castor3d
{
	UniformBufferPools::UniformBufferPools( RenderSystem & renderSystem )
		: castor::OwnedBy< RenderSystem >{ renderSystem }
		, m_pools
		{
			UniformBufferPool{ renderSystem, "Matrix" },
			UniformBufferPool{ renderSystem, "ModelMatrix" },
			UniformBufferPool{ renderSystem, "Model" },
			UniformBufferPool{ renderSystem, "Billboard" },
			UniformBufferPool{ renderSystem, "Skinning" },
			UniformBufferPool{ renderSystem, "Morphing" },
			UniformBufferPool{ renderSystem, "Picking" },
			UniformBufferPool{ renderSystem, "Textures" },
			UniformBufferPool{ renderSystem, "ShadowMap" },
			UniformBufferPool{ renderSystem, "HDRConfig" },
			UniformBufferPool{ renderSystem, "RSMConfig" },
			UniformBufferPool{ renderSystem, "Generic" },
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
