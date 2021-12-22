#include "Castor3D/Render/ShadowMap/ShadowMap.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapPass.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include <CastorUtils/Graphics/RgbaColour.hpp>

#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/Sync/Fence.hpp>
#include <ashespp/Sync/Semaphore.hpp>

CU_ImplementCUSmartPtr( castor3d, ShadowMap )

namespace castor3d
{
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
			VkImageMemoryBarrier transferBarrier{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER
				, nullptr
				, 0u
				, VK_ACCESS_TRANSFER_WRITE_BIT
				, VK_IMAGE_LAYOUT_UNDEFINED
				, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
				, VK_QUEUE_FAMILY_IGNORED
				, VK_QUEUE_FAMILY_IGNORED
				, texture.image
				, texture.wholeViewId.data->info.subresourceRange };
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

			VkImageMemoryBarrier shaderBarrier{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER
				, nullptr
				, 0u
				, VK_ACCESS_SHADER_READ_BIT
				, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				, VK_QUEUE_FAMILY_IGNORED
				, VK_QUEUE_FAMILY_IGNORED
				, texture.image
				, texture.wholeViewId.data->info.subresourceRange };
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
		if ( updater.index < doGetMaxCount()
			&& updater.index >= m_runnables.size() )
		{
			auto passes = doCreatePass( updater.index );

			for ( auto & pass : passes )
			{
				m_passes.emplace_back( std::move( pass ) );
			}

			m_runnables.push_back( m_graphs.back()->compile( m_device.makeContext() ) );
			auto runnable = m_runnables.back().get();
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
				visitor.visit( m_name + getName( SmTexture( i ) ) + cuT( "L" ) + castor::string::toString( index++ )
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
		if ( isUpToDate( index ) )
		{
			return toWait;	
		}
#endif

		if ( !m_runnables[index] )
		{
			m_runnables[index] = m_graphs[index]->compile( m_device.makeContext() );
			auto runnable = m_runnables[index].get();
			runnable->record();
		}

		return m_runnables[index]->run( toWait, queue );
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
