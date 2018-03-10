#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkFormatFeatureFlags convert( renderer::FormatFeatureFlags const & flags )
	{
		VkFormatFeatureFlags result{ 0 };

		if ( checkFlag( flags, renderer::FormatFeatureFlag::eSampledImage ) )
		{
			result |= VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
		}

		if ( checkFlag( flags, renderer::FormatFeatureFlag::eStorageImage ) )
		{
			result |= VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
		}

		if ( checkFlag( flags, renderer::FormatFeatureFlag::eStorageImageAtomic ) )
		{
			result |= VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT;
		}

		if ( checkFlag( flags, renderer::FormatFeatureFlag::eUniformTexelBuffer ) )
		{
			result |= VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT;
		}

		if ( checkFlag( flags, renderer::FormatFeatureFlag::eStorageTexelBuffer ) )
		{
			result |= VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT;
		}

		if ( checkFlag( flags, renderer::FormatFeatureFlag::eStorageTexelBufferAtomic ) )
		{
			result |= VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT;
		}

		if ( checkFlag( flags, renderer::FormatFeatureFlag::eVertexBuffer ) )
		{
			result |= VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;
		}

		if ( checkFlag( flags, renderer::FormatFeatureFlag::eColourAttachment ) )
		{
			result |= VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
		}

		if ( checkFlag( flags, renderer::FormatFeatureFlag::eColourAttachmentBlend ) )
		{
			result |= VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT;
		}

		if ( checkFlag( flags, renderer::FormatFeatureFlag::eDepthStencilAttachment ) )
		{
			result |= VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
		}

		if ( checkFlag( flags, renderer::FormatFeatureFlag::eBlitSrc ) )
		{
			result |= VK_FORMAT_FEATURE_BLIT_SRC_BIT;
		}

		if ( checkFlag( flags, renderer::FormatFeatureFlag::eBlitDst ) )
		{
			result |= VK_FORMAT_FEATURE_BLIT_DST_BIT;
		}

		if ( checkFlag( flags, renderer::FormatFeatureFlag::eSampledImageFilterLinear ) )
		{
			result |= VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;
		}

		if ( checkFlag( flags, renderer::FormatFeatureFlag::eSampledImageFilterCubic ) )
		{
			result |= VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG;
		}

		return result;
	}

	renderer::FormatFeatureFlags convertFormatFeatureFlags( VkFormatFeatureFlags const & flags )
	{
		renderer::FormatFeatureFlags result{ 0 };

		if ( renderer::checkFlag( flags, VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT ) )
		{
			result |= renderer::FormatFeatureFlag::eSampledImage;
		}

		if ( renderer::checkFlag( flags, VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT ) )
		{
			result |= renderer::FormatFeatureFlag::eStorageImage;
		}

		if ( renderer::checkFlag( flags, VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT ) )
		{
			result |= renderer::FormatFeatureFlag::eStorageImageAtomic;
		}

		if ( renderer::checkFlag( flags, VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT ) )
		{
			result |= renderer::FormatFeatureFlag::eUniformTexelBuffer;
		}

		if ( renderer::checkFlag( flags, VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT ) )
		{
			result |= renderer::FormatFeatureFlag::eStorageTexelBuffer;
		}

		if ( renderer::checkFlag( flags, VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT ) )
		{
			result |= renderer::FormatFeatureFlag::eStorageTexelBufferAtomic;
		}

		if ( renderer::checkFlag( flags, VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT ) )
		{
			result |= renderer::FormatFeatureFlag::eVertexBuffer;
		}

		if ( renderer::checkFlag( flags, VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT ) )
		{
			result |= renderer::FormatFeatureFlag::eColourAttachment;
		}

		if ( renderer::checkFlag( flags, VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT ) )
		{
			result |= renderer::FormatFeatureFlag::eColourAttachmentBlend;
		}

		if ( renderer::checkFlag( flags, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT ) )
		{
			result |= renderer::FormatFeatureFlag::eDepthStencilAttachment;
		}

		if ( renderer::checkFlag( flags, VK_FORMAT_FEATURE_BLIT_SRC_BIT ) )
		{
			result |= renderer::FormatFeatureFlag::eBlitSrc;
		}

		if ( renderer::checkFlag( flags, VK_FORMAT_FEATURE_BLIT_DST_BIT ) )
		{
			result |= renderer::FormatFeatureFlag::eBlitDst;
		}

		if ( renderer::checkFlag( flags, VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT ) )
		{
			result |= renderer::FormatFeatureFlag::eSampledImageFilterLinear;
		}

		if ( renderer::checkFlag( flags, VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG ) )
		{
			result |= renderer::FormatFeatureFlag::eSampledImageFilterCubic;
		}

		return result;
	}
}
