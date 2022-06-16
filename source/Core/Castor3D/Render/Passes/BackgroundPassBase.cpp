#include "Castor3D/Render/Passes/BackgroundPassBase.hpp"

#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Background/Background.hpp"

#include <RenderGraph/FrameGraph.hpp>
#include <RenderGraph/GraphContext.hpp>
#include <RenderGraph/RunnableGraph.hpp>

#include <ashes/ashes.hpp>

namespace castor3d
{
	//*********************************************************************************************

	BackgroundPassBase::BackgroundPassBase( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, SceneBackground & background
		, VkExtent2D const & size
		, crg::ImageViewId const * depth )
		: crg::RenderPass{ pass
			, context
			, graph
			, { [this](){ doSubInitialise(); }
				, [this]( crg::RecordContext & context, VkCommandBuffer cb, uint32_t i ){ doSubRecordInto( context, cb, i ); }
				, crg::defaultV< crg::RenderPass::GetSubpassContentsCallback >
				, crg::defaultV< crg::RenderPass::GetPassIndexCallback >
				, IsEnabledCallback( [this](){ return doIsEnabled(); } ) }
			, size
			, { 1u, true } }
		, m_device{ device }
		, m_background{ &background }
		, m_size{ size }
		, m_depth{ depth }
		, m_usesDepth{ m_depth != nullptr }
		, m_viewport{ *device.renderSystem.getEngine() }
		, m_onBackgroundChanged{ background.onChanged.connect( [this]( SceneBackground const & background )
			{
				doResetPipeline();
			} ) }
	{
	}

	void BackgroundPassBase::update( CpuUpdater & updater )
	{
		updater.viewport = &m_viewport;
		m_background->update( updater );
	}

	void BackgroundPassBase::update( GpuUpdater & updater )
	{
		m_background->update( updater );
	}

	bool BackgroundPassBase::doIsEnabled()const
	{
		return m_background->isInitialised();
	}

	ashes::PipelineViewportStateCreateInfo BackgroundPassBase::doCreateViewportState()
	{
		ashes::VkViewportArray viewports( 1u
			, { 0.0f
			, 0.0f
			, float( m_size.width )
			, float( m_size.height )
			, 0.0f
			, 1.0f } );
		ashes::VkScissorArray scissors( 1u
			, { 0
			, 0
			, m_size.width
			, m_size.height } );
		return { 0u
			, uint32_t( viewports.size() )
			, viewports
			, uint32_t( scissors.size() )
			, scissors };
	}

	void BackgroundPassBase::doSubInitialise()
	{
		doInitialiseVertexBuffer();

		if ( doIsEnabled() )
		{
			if ( m_descriptorSetLayout )
			{
				return;
			}

			doFillDescriptorBindings();
			m_descriptorSetLayout = m_device->createDescriptorSetLayout( m_pass.getGroupName()
				, m_descriptorBindings );
			m_pipelineLayout = m_device->createPipelineLayout( m_pass.getGroupName()
				, *m_descriptorSetLayout );
			m_descriptorSetPool = m_descriptorSetLayout->createPool( 1u );
			doCreateDescriptorSet();
			doCreatePipeline();
		}
	}

	void BackgroundPassBase::doSubRecordInto( crg::RecordContext & context
		, VkCommandBuffer commandBuffer
		, uint32_t index )
	{
		if ( doIsEnabled() )
		{
			VkDeviceSize offset{};
			VkDescriptorSet descriptorSet = *m_descriptorSet;
			VkBuffer vbo = m_vertexBuffer->getBuffer();
			m_context.vkCmdBindPipeline( commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline );
			m_context.vkCmdBindDescriptorSets( commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipelineLayout, 0u, 1u, &descriptorSet, 0u, nullptr );
			m_context.vkCmdBindVertexBuffers( commandBuffer, 0u, 1u, &vbo, &offset );

			if ( m_indexBuffer )
			{
				m_context.vkCmdBindIndexBuffer( commandBuffer, m_indexBuffer->getBuffer(), 0u, VK_INDEX_TYPE_UINT16 );
				m_context.vkCmdDrawIndexed( commandBuffer, uint32_t( m_indexBuffer->getCount() ), 1u, 0u, 0u, 0u );
			}
			else
			{
				m_context.vkCmdDraw( commandBuffer, uint32_t( m_vertexBuffer->getCount() ), 1u, 0u, 0u );
			}
		}
	}

	void BackgroundPassBase::doCreateDescriptorSet()
	{
		m_descriptorSet = m_descriptorSetPool->createDescriptorSet( m_pass.getGroupName() );

		for ( auto & write : m_descriptorWrites )
		{
			write.update( *m_descriptorSet );
		}

		m_descriptorSet->setBindings( crg::makeVkArray< VkWriteDescriptorSet >( m_descriptorWrites ) );
		m_descriptorSet->update();
	}

	void BackgroundPassBase::doResetPipeline()
	{
		if ( m_vertexBuffer )
		{
			resetCommandBuffer();
			m_descriptorSet.reset();
			m_descriptorSetPool.reset();
			m_pipeline.reset();
			m_pipelineLayout.reset();
			m_descriptorSetLayout.reset();
			m_descriptorBindings.clear();
			m_descriptorWrites.clear();
			doFillDescriptorBindings();
			m_descriptorSetLayout = m_device->createDescriptorSetLayout( m_pass.getGroupName()
				, m_descriptorBindings );
			m_pipelineLayout = m_device->createPipelineLayout( m_pass.getGroupName()
				, *m_descriptorSetLayout );
			m_descriptorSetPool = m_descriptorSetLayout->createPool( 1u );
			doCreateDescriptorSet();
			doCreatePipeline();
			reRecordCurrent();
		}
	}

	//*********************************************************************************************
}
