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
		, m_size{ size }
		, m_renderer{ castor::makeUnique< OverlayRenderer >( device, output, VK_COMMAND_BUFFER_LEVEL_SECONDARY ) }
	{
	}

	OverlayPass::~OverlayPass()
	{
	}

	void OverlayPass::update( CpuUpdater & updater )
	{
		resetCommandBuffer();
		m_renderer->beginPrepare( m_renderPass.getRenderPass()
			, m_renderPass.getFramebuffer( 0u ) );
		auto preparer = m_renderer->getPreparer( m_device );

		for ( auto category : m_scene.getEngine()->getOverlayCache() )
		{
			SceneSPtr scene = category->getOverlay().getScene();

			if ( category->getOverlay().isVisible()
				&& scene
				&& scene.get() == &m_scene )
			{
				category->update( *m_renderer );
				category->accept( preparer );
			}
		}

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
