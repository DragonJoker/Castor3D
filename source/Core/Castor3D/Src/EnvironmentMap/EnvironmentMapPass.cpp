#include "EnvironmentMapPass.hpp"

#include "EnvironmentMap/EnvironmentMap.hpp"
#include "Render/Viewport.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Scene.hpp"

#include <Command/CommandBuffer.hpp>
#include <Command/Queue.hpp>
#include <Descriptor/DescriptorSet.hpp>
#include <Descriptor/DescriptorSetPool.hpp>
#include <Sync/Fence.hpp>

using namespace castor;

namespace castor3d
{
	namespace
	{
		CameraSPtr doCreateCamera( SceneNode & node )
		{
			Viewport viewport{ *node.getScene()->getEngine() };
			auto camera = std::make_shared< Camera >( cuT( "EnvironmentMap_" ) + node.getName()
				, *node.getScene()
				, node.shared_from_this()
				, std::move( viewport ) );
			camera->update();
			return camera;
		}

		renderer::TextureViewPtr doCreateView( TextureView const & image
			, uint32_t face )
		{
			renderer::ImageViewCreateInfo view{};
			view.format = image.getView().getFormat();
			view.viewType = renderer::TextureViewType::e2D;
			view.subresourceRange.aspectMask = renderer::ImageAspectFlag::eColour;
			view.subresourceRange.baseArrayLayer = face;
			view.subresourceRange.layerCount = 1u;
			view.subresourceRange.baseMipLevel = 0u;
			view.subresourceRange.levelCount = 1u;
			return image.getOwner()->getTexture().createView( view );
		}
	}

	EnvironmentMapPass::EnvironmentMapPass( EnvironmentMap & reflectionMap
		, SceneNodeSPtr node
		, SceneNode const & objectNode )
		: OwnedBy< EnvironmentMap >{ reflectionMap }
		, m_node{ node }
		, m_camera{ doCreateCamera( *node ) }
		, m_opaquePass{ std::make_unique< ForwardRenderTechniquePass >( cuT( "environment_opaque" )
			, *node->getScene()
			, m_camera.get()
			, true
			, &objectNode
			, SsaoConfig{} ) }
		, m_transparentPass{ std::make_unique< ForwardRenderTechniquePass >( cuT( "environment_transparent" )
			, *node->getScene()
			, m_camera.get()
			, true
			, true
			, &objectNode
			, SsaoConfig{} ) }
		, m_mtxView{ m_camera->getView() }
		, m_matrixUbo{ *reflectionMap.getEngine() }
		, m_modelMatrixUbo{ *reflectionMap.getEngine() }
		, m_hdrConfigUbo{ *reflectionMap.getEngine() }
	{
	}

	EnvironmentMapPass::~EnvironmentMapPass()
	{
	}

