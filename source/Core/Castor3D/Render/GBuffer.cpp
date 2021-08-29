#include "Castor3D/Render/GBuffer.hpp"

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
