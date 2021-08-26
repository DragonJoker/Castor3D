#if defined( CU_CompilerMSVC )
#	pragma warning( disable:4503 )
#endif

#include "Castor3D/Render/RenderQueue.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderPass.hpp"
#include "Castor3D/Render/Culling/SceneCuller.hpp"
#include "Castor3D/Render/Node/QueueCulledRenderNodes.hpp"

#include <ashespp/Command/CommandBufferInheritanceInfo.hpp>

using namespace castor;

using ashes::operator==;
using ashes::operator!=;

namespace castor3d
{
	RenderQueue::RenderQueue( SceneRenderPass & renderPass
		, RenderMode mode
		, SceneNode const * ignored )
		: OwnedBy< SceneRenderPass >{ renderPass }
		, m_ignoredNode{ ignored }
		, m_culler{ getOwner()->getCuller() }
		, m_onCullerCompute( m_culler.onCompute.connect( [this]( SceneCuller const & culler )
			{
				doOnCullerCompute( culler );
			} ) )
		, m_renderNodes{ castor::makeUnique< QueueRenderNodes >( *this ) }
		, m_culledRenderNodes{ castor::makeUnique < QueueCulledRenderNodes >( *this ) }
		, m_viewport{ castor::makeChangeTracked< ashes::Optional< VkViewport > >( m_culledChanged, ashes::nullopt ) }
		, m_scissor{ castor::makeChangeTracked< ashes::Optional< VkRect2D > >( m_culledChanged, ashes::nullopt ) }
		, m_commandBuffer{ renderPass.getEngine()->getRenderSystem()->getRenderDevice().graphicsData()->commandPool->createCommandBuffer( renderPass.getName()
			, VK_COMMAND_BUFFER_LEVEL_SECONDARY ) }
	{
	}

	void RenderQueue::initialise()
	{
		getCommandBuffer().begin( VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT
			, makeVkType< VkCommandBufferInheritanceInfo >( VkRenderPass( getOwner()->getRenderPass() )
				, 0u
				, VkFramebuffer( VK_NULL_HANDLE )
				, VkBool32( VK_FALSE )
				, 0u
				, 0u ) );
		getCommandBuffer().end();
	}

	void RenderQueue::cleanup()
	{
		m_culledRenderNodes.reset();
		m_renderNodes.reset();
		m_commandBuffer.reset();
	}

	void RenderQueue::update( ShadowMapLightTypeArray & shadowMaps )
	{
		if ( m_allChanged )
		{
			doParseAllRenderNodes( shadowMaps );
			m_allChanged = false;
		}

		bool commandBuffersChanged = m_culledChanged;

		if ( m_culledChanged )
		{
			doParseCulledRenderNodes();
			m_culledChanged = false;
		}

		if ( commandBuffersChanged )
		{
			// Force regeneration of CommandBuffers if running.
			m_preparation = ( m_preparation == Preparation::eWaiting )
				? Preparation::eWaiting
				: Preparation::eDone;
		}

		if ( commandBuffersChanged
			&& m_preparation == Preparation::eDone )
		{
			m_preparation = Preparation::eWaiting;
			getOwner()->getEngine()->sendEvent( makeGpuFunctorEvent( EventType::ePreRender
				, [this]( RenderDevice const & device
					, QueueData const & queueData )
				{
					m_preparation = Preparation::eRunning;
					doPrepareCommandBuffer();
					m_preparation = ( m_preparation == Preparation::eWaiting )
						? Preparation::eWaiting
						: Preparation::eDone;
				} ) );
		}
	}

	void RenderQueue::update( ShadowMapLightTypeArray & shadowMaps
		, VkViewport const & viewport
		, VkRect2D const & scissor )
	{
		m_viewport = viewport;
		m_scissor = scissor;
		update( shadowMaps );
	}

	void RenderQueue::update( ShadowMapLightTypeArray & shadowMaps
		, VkRect2D const & scissor )
	{
		m_scissor = scissor;
		update( shadowMaps );
	}

	void RenderQueue::setIgnoredNode( SceneNode const & node )
	{
		m_allChanged = m_allChanged || ( m_ignoredNode != &node );
		m_ignoredNode = &node;
	}

	bool RenderQueue::hasNodes()const
	{
		return ( m_renderNodes
			? m_renderNodes->hasNodes()
			: false );
	}

	RenderMode RenderQueue::getMode()const
	{
		return getOwner()->getRenderMode();
	}

	void RenderQueue::doPrepareCommandBuffer()
	{
		getOwner()->resetCommandBuffer();
		m_commandBuffer->reset();
		auto & culledNodes = getCulledRenderNodes();
		culledNodes.prepareCommandBuffers( *this
			, m_viewport.value()
			, m_scissor.value() );
		getOwner()->recordCurrent();
	}

	void RenderQueue::doParseAllRenderNodes( ShadowMapLightTypeArray & shadowMaps )
	{
		auto & allNodes = getAllRenderNodes();
		allNodes.parse( shadowMaps );
	}

	void RenderQueue::doParseCulledRenderNodes()
	{
		auto & culledNodes = getCulledRenderNodes();
		culledNodes.parse();
	}

	void RenderQueue::doOnCullerCompute( SceneCuller const & culler )
	{
		m_allChanged = m_allChanged || culler.areAllChanged();
		m_culledChanged = m_allChanged || culler.areCulledChanged();
	}
}