	bool EnvironmentMapPass::initialise( Size const & size
		, uint32_t face
		, renderer::RenderPass const & renderPass
		, SceneBackground const & background
		, renderer::DescriptorSetPool const & pool )
	{
		auto & device = *getOwner()->getEngine()->getRenderSystem()->getCurrentDevice();
		real const aspect = real( size.getWidth() ) / size.getHeight();
		real const near = 0.1_r;
		real const far = 1000.0_r;
		m_camera->getViewport().setPerspective( 90.0_degrees
			, aspect
			, near
			, far );
		m_camera->resize( size );

		static Matrix4x4r const projection = convert( device.perspective( ( 45.0_degrees ).radians()
			, 1.0f
			, 0.0f, 2.0f ) );
		m_matrixUbo.initialise();
		m_matrixUbo.update( m_mtxView, projection );
		m_modelMatrixUbo.initialise();
		m_hdrConfigUbo.initialise();
		auto const & environmentLayout = getOwner()->getTexture().getTexture();
		m_envView = doCreateView( environmentLayout->getImage( face ), face );
		auto const & depthView = getOwner()->getDepthView();

		// Initialise opaque pass.
		m_opaquePass->initialiseRenderPass( *m_envView
			, getOwner()->getDepthView()
			, size
			, true );
		m_opaquePass->initialise( size );

		// Create custom background pass.
		renderer::FrameBufferAttachmentArray attaches;
		attaches.emplace_back( *( renderPass.getAttachments().begin() + 0u ), depthView );
		attaches.emplace_back( *( renderPass.getAttachments().begin() + 1u ), *m_envView );
		m_frameBuffer = renderPass.createFrameBuffer( renderer::Extent2D{ size[0], size[1] }
			, std::move( attaches ) );
		m_backgroundCommands = device.getGraphicsCommandPool().createCommandBuffer( false );
		auto & commandBuffer = *m_backgroundCommands;
		m_renderPass = &renderPass;
		auto & descriptorLayout = pool.getLayout();
		m_backgroundDescriptorSet = pool.createDescriptorSet( 0u );
		background.initialiseDescriptorSet( m_matrixUbo
			, m_modelMatrixUbo
			, m_hdrConfigUbo
			, *m_backgroundDescriptorSet );
		m_backgroundDescriptorSet->update();
		background.prepareFrame( commandBuffer
			, size
			, renderPass
			, *m_backgroundDescriptorSet );

		// Initialise transparent pass.
		m_transparentPass->initialiseRenderPass( *m_envView
			, getOwner()->getDepthView()
			, size
			, false );
		m_transparentPass->initialise( size );

		m_commandBuffer = device.getGraphicsCommandPool().createCommandBuffer();
		m_finished = device.createSemaphore();
		m_fence = device.createFence( renderer::FenceCreateFlag::eSignaled );
		return true;
	}

	void EnvironmentMapPass::cleanup()
	{
		m_fence.reset();
		m_finished.reset();
		m_commandBuffer.reset();
		m_backgroundCommands.reset();
		m_backgroundDescriptorSet.reset();
		m_frameBuffer.reset();
		m_opaquePass->cleanup();
		m_transparentPass->cleanup();
		m_envView.reset();
		m_hdrConfigUbo.cleanup();
		m_modelMatrixUbo.cleanup();
		m_matrixUbo.cleanup();
	}

	void EnvironmentMapPass::update( SceneNode const & node, RenderQueueArray & queues )
	{
		auto position = node.getDerivedPosition();
		m_camera->getParent()->setPosition( position );
		m_camera->getParent()->update();
		m_camera->update();
		castor::matrix::setTranslate( m_mtxModel, position );
		castor::matrix::scale( m_mtxModel, Point3r{ 1, -1, 1 } );
		static_cast< RenderTechniquePass & >( *m_opaquePass ).update( queues );
		static_cast< RenderTechniquePass & >( *m_transparentPass ).update( queues );
	}

	renderer::Semaphore const & EnvironmentMapPass::render( renderer::Semaphore const & toWait )
	{
		auto & device = *getOwner()->getEngine()->getRenderSystem()->getCurrentDevice();
		RenderInfo info;
		m_opaquePass->update( info, {} );
		m_transparentPass->update( info, {} );
		m_matrixUbo.update( m_camera->getView()
			, m_camera->getViewport().getProjection() );
		m_modelMatrixUbo.update( m_mtxModel );
		renderer::Semaphore const * result = &toWait;

		static renderer::ClearColorValue const black{};
		static renderer::DepthStencilClearValue const depth{ 1.0, 0 };

		m_commandBuffer->begin();
		m_commandBuffer->beginRenderPass( *m_renderPass
			, *m_frameBuffer
			, { depth, black }
			, renderer::SubpassContents::eSecondaryCommandBuffers );
		renderer::CommandBufferCRefArray commandBuffers;

		if ( m_opaquePass->hasNodes() )
		{
			commandBuffers.emplace_back( m_opaquePass->getCommandBuffer() );
		}

		commandBuffers.emplace_back( *m_backgroundCommands );

		if ( m_transparentPass->hasNodes() )
		{
			commandBuffers.emplace_back( m_transparentPass->getCommandBuffer() );
		}

		m_commandBuffer->executeCommands( commandBuffers );
		m_commandBuffer->endRenderPass();
		m_commandBuffer->end();

		device.getGraphicsQueue().submit( *m_commandBuffer
			, toWait
			, renderer::PipelineStageFlag::eBottomOfPipe
			, *m_finished
			, nullptr );
		return *m_finished;
	}
}
