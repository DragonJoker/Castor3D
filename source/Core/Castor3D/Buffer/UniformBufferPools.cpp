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
			UniformBufferPoolBase{ renderSystem, "Matrix" },
			UniformBufferPoolBase{ renderSystem, "ModelMatrix" },
			UniformBufferPoolBase{ renderSystem, "Model" },
			UniformBufferPoolBase{ renderSystem, "Billboard" },
			UniformBufferPoolBase{ renderSystem, "Skinning" },
			UniformBufferPoolBase{ renderSystem, "Morphing" },
			UniformBufferPoolBase{ renderSystem, "Picking" },
			UniformBufferPoolBase{ renderSystem, "Textures" },
			UniformBufferPoolBase{ renderSystem, "ShadowMap" },
			UniformBufferPoolBase{ renderSystem, "HDRConfig" },
			UniformBufferPoolBase{ renderSystem, "RSMConfig" },
			UniformBufferPoolBase{ renderSystem, "Generic" },
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
