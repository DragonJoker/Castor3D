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

CU_ImplementCUSmartPtr( castor3d, ShadowMap )

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
	}

	ShadowMap::ShadowMap( crg::ResourceHandler & handler
		, RenderDevice const & device
		, Scene & scene
		, LightType lightType
		, VkImageCreateFlags createFlags
		, castor::Size const & size
		, uint32_t layerCount
		, uint32_t count )
		: OwnedBy< Engine >{ *scene.getEngine() }
		, m_device{ device }
		, m_handler{ handler }
		, m_scene{ scene }
		, m_name{ castor::string::snakeToCamelCase( getName( lightType ) ) + "SM" }
		, m_lightType{ lightType }
		, m_result{ handler
			, m_device
			, m_name
			, createFlags
			, size
			, layerCount }
		, m_count{ count }
	{
		m_result.create();
		auto & context = m_device.makeContext();
		auto queueData = m_device.graphicsData();
		auto commandBuffer = queueData->commandPool->createCommandBuffer();
		commandBuffer->begin();

		for ( auto & ptexture : m_result )
		{
			auto & texture = *ptexture;
			texture.image = handler.createImage( context, texture.imageId );
			auto transferBarrier = makeVkStruct< VkImageMemoryBarrier >( 0u
				, VkAccessFlags( VK_ACCESS_TRANSFER_WRITE_BIT )
				, VK_IMAGE_LAYOUT_UNDEFINED
				, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
				, VK_QUEUE_FAMILY_IGNORED
				, VK_QUEUE_FAMILY_IGNORED
				, texture.image
				, texture.wholeViewId.data->info.subresourceRange );
			device->vkCmdPipelineBarrier( *commandBuffer
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
				device->vkCmdClearDepthStencilImage( *commandBuffer
					, texture.image
					, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
					, &defaultClearDepthStencil.depthStencil
					, 1u
					, &texture.wholeViewId.data->info.subresourceRange );
			}
			else
			{
				device->vkCmdClearColorImage( *commandBuffer
					, texture.image
					, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
					, &transparentBlackClearColor.color
					, 1u
					, &texture.wholeViewId.data->info.subresourceRange );
			}

			auto shaderBarrier = makeVkStruct< VkImageMemoryBarrier >( 0u
				, VkAccessFlags( VK_ACCESS_SHADER_READ_BIT )
				, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				, VK_QUEUE_FAMILY_IGNORED
				, VK_QUEUE_FAMILY_IGNORED
				, texture.image
				, texture.wholeViewId.data->info.subresourceRange );
			device->vkCmdPipelineBarrier( *commandBuffer
				, VK_PIPELINE_STAGE_TRANSFER_BIT
				, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
				, VK_DEPENDENCY_BY_REGION_BIT
				, 0u
				, nullptr
				, 0u
				, nullptr
				, 1u
				, &shaderBarrier );
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
			&& updater.index >= myPasses.runnables.size() )
		{
			auto passes = doCreatePass( updater.index, vsm, rsm );

			for ( auto & pass : passes )
			{
				myPasses.passes.emplace_back( std::move( pass ) );
			}

			myPasses.runnables.push_back( myPasses.graphs.back()->compile( m_device.makeContext() ) );
			auto runnable = myPasses.runnables.back().get();
			printGraph( *runnable );
			runnable->record();
		}

		doUpdate( updater );
	}

	void ShadowMap::accept( PipelineVisitorBase & visitor )
	{
		for ( uint32_t i = 1u; i < uint32_t( SmTexture::eCount ); ++i )
		{
			uint32_t index = 0u;
			auto & result = m_result[SmTexture( i )];

			for ( auto & view : result.subViewsId )
			{
				visitor.visit( m_name + getTexName( SmTexture( i ) ) + cuT( "L" ) + castor::string::toString( index++ )
					, view
					, ( ashes::isDepthOrStencilFormat( view.data->info.format )
						? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
						: VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL )
					, TextureFactors{}.invert( true ) );
			}
		}
	}

	crg::SemaphoreWaitArray ShadowMap::render( crg::SemaphoreWaitArray const & toWait
		, ashes::Queue const & queue
		, uint32_t index )
	{
#if !C3D_MeasureShadowMapImpact
		if ( doIsUpToDate( index )
			&& getEngine()->areUpdateOptimisationsEnabled() )
		{
			return toWait;
		}
#endif

		auto & myPasses = m_passes[m_passesIndex];

		if ( !myPasses.runnables[index] )
		{
			myPasses.runnables[index] = myPasses.graphs[index]->compile( m_device.makeContext() );
			auto runnable = myPasses.runnables[index].get();
			printGraph( *runnable );
			runnable->record();
		}

		doSetUpToDate( index );
		return myPasses.runnables[index]->run( toWait, queue );
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
}
