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
	//*********************************************************************************************

	namespace
	{
		VkFormat getDepthFormat( RenderDevice const & device
			, VkFormat format )
		{
			std::vector< VkFormat > depthFormats
			{
				format,
				VK_FORMAT_D24_UNORM_S8_UINT,
				VK_FORMAT_D16_UNORM_S8_UINT,
			};
			return device.selectSuitableFormat( depthFormats
				, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT );
		}

		VkFormat getFormat( RenderDevice const & device
			, VkFormat format )
		{
			return ashes::isDepthOrStencilFormat( format )
				? getDepthFormat( device, format )
				: format;
		}
	}

	//*********************************************************************************************

	Texture GBufferBase::doCreateTexture( crg::FrameGraph & graph
		, castor::String const & name
		, VkImageCreateFlags createFlags
		, VkExtent3D const & size
		, uint32_t layerCount
		, uint32_t mipLevels
		, VkFormat format
		, VkImageUsageFlags usageFlags
		, VkBorderColor const & borderColor )const
	{
		mipLevels = std::max( 1u, mipLevels );
		layerCount = ( size.depth > 1u ? 1u : layerCount );
		Texture result;
		result.image = graph.createImage( crg::ImageData{ name
			, ( createFlags
				| ( size.depth > 1u
					? VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT
					: 0u ) )
			, ( size.depth > 1u
				? VK_IMAGE_TYPE_3D
				: VK_IMAGE_TYPE_2D )
			, getFormat( m_device, format )
			, size
			, ( usageFlags
				| ( mipLevels > 1u
					? ( VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT )
					: 0u ) )
			, mipLevels
			, layerCount } );
		result.wholeView = graph.createView( crg::ImageViewData{ name + "Whole"
			, result.image
			, 0u
			, ( size.depth > 1u
				? VK_IMAGE_VIEW_TYPE_3D
				: ( layerCount > 1u 
					? VK_IMAGE_VIEW_TYPE_2D_ARRAY
					: VK_IMAGE_VIEW_TYPE_2D ) )
			, format
			, { ashes::getAspectMask( format ), 0u, 1u, 0u, layerCount } } );
		auto sliceLayerCount = std::max( size.depth, layerCount );
		auto data = *result.wholeView.data;

		for ( uint32_t index = 0u; index < sliceLayerCount; ++index )
		{
			result.subViews.push_back( graph.createView( crg::ImageViewData{ name + "Sub" + std::to_string( index )
				, result.image
				, 0u
				, VK_IMAGE_VIEW_TYPE_2D
				, format
				, { ashes::getAspectMask( format ), 0u, 1u, index, 1u } } ) );
		}

		auto & engine = *m_device.renderSystem.getEngine();
		SamplerSPtr sampler;

		if ( engine.getSamplerCache().has( name ) )
		{
			sampler = engine.getSamplerCache().find( name );
		}
		else
		{
			sampler = engine.getSamplerCache().add( name );
			sampler->setMinFilter( VK_FILTER_LINEAR );
			sampler->setMagFilter( VK_FILTER_LINEAR );
			sampler->setMipFilter( VK_SAMPLER_MIPMAP_MODE_LINEAR );
			sampler->setWrapS( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
			sampler->setWrapT( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
			sampler->setWrapR( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
			sampler->setBorderColour( borderColor );
			sampler->initialise( m_device );
		}

		result.sampler = &sampler->getSampler();
		return result;
	}

	//*********************************************************************************************
}
