/*
See LICENSE file in root folder
*/
#include "Castor3D/Render/Upscale/UpscaleBlit.hpp"

#include <RenderGraph/GraphContext.hpp>
#include <RenderGraph/RecordContext.hpp>
#include <RenderGraph/ResourceHandler.hpp>

namespace c3d
{
	//*********************************************************************************************

	bool BlitUpscalingSDKInstance::initialise( [[maybe_unused]] Extent2D const & renderSize
		, [[maybe_unused]] Extent2D const & displaySize
		, [[maybe_unused]] UpscaleConfig const & config )
	{
		return true;
	}

	void BlitUpscalingSDKInstance::cleanup()noexcept
	{
	}

	Point2f BlitUpscalingSDKInstance::getJitter( [[maybe_unused]] u32 & frameIndex )const noexcept
	{
		return Point2f{};
	}

	void BlitUpscalingSDKInstance::evaluate( crg::RecordContext & recContext
		, VkCommandBuffer commandBuffer
		, crg::ImageViewId resolvedColor
		, crg::ImageViewId unresolvedColor
		, [[maybe_unused]] crg::ImageViewId motionVectors
		, [[maybe_unused]] crg::ImageViewId depth
		, [[maybe_unused]] bool resetAccumulation
		, [[maybe_unused]] Point2f const & jitterOffset
		, [[maybe_unused]] Point2f const & mVScale )const
	{
		auto & srcSubresource = unresolvedColor.data->info.subresourceRange;
		auto & dstSubresource = resolvedColor.data->info.subresourceRange;
		auto srcExtent = getExtent( unresolvedColor );
		auto dstExtent = getExtent( resolvedColor );
		VkImageBlit region{ getSubresourceLayer( srcSubresource ), { VkOffset3D{}, VkOffset3D{ int32_t( srcExtent.width ), int32_t( srcExtent.height ), 1 } }
			, getSubresourceLayer( dstSubresource ), { VkOffset3D{}, VkOffset3D{ int32_t( dstExtent.width ), int32_t( dstExtent.height ), 1 } } };
		auto srcState = recContext.getLayoutState( unresolvedColor );
		auto dstState = recContext.getLayoutState( resolvedColor );
		recContext.memoryBarrier( commandBuffer, unresolvedColor, makeLayoutState( ImageLayout::eTransferSrc ) );
		recContext.memoryBarrier( commandBuffer, resolvedColor, makeLayoutState( ImageLayout::eTransferDst ) );
		auto & resources = recContext.getResources();
		resources->vkCmdBlitImage( commandBuffer
			, resources.createImage( unresolvedColor.data->image ), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
			, resources.createImage( resolvedColor.data->image ), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
			, 1u, &region, VK_FILTER_LINEAR );
		recContext.memoryBarrier( commandBuffer, resolvedColor, dstState );
		recContext.memoryBarrier( commandBuffer, unresolvedColor, srcState );
	}

	//*********************************************************************************************

	bool BlitUpscalingSDK::initialise( [[maybe_unused]] RenderDevice const & device )
	{
		return true;
	}

	void BlitUpscalingSDK::cleanup()noexcept
	{
	}

	bool BlitUpscalingSDK::queryOptimalSettings( Extent2D const & displaySize
		, [[maybe_unused]] UpscaleConfig const & config
		, Extent2D & recommendedSize )const
	{
		recommendedSize = Extent2D{ displaySize.width / 2, displaySize.height / 2 };
		return true;
	}

	RawUniquePtr< UpscalingSDKInstance > BlitUpscalingSDK::createInstance()const
	{
		return makeRawUnique< BlitUpscalingSDKInstance >();
	}

	//*********************************************************************************************
}
