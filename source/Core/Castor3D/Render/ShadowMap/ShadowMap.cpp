#include "Castor3D/Render/ShadowMap/ShadowMap.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapPass.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Light/Light.hpp"

#include <CastorUtils/Graphics/RgbaColour.hpp>

#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/Sync/Fence.hpp>
#include <ashespp/Sync/Semaphore.hpp>

CU_ImplementSmartPtr( castor3d, ShadowMap )

namespace castor3d
{
	namespace shdmap
	{
		static uint32_t getPassesIndex( bool needsVsm
			, bool needsRsm )
		{
			if ( needsVsm )
			{
				if ( needsRsm )
				{
					return 3u;
				}

				return 1u;
			}

			if ( needsRsm )
			{
				return 2u;
			}

			return 0u;
		}

		static void doInitialiseImage( RenderDevice const & device
			, crg::ResourcesCache & resources
			, crg::GraphContext & context
			, ashes::CommandBuffer & commandBuffer
			, Texture const & texture
			, VkImageLayout finalLayout
			, VkClearValue clearValue )
		{
			auto transferBarrier = makeVkStruct< VkImageMemoryBarrier >( 0u
				, VkAccessFlags( VK_ACCESS_TRANSFER_WRITE_BIT )
				, VK_IMAGE_LAYOUT_UNDEFINED
				, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
				, VK_QUEUE_FAMILY_IGNORED
				, VK_QUEUE_FAMILY_IGNORED
				, texture.image
				, texture.wholeViewId.data->info.subresourceRange );
			device->vkCmdPipelineBarrier( commandBuffer
				, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT
				, VK_PIPELINE_STAGE_TRANSFER_BIT
				, VK_DEPENDENCY_BY_REGION_BIT
				, 0u
				, nullptr
				, 0u
				, nullptr
				, 1u
				, &transferBarrier );

			if ( ashes::isDepthOrStencilFormat( texture.imageId.data->info.format ) )
			{
				device->vkCmdClearDepthStencilImage( commandBuffer
					, texture.image
					, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
					, &clearValue.depthStencil
					, 1u
					, &texture.wholeViewId.data->info.subresourceRange );
			}
			else
			{
				device->vkCmdClearColorImage( commandBuffer
					, texture.image
					, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
					, &clearValue.color
					, 1u
					, &texture.wholeViewId.data->info.subresourceRange );
			}

			auto shaderBarrier = makeVkStruct< VkImageMemoryBarrier >( VkAccessFlags( VK_ACCESS_TRANSFER_WRITE_BIT )
				, crg::getAccessMask( finalLayout )
				, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
				, finalLayout
				, VK_QUEUE_FAMILY_IGNORED
				, VK_QUEUE_FAMILY_IGNORED
				, texture.image
				, texture.wholeViewId.data->info.subresourceRange );
			device->vkCmdPipelineBarrier( commandBuffer
				, VK_PIPELINE_STAGE_TRANSFER_BIT
				, crg::getStageMask( finalLayout )
				, VK_DEPENDENCY_BY_REGION_BIT
				, 0u
				, nullptr
				, 0u
				, nullptr
				, 1u
				, &shaderBarrier );
		}
	}

	ShadowMap::ShadowMap( crg::ResourcesCache & resources
		, RenderDevice const & device
		, Scene & scene
		, LightType lightType
		, VkImageCreateFlags createFlags
		, castor::Size const & size
		, uint32_t layerCount
		, uint32_t count )
		: OwnedBy< Engine >{ *scene.getEngine() }
		, m_device{ device }
		, m_resources{ resources }
		, m_scene{ scene }
		, m_name{ castor::string::snakeToCamelCase( getName( lightType ) ) }
		, m_lightType{ lightType }
		, m_staticsResult{ resources
			, m_device
			, m_name + "/Statics"
			, createFlags
			, size
			, layerCount }
		, m_result{ resources
			, m_device
			, m_name
			, createFlags
			, size
			, layerCount }
		, m_count{ count }
	{
		m_staticsResult.create();
		m_result.create();
		auto & context = m_device.makeContext();
		auto queueData = m_device.graphicsData();
		auto commandBuffer = queueData->commandPool->createCommandBuffer();
		commandBuffer->begin();
		auto it = m_result.begin();
		auto sit = m_staticsResult.begin();
		uint32_t index{};

		while ( it != m_result.end() )
		{
			shdmap::doInitialiseImage( device
				, resources
				, context
				, *commandBuffer
				, **sit
				, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
				, getClearValue( SmTexture( index ) ) );
			shdmap::doInitialiseImage( device
				, resources
				, context
				, *commandBuffer
				, **it
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				, getClearValue( SmTexture( index ) ) );
			++it;
			++sit;
			++index;
		}

		commandBuffer->end();
		auto fence = m_device->createFence();
		queueData->queue->submit( *commandBuffer, fence.get() );
		fence->wait( ashes::MaxTimeout );
	}

