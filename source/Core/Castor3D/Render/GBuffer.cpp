#include "Castor3D/Render/GBuffer.hpp"

namespace castor3d
{
	VkFormatFeatureFlags getFeatureFlags( VkImageUsageFlags flags )
	{
		VkFormatFeatureFlags result{};

		if ( ashes::checkFlag( flags, VK_IMAGE_USAGE_TRANSFER_SRC_BIT ) )
		{
			result |= VK_FORMAT_FEATURE_TRANSFER_SRC_BIT;
		}

		if ( ashes::checkFlag( flags, VK_IMAGE_USAGE_TRANSFER_DST_BIT ) )
		{
			result |= VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
		}

		if ( ashes::checkFlag( flags, VK_IMAGE_USAGE_SAMPLED_BIT ) )
		{
			result |= VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
		}

		if ( ashes::checkFlag( flags, VK_IMAGE_USAGE_STORAGE_BIT ) )
		{
			result |= VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
		}

		if ( ashes::checkFlag( flags, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT ) )
		{
			result |= VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
		}
		else if ( ashes::checkFlag( flags, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT ) )
		{
			result |= VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
		}
		else if ( ashes::checkFlag( flags, VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT ) )
		{
			result |= VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
		}

		if ( ashes::checkFlag( flags, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT ) )
		{
		}

#ifdef VK_ENABLE_BETA_EXTENSIONS
		if ( ashes::checkFlag( flags, VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR ) )
		{
			result |= VK_FORMAT_FEATURE_VIDEO_DECODE_OUTPUT_BIT_KHR;
		}
#endif
#ifdef VK_ENABLE_BETA_EXTENSIONS
		if ( ashes::checkFlag( flags, VK_IMAGE_USAGE_VIDEO_DECODE_SRC_BIT_KHR ) )
		{
		}
#endif
#ifdef VK_ENABLE_BETA_EXTENSIONS
		if ( ashes::checkFlag( flags, VK_IMAGE_USAGE_VIDEO_DECODE_DPB_BIT_KHR ) )
		{
			result |= VK_FORMAT_FEATURE_VIDEO_DECODE_DPB_BIT_KHR;
		}
#endif
		if ( ashes::checkFlag( flags, VK_IMAGE_USAGE_FRAGMENT_DENSITY_MAP_BIT_EXT ) )
		{
			result |= VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT;
		}

		if ( ashes::checkFlag( flags, VK_IMAGE_USAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR ) )
		{
			result |= VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
		}

#ifdef VK_ENABLE_BETA_EXTENSIONS
		if ( ashes::checkFlag( flags, VK_IMAGE_USAGE_VIDEO_ENCODE_DST_BIT_KHR ) )
		{
		}
#endif
#ifdef VK_ENABLE_BETA_EXTENSIONS
		if ( ashes::checkFlag( flags, VK_IMAGE_USAGE_VIDEO_ENCODE_SRC_BIT_KHR ) )
		{
			result |= VK_FORMAT_FEATURE_VIDEO_ENCODE_INPUT_BIT_KHR;
		}
#endif
#ifdef VK_ENABLE_BETA_EXTENSIONS
		if ( ashes::checkFlag( flags, VK_IMAGE_USAGE_VIDEO_ENCODE_DPB_BIT_KHR ) )
		{
			result |= VK_FORMAT_FEATURE_VIDEO_ENCODE_DPB_BIT_KHR;
		}
#endif
		if ( ashes::checkFlag( flags, VK_IMAGE_USAGE_ATTACHMENT_FEEDBACK_LOOP_BIT_EXT ) )
		{
		}

		if ( ashes::checkFlag( flags, VK_IMAGE_USAGE_INVOCATION_MASK_BIT_HUAWEI ) )
		{
		}

		if ( ashes::checkFlag( flags, VK_IMAGE_USAGE_SAMPLE_WEIGHT_BIT_QCOM ) )
		{
		}

		if ( ashes::checkFlag( flags, VK_IMAGE_USAGE_SAMPLE_BLOCK_MATCH_BIT_QCOM ) )
		{
		}

		return result;
	}

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
		, VkSampleCountFlagBits sampleCount
		, uint32_t mipLevels
		, VkFormat format
		, VkImageUsageFlags usageFlags
		, VkBorderColor const & borderColor
		, VkCompareOp compareOp )const
	{
		return std::make_shared< Texture >( m_device
			, resources
			, name
			, createFlags
			, size
			, layerCount
			, sampleCount
			, mipLevels
			, format
			, usageFlags
			, borderColor
			, compareOp );
	}
}
