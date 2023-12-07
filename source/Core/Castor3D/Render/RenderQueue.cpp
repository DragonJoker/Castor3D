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

CU_ImplementSmartPtr( castor3d, RenderQueue )

namespace castor3d
{
	//*********************************************************************************************

	RenderQueue::PassData::~PassData()noexcept
	{
		auto lock( castor::makeUniqueLock( eventMutex ) );

		if ( initEvent )
		{
			initEvent->skip();
			initEvent = {};
		}
	}

	void RenderQueue::PassData::initialise( RenderDevice const & device
		, QueueData const & queueData
		, castor::String const & name
		, VkRenderPass renderPass )
	{
		renderPassAtInit = renderPass;
		commandBuffer = queueData.commandPool->createCommandBuffer( name
			, VK_COMMAND_BUFFER_LEVEL_SECONDARY );
		commandBuffer->begin( VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT
			, makeVkStruct< VkCommandBufferInheritanceInfo >( renderPassAtInit
				, 0u
				, VkFramebuffer( nullptr )
				, VK_FALSE
				, 0u
				, 0u ) );
		commandBuffer->end();
		initialised = true;
		initEvent = nullptr;
	}

	//*********************************************************************************************

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
		, m_pass{ std::make_unique< PassData >() }
		, m_currentPass{ m_pass.get() }
		, m_viewport{ castor::makeGroupChangeTracked< ashes::Optional< VkViewport > >( m_culledChanged, ashes::nullopt ) }
		, m_scissor{ castor::makeGroupChangeTracked< ashes::Optional< VkRect2D > >( m_culledChanged, ashes::nullopt ) }
	{
	}

	RenderQueue::~RenderQueue()
	{
	}

	void RenderQueue::initialise()
	{
	}

	void RenderQueue::invalidate()
	{
		m_toDelete = std::move( m_pass );
		m_pass = std::make_unique< PassData >();
		m_currentPass = m_pass.get();
		m_invalidated = true;
		m_commandsChanged = true;
	}

	void RenderQueue::cleanup()
	{
		CU_Require( m_renderNodes );
		m_toDelete = std::move( m_pass );
	}

	void RenderQueue::update( ShadowMapLightTypeArray & shadowMaps
		, ShadowBuffer const * shadowBuffer )
	{
		if ( hasCommandBuffer() )
		{
			m_toDelete.reset();

			if ( m_culledChanged )
			{
				CU_Require( m_renderNodes );

				if ( m_fullSort )
				{
					m_culledChanged = !m_renderNodes->sortNodes( shadowMaps, shadowBuffer );
					m_fullSort = false;
				}
				else
				{
					m_culledChanged = !m_renderNodes->updateNodes( shadowMaps, shadowBuffer );
				}

				m_commandsChanged = true;
			}

			if ( m_commandsChanged )
			{
				doPrepareCommandBuffer();
				m_commandsChanged = m_fullSort;
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
		, ShadowBuffer const * shadowBuffer
		, VkViewport const & viewport
		, VkRect2D const & scissor )
	{
		m_viewport = viewport;
		m_scissor = scissor;
		update( shadowMaps, shadowBuffer );
	}

	void RenderQueue::update( ShadowMapLightTypeArray & shadowMaps
		, ShadowBuffer const * shadowBuffer
		, VkRect2D const & scissor )
	{
		m_scissor = scissor;
		m_commandsChanged = m_commandsChanged || m_scissor.isDirty();
		update( shadowMaps, shadowBuffer );
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

	bool RenderQueue::needsInitialise()const
	{
		auto & pass = *m_currentPass;
		auto lock( castor::makeUniqueLock( pass.eventMutex ) );
		return !pass.initEvent
			&& !pass.initialised;
	}

	RenderFilters RenderQueue::getFilters()const
	{
		return getOwner()->getRenderFilters();
	}

	ashes::CommandBuffer const & RenderQueue::initCommandBuffer()
	{
		if ( hasCommandBuffer()
			&& m_currentPass->renderPassAtInit == getOwner()->getRenderPass( 0u ) )
		{
			return getCommandBuffer();
		}

		if ( hasCommandBuffer() )
		{
			m_toDelete = std::move( m_pass );
			m_pass = std::make_unique< PassData >();
			m_currentPass = m_pass.get();
		}

		auto & device = *getOwner()->getEngine()->getRenderDevice();
		auto queueData = device.graphicsData();
		m_currentPass->initialise( device
			, *queueData
			, getOwner()->getName(), getOwner()->getRenderPass( 0u ) );
		return getCommandBuffer();
	}

	RenderCounts const & RenderQueue::getVisibleCounts()const
	{
		return m_renderNodes->getVisibleCounts();
	}

	void RenderQueue::doInitialise( RenderDevice const & device
		, QueueData const & queueData
		, PassData & pass )
	{
		invalidate();
	}

	void RenderQueue::doPrepareCommandBuffer()
	{
		getOwner()->resetCommandBuffer( 0u );

		if ( m_invalidated.exchange( false ) )
		{
			// Do this here to prevent use of deleted data in command buffer.
			getRenderNodes().clear();
			getOwner()->cleanupPipelines();
			initCommandBuffer();
			m_culledChanged = true;
			m_fullSort = true;
		}
		else
		{
			auto & pass = *m_currentPass;
			CU_Require( pass.commandBuffer );
			CU_Require( m_renderNodes );
			pass.commandBuffer->reset();
			m_drawCalls = m_renderNodes->prepareCommandBuffers( m_viewport.value()
				, m_scissor.value()
				, *pass.commandBuffer );
		}

		getOwner()->reRecordCurrent();
	}

	void RenderQueue::doOnCullerCompute( SceneCuller const & culler )
	{
		m_culledChanged = m_culledChanged || culler.areCulledChanged();
		m_commandsChanged = m_commandsChanged || m_culledChanged;
	}

	//*********************************************************************************************
}
