#if defined( CU_CompilerMSVC )
#	pragma warning( disable:4503 )
#endif

#include "Castor3D/Render/RenderQueue.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderNodesPass.hpp"
#include "Castor3D/Render/Culling/SceneCuller.hpp"
#include "Castor3D/Render/Node/QueueRenderNodes.hpp"

using ashes::operator==;
using ashes::operator!=;

CU_ImplementCUSmartPtr( castor3d, RenderQueue )

namespace castor3d
{
	RenderQueue::RenderQueue( RenderNodesPass & renderPass
		, SceneNode const * ignored )
		: OwnedBy< RenderNodesPass >{ renderPass }
		, m_culler{ getOwner()->getCuller() }
		, m_onCullerCompute( m_culler.onCompute.connect( [this]( SceneCuller const & culler )
			{
				doOnCullerCompute( culler );
			} ) )
		, m_ignoredNode{ ignored }
		, m_renderNodes{ castor::makeUnique< QueueRenderNodes >( *this ) }
		, m_viewport{ castor::makeGroupChangeTracked< ashes::Optional< VkViewport > >( m_culledChanged, ashes::nullopt ) }
		, m_scissor{ castor::makeGroupChangeTracked< ashes::Optional< VkRect2D > >( m_culledChanged, ashes::nullopt ) }
	{
	}

	RenderQueue::~RenderQueue()
	{
		auto lock( castor::makeUniqueLock( m_eventMutex ) );

		if ( m_initEvent )
		{
			m_initEvent->skip();
		}
	}

	void RenderQueue::initialise()
	{
		if ( auto event = getOwner()->getEngine()->postEvent( makeGpuFunctorEvent( EventType::ePreRender
			, [this]( RenderDevice const & device
				, QueueData const & queueData )
			{
				auto lock( castor::makeUniqueLock( m_eventMutex ) );

				if ( m_initEvent )
				{
					doInitialise( device, queueData );
				}
			} ) ) )
		{
			auto lock( castor::makeUniqueLock( m_eventMutex ) );
			m_initEvent = event;
		}
	}

	bool RenderQueue::needsInitialise()const
	{
		return !m_initEvent
			&& !m_initialised;
	}

	void RenderQueue::cleanup()
	{
		CU_Require( m_renderNodes );
		m_renderNodes->cleanup();
		m_commandBuffer.reset();
	}

	void RenderQueue::update( ShadowMapLightTypeArray & shadowMaps )
	{
		if ( m_commandBuffer )
		{
			if ( m_culledChanged )
			{
				CU_Require( m_renderNodes );
				m_renderNodes->sortNodes( shadowMaps );
				m_culledChanged = false;
			}

			if ( m_commandsChanged )
			{
				doPrepareCommandBuffer();
				m_commandsChanged = false;
			}
		}
		else if ( m_culledChanged )
		{
			CU_Require( m_renderNodes );
			m_renderNodes->checkEmpty();
			m_culledChanged = false;
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
		m_commandsChanged = m_commandsChanged || m_scissor.isDirty();
		update( shadowMaps );
	}

	void RenderQueue::setIgnoredNode( SceneNode const & node )
	{
		m_culledChanged = m_culledChanged || ( m_ignoredNode != &node );
		m_ignoredNode = &node;
	}

	bool RenderQueue::hasNodes()const
	{
		return getCuller().hasNodes()
			&& m_renderNodes
			&& m_renderNodes->hasCulledNodes();
	}

	RenderFilters RenderQueue::getFilters()const
	{
		return getOwner()->getRenderFilters();
	}

	ashes::CommandBuffer const & RenderQueue::initCommandBuffer()
	{
		if ( hasCommandBuffer()
			&& m_renderPassAtInit == getOwner()->getRenderPass( 0u ) )
		{
			return getCommandBuffer();
		}

		{
			auto lock( castor::makeUniqueLock( m_eventMutex ) );

			if ( m_initEvent )
			{
				m_initEvent->skip();
				m_initEvent = nullptr;

				auto & device = getOwner()->getEngine()->getRenderSystem()->getRenderDevice();
				auto queueData = device.graphicsData();
				doInitialise( device, *queueData );
			}
		}

		return getCommandBuffer();
	}

	void RenderQueue::doInitialise( RenderDevice const & device
		, QueueData const & queueData )
	{
		m_commandBuffer.reset();
		m_renderPassAtInit = getOwner()->getRenderPass( 0u );
		m_commandBuffer = queueData.commandPool->createCommandBuffer( getOwner()->getName()
			, VK_COMMAND_BUFFER_LEVEL_SECONDARY );
		m_commandBuffer->begin( VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT
			, makeVkStruct< VkCommandBufferInheritanceInfo >( m_renderPassAtInit
				, 0u
				, VkFramebuffer( nullptr )
				, VK_FALSE
				, 0u
				, 0u ) );
		m_commandBuffer->end();
		m_renderNodes->initialise( device );
		m_culledChanged = true;
		m_commandsChanged = true;
		m_initialised = true;
		m_initEvent = nullptr;
	}

	void RenderQueue::doPrepareCommandBuffer()
	{
		getOwner()->resetCommandBuffer();
		CU_Require( m_commandBuffer );
		CU_Require( m_renderNodes );
		m_commandBuffer->reset();
		m_renderNodes->prepareCommandBuffers( m_viewport.value()
			, m_scissor.value() );
		getOwner()->reRecordCurrent();
	}

	void RenderQueue::doOnCullerCompute( SceneCuller const & culler )
	{
		m_culledChanged = m_culledChanged || culler.areCulledChanged();
		m_commandsChanged = m_commandsChanged || m_culledChanged;
	}
}
