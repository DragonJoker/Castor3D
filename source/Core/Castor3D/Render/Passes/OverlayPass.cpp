#include "Castor3D/Render/Passes/OverlayPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/OverlayCache.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Overlay/OverlayCategory.hpp"
#include "Castor3D/Overlay/OverlayRenderer.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include <RenderGraph/RunnableGraph.hpp>
#include <RenderGraph/GraphContext.hpp>

namespace castor3d
{
	namespace
	{
		void doParseOverlay( Scene const & refScene
			, Overlay const & overlay
			, OverlayRenderer & renderer
			, OverlayRenderer::Preparer & preparer
			, std::set< Overlay const * > & visited )
		{
			auto ires = visited.insert( &overlay );

			if ( ires.second )
			{
				auto scene = overlay.getScene();

				if ( overlay.isVisible()
					&& scene
					&& scene == &refScene )
				{
					overlay.getCategory()->update( renderer );
					overlay.getCategory()->accept( preparer );

					for ( auto & child : overlay )
					{
						doParseOverlay( refScene
							, *child
							, renderer
							, preparer
							, visited );
					}
				}
			}
		}

		void doParseOverlays( Scene const & refScene
			, OverlayRenderer & renderer
			, OverlayRenderer::Preparer & preparer )
		{
			std::set< Overlay const * > visited;

			for ( auto category : refScene.getEngine()->getOverlayCache() )
			{
				doParseOverlay( refScene
					, category->getOverlay()
					, renderer
					, preparer
					, visited );
			}
		}
	}

	OverlayPass::OverlayPass( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, Scene const & scene
		, VkExtent2D const & size
		, Texture const & output )
		: crg::RunnablePass{ pass
			, context
			, graph
			, crg::RunnablePass::Callbacks{ [this](){ doInitialise(); }
				, crg::defaultV< crg::RunnablePass::GetSemaphoreWaitFlagsCallback >
				, [this]( VkCommandBuffer cb, uint32_t i ){ doRecordInto( cb, i ); } }
			, 1u }
		, m_device{ device }
		, m_scene{ scene }
		, m_renderPass{ pass
			, context
			, graph
			, 1u
			, size }
		, m_renderer{ castor::makeUnique< OverlayRenderer >( device, output, VK_COMMAND_BUFFER_LEVEL_SECONDARY ) }
	{
	}

	void OverlayPass::update( CpuUpdater & updater )
	{
		resetCommandBuffer();
		m_renderer->beginPrepare( m_renderPass.getRenderPass()
			, m_renderPass.getFramebuffer( 0u ) );
		auto preparer = m_renderer->getPreparer( m_device );
		doParseOverlays( m_scene, *m_renderer, preparer );
		m_renderer->endPrepare();
		recordCurrent();
	}

	void OverlayPass::update( GpuUpdater & updater )
	{
		m_renderer->update( updater );
	}

	void OverlayPass::doInitialise()
	{
		m_renderPass.initialise( *this );
		m_renderer->beginPrepare( m_renderPass.getRenderPass()
			, m_renderPass.getFramebuffer( 0u ) );
		m_renderer->endPrepare();
	}

	void OverlayPass::doRecordInto( VkCommandBuffer commandBuffer
		, uint32_t index )
	{
		VkCommandBuffer secondary = m_renderer->getCommands();
		m_renderPass.begin( commandBuffer
			, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS
			, 0u );
		m_context.vkCmdExecuteCommands( commandBuffer
			, 1u
			, &secondary );
		m_renderPass.end( commandBuffer );
	}
}
