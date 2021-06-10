#include "Castor3D/Render/GBuffer.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/SamplerCache.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <RenderGraph/FrameGraph.hpp>

using namespace castor;

namespace castor3d
{
	GBufferBase::GBufferBase( RenderDevice const & device
		, castor::String name )
		: castor::Named{ std::move( name ) }
		, m_device{ device }
	{
	}

	Texture GBufferBase::doCreateTexture( crg::ResourceHandler & handler
		, castor::String const & name
		, VkImageCreateFlags createFlags
		, VkExtent3D const & size
		, uint32_t layerCount
		, uint32_t mipLevels
		, VkFormat format
		, VkImageUsageFlags usageFlags
		, VkBorderColor const & borderColor )const
	{
		return Texture{ m_device
			, handler
			, name
			, createFlags
			, size
			, layerCount
			, mipLevels
			, format
			, usageFlags
			, borderColor };
	}
}