	void ShadowMap::update( CpuUpdater & updater )
	{
		auto vsm = updater.light->getShadowType() == ShadowType::eVariance;
		auto rsm = updater.light->needsRsmShadowMaps();
		m_passesIndex = shdmap::getPassesIndex( vsm, rsm );
		auto & myPasses = m_passes[m_passesIndex];

		if ( updater.index < doGetMaxCount()
			&& updater.index >= myPasses.otherNodes.runnables.size() )
		{
			auto graph = std::make_unique< crg::FrameGraph >( m_resources.getHandler(), m_name + "SM" );
			auto previous = doCreatePasses( *graph
				, crg::FramePassArray{}
				, updater.index
				, vsm
				, rsm
				, true
				, myPasses.staticNodes );
			doCreatePasses( *graph
				, previous
				, updater.index
				, vsm
				, rsm
				, false
				, myPasses.otherNodes );
			myPasses.staticNodes.graphs.emplace_back( nullptr );
			myPasses.otherNodes.graphs.emplace_back( std::move( graph ) );
		}

		doUpdate( updater, myPasses.staticNodes );
		doUpdate( updater, myPasses.otherNodes );
	}

	void ShadowMap::update( GpuUpdater & updater )
	{
		auto & myPasses = m_passes[m_passesIndex];
		doUpdate( updater, myPasses.staticNodes );
		doUpdate( updater, myPasses.otherNodes );
	}

	void ShadowMap::accept( PipelineVisitorBase & visitor )
	{
		for ( uint32_t i = 1u; i < uint32_t( SmTexture::eCount ); ++i )
		{
			uint32_t index = 0u;
			auto & result = m_result[SmTexture( i )];

			for ( auto & view : result.subViewsId )
			{
				auto smTexture = SmTexture( i );
				visitor.visit( m_name + "/" + getTexName( smTexture ) + cuT( "L" ) + castor::string::toString( index++ )
					, view
					, ( ashes::isDepthOrStencilFormat( view.data->info.format )
						? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
						: VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL )
					, TextureFactors::tex2D( { 25.0, 25.0, 25.0 }, { -24.0, -24.0, -24.0 } )
						.invert( true )
						.depth( smTexture == SmTexture::eLinearDepth ) );
			}
		}
	}

	crg::SemaphoreWaitArray ShadowMap::render( crg::SemaphoreWaitArray const & toWait
		, ashes::Queue const & queue
		, uint32_t index )
	{
		auto & myPasses = m_passes[m_passesIndex];

#if !C3D_MeasureShadowMapImpact
		if ( getEngine()->areUpdateOptimisationsEnabled()
			&& doIsUpToDate( index, myPasses.staticNodes )
			&& doIsUpToDate( index, myPasses.otherNodes ) )
		{
			return toWait;
		}
#endif

		if ( !myPasses.otherNodes.runnables[index] )
		{
			CU_Require( myPasses.otherNodes.graphs[index] != nullptr );
			myPasses.otherNodes.runnables[index] = myPasses.otherNodes.graphs[index]->compile( m_device.makeContext() );
			printGraph( *myPasses.otherNodes.runnables.back() );
			myPasses.otherNodes.runnables[index]->record();
		}

		auto result = myPasses.otherNodes.runnables[index]->run( toWait, queue );
		doSetUpToDate( index, myPasses.staticNodes );
		doSetUpToDate( index, myPasses.otherNodes );
		return result;
	}

