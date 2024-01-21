#include "Castor3D/Render/Overlays/OverlayPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/OverlayCache.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Overlay/OverlayCategory.hpp"
#include "Castor3D/Render/RenderInfo.hpp"
#include "Castor3D/Render/Overlays/OverlayPreparer.hpp"
#include "Castor3D/Render/Overlays/OverlayRenderer.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include <RenderGraph/RunnableGraph.hpp>
#include <RenderGraph/GraphContext.hpp>

namespace castor3d
{
	namespace passovy
	{
		static OverlaysCounts parseOverlays( OverlayCache const & cache
			, OverlayRenderer const & renderer
			, OverlayPreparer & preparer )
		{
			OverlaysCounts result{};
			auto lock( castor::makeUniqueLock( cache ) );
			std::vector< OverlayCategory * > categories;
			categories.reserve( cache.getCategories().size() );

			for ( auto category : cache.getCategories() )
			{
				if ( category->getOverlay().isVisible() )
				{
					categories.push_back( category );

					if ( category->hasAnyChange() || renderer.isSizeChanged() )
					{
						category->update( renderer );
					}
				}
			}

			for ( auto category : categories )
			{
				if ( category->getOverlay().isDisplayable() )
				{
					auto counts = preparer.registerOverlay( category->getOverlay() );
					result.drawCalls += counts.drawCalls;
					result.overlays += counts.overlays;
					result.quads += counts.quads;
				}
			}

			for ( auto category : categories )
			{
				if ( category->hasAnyChange() || renderer.isSizeChanged() )
				{
					category->reset();
				}
			}

			return result;
		}
	}

	OverlayPass::OverlayPass( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, Scene const & scene
		, VkExtent2D const & size
		, Texture const & output
		, HdrConfigUbo const & hdrConfigUbo
		, bool drawGlobal )
		: crg::RunnablePass{ pass
			, context
			, graph
			, crg::RunnablePass::Callbacks{ []( uint32_t ){}
				, crg::defaultV< crg::RunnablePass::GetPipelineStateCallback >
				, [this]( crg::RecordContext & ctx, VkCommandBuffer cb, uint32_t ){ doRecordInto( ctx, cb ); } }
			, { 1u, true } }
		, m_device{ device }
		, m_scene{ scene }
		, m_renderPass{ pass
			, context
			, graph
			, 1u
			, size }
		, m_renderer{ castor::makeUnique< OverlayRenderer >( device, output, hdrConfigUbo, m_timer, VK_COMMAND_BUFFER_LEVEL_SECONDARY ) }
		, m_drawGlobal{ drawGlobal }
	{
	}

	void OverlayPass::update( CpuUpdater & updater )
	{
		resetCommandBuffer( 0u );
		{
			m_counts = {};
			auto preparer = m_renderer->beginPrepare( m_device
				, m_renderPass.getRenderPass( 0u )
				, m_renderPass.getFramebuffer( 0u )
				, m_graph.getFence() );
			preparer.setDrawCounts( m_counts.drawCalls );

			if ( m_drawGlobal )
			{
				auto counts = passovy::parseOverlays( m_scene.getEngine()->getOverlayCache()
					, *m_renderer
					, preparer );
				m_counts.overlays += counts.overlays;
				m_counts.quads += counts.quads;
			}

			auto counts = passovy::parseOverlays( m_scene.getOverlayCache()
				, *m_renderer
				, preparer );
			m_counts.overlays += counts.overlays;
			m_counts.quads += counts.quads;
		}
		reRecordCurrent();
	}

	void OverlayPass::update( GpuUpdater & updater )
	{
		updater.info.visibleOverlaysCount = m_counts.overlays;
		updater.info.visibleOverlayQuadsCount = m_counts.quads;
		updater.info.drawCalls += m_counts.drawCalls;
		m_renderer->update( updater );
	}

	void OverlayPass::upload( UploadData & uploader )
	{
		m_renderer->upload( uploader );
	}

	void OverlayPass::doRecordInto( crg::RecordContext & context
		, VkCommandBuffer commandBuffer )
	{
		if ( m_renderPass.initialise( context, *this, 0u ) )
		{
			m_renderer->beginPrepare( m_device
				, m_renderPass.getRenderPass( 0u )
				, m_renderPass.getFramebuffer( 0u )
				, m_graph.getFence() );
		}

		m_renderer->registerComputeCommands( context, commandBuffer );
		VkCommandBuffer secondary = m_renderer->getCommands();
		m_renderPass.begin( context
			, commandBuffer
			, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS
			, 0u );
		m_context.vkCmdExecuteCommands( commandBuffer
			, 1u
			, &secondary );
		m_renderPass.end( context
			, commandBuffer );
	}
}
