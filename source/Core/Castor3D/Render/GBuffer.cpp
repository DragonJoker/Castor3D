#include "Castor3D/Render/GBuffer.hpp"

namespace castor3d
{
	GBufferBase::GBufferBase( RenderDevice const & device
		, castor::String name )
		: castor::Named{ std::move( name ) }
		, m_device{ device }
	{
	}

	TexturePtr GBufferBase::doCreateTexture( crg::ResourcesCache & resources
		, castor::String const & name
		, VkImageCreateFlags createFlags
		, VkExtent3D const & size
		, uint32_t layerCount
		, uint32_t mipLevels
		, VkFormat format
		, VkImageUsageFlags usageFlags
		, VkBorderColor const & borderColor )const
	{
		return std::make_shared< Texture >( m_device
			, resources
			, name
			, createFlags
			, size
			, layerCount
			, mipLevels
			, format
			, usageFlags
			, borderColor );
	}
}
