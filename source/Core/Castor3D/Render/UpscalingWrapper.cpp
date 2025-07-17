/*
See LICENSE file in root folder
*/
#include "Castor3D/Render/UpscalingWrapper.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderTarget.hpp"

#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Command/CommandPool.hpp>

#include <RenderGraph/RunnableGraph.hpp>

namespace c3d
{
	class UpscalingSDK
	{
	public:
		virtual ~UpscalingSDK() = default;
		virtual bool initialise( RenderDevice const & device ) = 0;
		virtual void cleanup()noexcept = 0;
		virtual bool queryOptimalSettings( Extent2D const & displaySize
			, UpscalingConfig const & config
			, UpscalingRecommendedSettings & recommendedSettings )const = 0;
		virtual RawUniquePtr< UpscalingSDKInstance > createInstance()const = 0;
	};

	class UpscalingSDKInstance
	{
	public:
		virtual ~UpscalingSDKInstance() = default;
		virtual bool initialise( Extent2D const & renderSize
			, Extent2D const & displaySize
			, UpscalingConfig const & config ) = 0;
		virtual void cleanup()noexcept = 0;
		virtual void evaluate( crg::RecordContext & recContext
			, VkCommandBuffer commandBuffer
			, crg::ImageViewId resolvedColor
			, crg::ImageViewId unresolvedColor )const = 0;
	};

	class BlitUpscalingSDK
		: public UpscalingSDK
	{
	public:
		bool initialise( [[maybe_unused]] RenderDevice const & device )override
		{
			return true;
		}

		void cleanup()noexcept override
		{
		}

		bool queryOptimalSettings( Extent2D const & displaySize
			, [[maybe_unused]] UpscalingConfig const & config
			, UpscalingRecommendedSettings & recommendedSettings )const override
		{
			auto result = Extent2D{ displaySize.width / 2, displaySize.height / 2 };
			recommendedSettings.recommendedOptimalRenderSize = result;
			recommendedSettings.dynamicMaximumRenderSize = result;
			recommendedSettings.dynamicMinimumRenderSize = result;
			return true;
		}

		RawUniquePtr< UpscalingSDKInstance > createInstance()const override;
	};

	class BlitUpscalingSDKInstance
		: public UpscalingSDKInstance
	{
	public:
		bool initialise( [[maybe_unused]] Extent2D const & renderSize
			, [[maybe_unused]] Extent2D const & displaySize
			, [[maybe_unused]] UpscalingConfig const & config )override
		{
			return true;
		}

		void cleanup()noexcept override
		{
		}

		void evaluate( crg::RecordContext & recContext
			, VkCommandBuffer commandBuffer
			, crg::ImageViewId resolvedColor
			, crg::ImageViewId unresolvedColor )const override
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
	};

	RawUniquePtr< UpscalingSDKInstance > BlitUpscalingSDK::createInstance()const
	{
		return makeRawUnique< BlitUpscalingSDKInstance >();
	}

	//*********************************************************************************************

	UpscalingInstance::UpscalingInstance( UpscalingWrapper const & parent
		, RenderTarget & target
		, Extent2D const & renderSize
		, Extent2D const & displaySize
		, UpscalingConfig const & config )
		: m_sdk{ parent.m_sdk->createInstance() }
		, m_target{ target }
	{
		m_sdk->initialise( renderSize, displaySize, config );
	}

	UpscalingInstance::~UpscalingInstance()noexcept
	{
		m_sdk->cleanup();
		m_sdk.reset();
	}

	void UpscalingInstance::update()
	{
	}

	void UpscalingInstance::evaluateUpscaling( crg::RecordContext & recContext
		, VkCommandBuffer commandBuffer
		, crg::ImageViewId resolvedColor
		, crg::ImageViewId unresolvedColor )const
	{
		m_sdk->evaluate( recContext, commandBuffer
			, resolvedColor, unresolvedColor );
	}

	//*********************************************************************************************

	UpscalingWrapper::UpscalingWrapper( RenderDevice const & device )
	{
			m_sdk = makeRawUnique< BlitUpscalingSDK >();
			m_sdk->initialise( device );
	}

	UpscalingWrapper::~UpscalingWrapper()noexcept
	{
		m_sdk->cleanup();
		m_sdk.reset();
	}

	bool UpscalingWrapper::queryOptimalSettings( Extent2D const & displaySize
		, UpscalingConfig const & config
		, UpscalingRecommendedSettings & recommendedSettings )const
	{
		if ( !config.enabled )
		{
			recommendedSettings.recommendedOptimalRenderSize = displaySize;
			recommendedSettings.dynamicMaximumRenderSize = displaySize;
			recommendedSettings.dynamicMinimumRenderSize = displaySize;
			return true;
		}

		return m_sdk->queryOptimalSettings( displaySize, config
			, recommendedSettings );
	}

	UpscalingInstanceUPtr UpscalingWrapper::createInstance( RenderTarget & target
		, Extent2D const & renderSize
		, Extent2D const & displaySize
		, UpscalingConfig const & config )const
	{
		return makeUnique< UpscalingInstance >( *this, target
			, renderSize, displaySize, config );
	}

	//*********************************************************************************************
}

CU_ImplementSmartPtr( c3d, UpscalingWrapper )
CU_ImplementSmartPtr( c3d, UpscalingInstance )
