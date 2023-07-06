#include "Castor3D/Render/Passes/DownscalePass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/Graphics/RgbaColour.hpp>

#include <ashespp/Image/ImageView.hpp>

#include <RenderGraph/FrameGraph.hpp>

CU_ImplementSmartPtr( castor3d, DownscalePass )

namespace castor3d
{
	//*********************************************************************************************

	namespace passdownscl
	{
		static TextureArray doCreateImages( RenderDevice const & device
			, castor::String const & name
			, TextureArray const & views
			, VkExtent2D const & size )
		{
			TextureArray result;

			if ( !views.empty() )
			{
				auto & resources = *( *views.begin() ).resources;

				for ( auto & view : views )
				{
					result.emplace_back( device
						, resources
						, name + castor::string::toString( result.size() )
						, 0u
						, VkExtent3D{ size.width, size.height, 1u }
						, 1u
						, 1u
						, view.getFormat()
						, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
							| VK_IMAGE_USAGE_SAMPLED_BIT ) );
				}
			}

			return result;
		}
	}

	//*********************************************************************************************

	DownscalePass::DownscalePass( crg::FrameGraph & graph
		, crg::FramePass const *& previousPass
		, RenderDevice const & device
		, castor::String const & category
		, TextureArray const & srcViews
		, VkExtent2D const & dstSize )
		: m_device{ device }
		, m_result{ passdownscl::doCreateImages( m_device, "Downscaled", srcViews, dstSize ) }
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
		//	auto dstView = it->getTexture()->getDefaultTargetView();

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

		for ( auto & texture : m_result )
		{
			visitor.visit( "Downscale" + castor::string::toString( index++ )
				, texture
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				, TextureFactors{}.invert( true ) );
		}
	}

	//*********************************************************************************************
}
