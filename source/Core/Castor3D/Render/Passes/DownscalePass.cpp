#include "Castor3D/Render/Passes/DownscalePass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/SamplerCache.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"

#include <CastorUtils/Graphics/RgbaColour.hpp>

#include <ashespp/Image/ImageView.hpp>

#include <RenderGraph/FrameGraph.hpp>

CU_ImplementCUSmartPtr( castor3d, DownscalePass )

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	SamplerSPtr doCreateSampler( Engine & engine
		, String const & name
		, VkSamplerAddressMode mode )
	{
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
			sampler->setWrapS( mode );
			sampler->setWrapT( mode );
		}

		return sampler;
	}

	crg::ImageId doCreateImage( crg::FrameGraph & graph
		, castor::String const & name
		, VkFormat format
		, VkExtent2D const & size )
	{
		return graph.createImage( crg::ImageData{ name
			, 0u
			, VK_IMAGE_TYPE_2D
			, format
			, { size.width, size.height, 1u }
			, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_SAMPLED_BIT ) } );
	}

	crg::ImageIdArray doCreateImages( crg::FrameGraph & graph
		, castor::String const & name
		, crg::ImageViewIdArray const & views
		, VkExtent2D const & size )
	{
		crg::ImageIdArray result;

		for ( auto & view : views )
		{
			result.emplace_back( doCreateImage( graph
				, name + string::toString( result.size() )
				, getFormat( view )
				, size ) );
		}

		return result;
	}

	crg::ImageViewIdArray doCreateViews( crg::FrameGraph & graph
		, crg::ImageIdArray const & images )
	{
		crg::ImageViewIdArray result;

		for ( auto & image : images )
		{
			result.emplace_back( graph.createView( crg::ImageViewData{ image.data->name
				, image
				, 0u
				, VK_IMAGE_VIEW_TYPE_2D
				, getFormat( image )
				, { ashes::getAspectMask( getFormat( image ) ), 0u, 1u, 0u, 1u } } ) );
		}

		return result;
	}

	//*********************************************************************************************

	DownscalePass::DownscalePass( crg::FrameGraph & graph
		, crg::FramePass const *& previousPass
		, RenderDevice const & device
		, castor::String const & category
		, crg::ImageViewIdArray const & srcViews
		, VkExtent2D const & dstSize )
		: m_device{ device }
		, m_result{ doCreateImages( graph, "Downscaled", srcViews, dstSize ) }
		, m_resultViews{ doCreateViews( graph, m_result ) }
	{
		// TODO CRG
		//m_commandBuffer->begin();
		//m_timer->beginPass( *m_commandBuffer, 0u );
		//m_commandBuffer->beginDebugBlock(
		//	{
		//		"Downscale",
		//		makeFloatArray( m_engine.getNextRainbowColour() ),
		//	} );
		//auto it = m_result.begin();

		//for ( auto & srcView : srcViews )
		//{
		//	auto srcSize = srcView.image->getDimensions();
		//	auto dstView = it->getTexture()->getDefaultView().getTargetView();

		//	m_commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
		//		, VK_PIPELINE_STAGE_TRANSFER_BIT
		//		, srcView.makeTransferSource( VK_IMAGE_LAYOUT_UNDEFINED ) );
		//	m_commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
		//		, VK_PIPELINE_STAGE_TRANSFER_BIT
		//		, dstView.makeTransferDestination( VK_IMAGE_LAYOUT_UNDEFINED ) );
		//	m_commandBuffer->blitImage( *srcView.image
		//		, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
		//		, *dstView.image
		//		, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
		//		, {
		//			VkImageBlit
		//			{
		//				{
		//					srcView->subresourceRange.aspectMask,
		//					srcView->subresourceRange.baseMipLevel,
		//					srcView->subresourceRange.baseArrayLayer,
		//					srcView->subresourceRange.layerCount,
		//				},
		//				{
		//					{ 0, 0, 0 },
		//					{ int32_t( srcSize.width ), int32_t( srcSize.height ), 1 },
		//				},
		//				{
		//					dstView->subresourceRange.aspectMask,
		//					dstView->subresourceRange.baseMipLevel,
		//					dstView->subresourceRange.baseArrayLayer,
		//					dstView->subresourceRange.layerCount,
		//				},
		//				{
		//					{ 0, 0, 0 },
		//					{ int32_t( dstSize.width ), int32_t( dstSize.height ), 1 },
		//				},
		//			}
		//		}
		//		, VK_FILTER_NEAREST );
		//	m_commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT
		//		, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
		//		, srcView.makeShaderInputResource( VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL ) );
		//	m_commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT
		//		, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
		//		, dstView.makeShaderInputResource( VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ) );
		//	++it;
		//}

		//m_commandBuffer->endDebugBlock();
		//m_timer->endPass( *m_commandBuffer, 0u );
		//m_commandBuffer->end();
	}

	void DownscalePass::accept( PipelineVisitorBase & visitor )
	{
		uint32_t index{};

		for ( auto & unit : m_resultViews )
		{
			visitor.visit( "Downscale" + string::toString( index++ )
				, unit
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				, TextureFactors{}.invert( true ) );
		}
	}

	//*********************************************************************************************
}
