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
		auto & pass = m_pass;
		auto lock( castor::makeUniqueLock( pass.eventMutex ) );

		if ( pass.initEvent )
		{
			pass.initEvent->skip();
			pass.initEvent = {};
		}
	}

	void RenderQueue::initialise()
	{
		if ( auto event = getOwner()->getEngine()->postEvent( makeGpuFunctorEvent( EventType::ePreRender
			, [this]( RenderDevice const & device
				, QueueData const & queueData )
			{
				auto & pass = m_pass;
				auto lock( castor::makeUniqueLock( pass.eventMutex ) );

				if ( pass.initEvent )
				{
					doInitialise( device, queueData );
				}
			} ) ) )
		{
			auto & pass = m_pass;
			auto lock( castor::makeUniqueLock( pass.eventMutex ) );
			pass.initEvent = event;
		}
	}

	void RenderQueue::cleanup()
	{
		CU_Require( m_renderNodes );
		m_renderNodes->cleanup();
		m_pass.commandBuffer.reset();
	}

	void RenderQueue::update( ShadowMapLightTypeArray & shadowMaps
		, ShadowBuffer const * shadowBuffer )
	{
		if ( hasCommandBuffer() )
		{
			if ( m_culledChanged )
			{
				CU_Require( m_renderNodes );
				m_renderNodes->sortNodes( shadowMaps, shadowBuffer );
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
		auto & pass = m_pass;
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
		auto & pass = m_pass;

		if ( hasCommandBuffer()
			&& pass.renderPassAtInit == getOwner()->getRenderPass( 0u ) )
		{
			return getCommandBuffer();
		}

		{
			auto lock( castor::makeUniqueLock( pass.eventMutex ) );

			if ( pass.initEvent )
			{
				pass.initEvent->skip();
				pass.initEvent = nullptr;
			}
		}

		auto & device = getOwner()->getEngine()->getRenderSystem()->getRenderDevice();
		auto queueData = device.graphicsData();
		doInitialise( device, *queueData );
		return getCommandBuffer();
	}

	void RenderQueue::doInitialise( RenderDevice const & device
		, QueueData const & queueData )
	{
		auto & pass = m_pass;
		pass.commandBuffer.reset();
		pass.renderPassAtInit = getOwner()->getRenderPass( 0u );
		pass.commandBuffer = queueData.commandPool->createCommandBuffer( getOwner()->getName()
			, VK_COMMAND_BUFFER_LEVEL_SECONDARY );
		pass.commandBuffer->begin( VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT
			, makeVkStruct< VkCommandBufferInheritanceInfo >( pass.renderPassAtInit
				, 0u
				, VkFramebuffer( nullptr )
				, VK_FALSE
				, 0u
				, 0u ) );
		pass.commandBuffer->end();
		pass.initialised = true;
		pass.initEvent = nullptr;

		m_renderNodes->initialise( device );
		m_culledChanged = true;
		m_commandsChanged = true;
	}

	void RenderQueue::doPrepareCommandBuffer()
	{
		getOwner()->resetCommandBuffer( 0u );
		auto & pass = m_pass;
		CU_Require( pass.commandBuffer );
		CU_Require( m_renderNodes );
		pass.commandBuffer->reset();
		m_drawCalls = m_renderNodes->prepareCommandBuffers( m_viewport.value()
			, m_scissor.value() );
		getOwner()->reRecordCurrent();
	}

	void RenderQueue::doOnCullerCompute( SceneCuller const & culler )
	{
		m_culledChanged = m_culledChanged || culler.areCulledChanged();
		m_commandsChanged = m_commandsChanged || m_culledChanged;
	}
}