	ashes::VkClearValueArray const & ShadowMap::getClearValues()const
	{
		static ashes::VkClearValueArray const result
		{
			[]()
			{
				ashes::VkClearValueArray tmp;

				for ( uint32_t i = 0u; i < uint32_t( SmTexture::eCount ); ++i )
				{
					tmp.push_back( getClearValue( SmTexture( i ) ) );
				}

				return tmp;
			}()
		};
		return result;
	}

	ashes::Sampler const & ShadowMap::getSampler( SmTexture texture
		, uint32_t index )const
	{
		return *m_result[texture].sampler;
	}

	crg::ImageViewId ShadowMap::getView( SmTexture texture
		, uint32_t index )const
	{
		return m_result[texture].wholeViewId;
	}

	crg::ImageViewIdArray ShadowMap::getViews( SmTexture texture
		, uint32_t index )const
	{
		return m_result[texture].subViewsId;
	}

	crg::FramePassArray ShadowMap::doCreatePasses( crg::FrameGraph & graph
		, crg::FramePassArray const & previousPasses
		, uint32_t index
		, bool vsm
		, bool rsm
		, bool isStatic
		, Passes & passes )
	{
		auto result = doCreatePass( graph
			, previousPasses
			, index
			, vsm
			, rsm
			, isStatic
			, passes );

		if ( !isStatic )
		{
			// The graph will be defined for finale shadow map pass, but not for static one.
			passes.runnables.push_back( graph.compile( m_device.makeContext() ) );
			printGraph( *passes.runnables.back() );
			passes.runnables.back()->record();
		}
		else
		{
			passes.runnables.push_back( nullptr );
		}

		return result;
	}

	void ShadowMap::doRegisterGraphIO( crg::FrameGraph & graph
		, bool vsm
		, bool rsm
		, bool isStatic )const
	{
		auto & smResult = getShadowPassResult( isStatic );
		auto & linear = smResult[SmTexture::eLinearDepth];
		auto & variance = smResult[SmTexture::eVariance];
		auto & normal = smResult[SmTexture::eNormal];
		auto & position = smResult[SmTexture::ePosition];
		auto & flux = smResult[SmTexture::eFlux];

		if ( isStatic )
		{
			auto & depth = smResult[SmTexture::eDepth];
			graph.addInput( depth.wholeViewId
				, crg::makeLayoutState( VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL ) );
			graph.addInput( linear.wholeViewId
				, crg::makeLayoutState( VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL ) );

			if ( vsm )
			{
				graph.addInput( variance.wholeViewId
					, crg::makeLayoutState( VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL ) );
			}

			if ( rsm )
			{
				graph.addInput( normal.wholeViewId
					, crg::makeLayoutState( VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL ) );
				graph.addInput( position.wholeViewId
					, crg::makeLayoutState( VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL ) );
				graph.addInput( flux.wholeViewId
					, crg::makeLayoutState( VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL ) );
			}
		}
		else
		{
			graph.addOutput( linear.wholeViewId
				, crg::makeLayoutState( VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ) );

			if ( vsm )
			{
				graph.addOutput( variance.wholeViewId
					, crg::makeLayoutState( VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ) );
			}

			if ( rsm )
			{
				graph.addOutput( normal.wholeViewId
					, crg::makeLayoutState( VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ) );
				graph.addOutput( position.wholeViewId
					, crg::makeLayoutState( VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ) );
				graph.addOutput( flux.wholeViewId
					, crg::makeLayoutState( VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ) );
			}
		}
	}

	bool ShadowMap::doEnableCopyStatic( uint32_t index )const
	{
		auto & dyn = m_passes[m_passesIndex].otherNodes;

		return !dyn.passes[index]->pass->hasNodes()
			|| dyn.passes[index]->pass->isPassEnabled();
	}

	bool ShadowMap::doEnableBlur( uint32_t index )const
	{
		auto & dyn = m_passes[m_passesIndex].otherNodes;

		return !dyn.passes[index]->pass->hasNodes()
			|| dyn.passes[index]->pass->isPassEnabled();
	}
}
