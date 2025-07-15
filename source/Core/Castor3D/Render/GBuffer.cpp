#include "Castor3D/Render/GBuffer.hpp"

namespace c3d
{
	VkFormatFeatureFlags getFeatureFlags( ImageUsageFlags flags )
	{
		VkFormatFeatureFlags result{};

		if ( ashes::checkFlag( flags, ImageUsageFlags::eTransferSrc ) )
		{
			result |= VK_FORMAT_FEATURE_TRANSFER_SRC_BIT;
		}

		if ( ashes::checkFlag( flags, ImageUsageFlags::eTransferDst ) )
		{
			result |= VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
		}

		if ( ashes::checkFlag( flags, ImageUsageFlags::eSampled ) )
		{
			result |= VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
		}

		if ( ashes::checkFlag( flags, ImageUsageFlags::eStorage ) )
		{
			result |= VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
		}

		if ( ashes::checkFlag( flags, ImageUsageFlags::eColorAttachment ) )
		{
			result |= VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
		}
		else if ( ashes::checkFlag( flags, ImageUsageFlags::eDepthStencilAttachment ) )
		{
			result |= VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
		}
		else if ( ashes::checkFlag( flags, ImageUsageFlags::eInputAttachment ) )
		{
			result |= VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
		}

#ifdef VK_ENABLE_BETA_EXTENSIONS
		if ( ashes::checkFlag( flags, ImageUsageFlags::eVideoDecodeDst ) )
		{
			result |= VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR;
		}
#endif
#ifdef VK_ENABLE_BETA_EXTENSIONS
		if ( ashes::checkFlag( flags, ImageUsageFlags::eVideoDecodeDpb ) )
		{
			result |= VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR;
		}
#endif
		if ( ashes::checkFlag( flags, ImageUsageFlags::eFragmentDensityMap ) )
		{
			result |= VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;
		}

		if ( ashes::checkFlag( flags, ImageUsageFlags::eFragmentShadingRateAttachment ) )
		{
			result |= VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
		}
#ifdef VK_ENABLE_BETA_EXTENSIONS
		if ( ashes::checkFlag( flags, ImageUsageFlags::eVideoEncodeSrc ) )
		{
			result |= VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR;
		}
#endif
#ifdef VK_ENABLE_BETA_EXTENSIONS
		if ( ashes::checkFlag( flags, ImageUsageFlags::eVideoEncodeDpb ) )
		{
			result |= VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR;
		}
#endif

		return result;
	}

	GBufferBase::GBufferBase( RenderDevice const & device
		, String name )
		: Named{ c3d::move( name ) }
		, m_device{ device }
	{
	}

	TextureUPtr GBufferBase::doCreateTexture( crg::ResourcesCache & resources
		, String const & name
		, ImageCreateFlags createFlags
		, Extent3D const & size
		, uint32_t layerCount
		, SampleCount sampleCount
		, uint32_t mipLevels
		, PixelFormat format
		, ImageUsageFlags usageFlags
		, BorderColour borderColor
		, ComparisonFunc compareOp )const
	{
		TextureSamplerCreateInfo samplerInfo{ .borderColor = borderColor
			, .compareOp = compareOp };
		TextureCreateInfo createInfo{ createFlags
			, size, layerCount, mipLevels
			, format, usageFlags, sampleCount };
		return makeUnique< Texture >( m_device
			, resources
			, name
			, createInfo
			, TextureSamplerInfo{ samplerInfo } );
	}
}
