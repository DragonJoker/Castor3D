#include "Castor3D/EnvironmentMap/EnvironmentMapPass.hpp"

#include "Castor3D/EnvironmentMap/EnvironmentMap.hpp"
#include "Castor3D/Miscellaneous/DebugName.hpp"
#include "Castor3D/Render/Culling/FrustumCuller.hpp"
#include "Castor3D/Render/Viewport.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Command/Queue.hpp>
#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Descriptor/DescriptorSetPool.hpp>
#include <ashespp/Sync/Fence.hpp>

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

		ashes::ImageView doCreateView( TextureView const & image
			, uint32_t face )
		{
			ashes::ImageViewCreateInfo view
			{
				0u,
				image.getOwner()->getTexture(),
				VK_IMAGE_VIEW_TYPE_2D,
				image.getView().getFormat(),
				VkComponentMapping{},
				{ VK_IMAGE_ASPECT_COLOR_BIT, 0u, 1u, face, 1u }
			};
			return image.getOwner()->getTexture().createView( view );
		}
	}

	EnvironmentMapPass::EnvironmentMapPass( EnvironmentMap & reflectionMap
		, SceneNodeSPtr node
		, SceneNode const & objectNode )
		: OwnedBy< EnvironmentMap >{ reflectionMap }
		, m_node{ node }
		, m_camera{ doCreateCamera( *node ) }
		, m_culler{ std::make_unique< FrustumCuller >( *m_camera->getScene(), *m_camera ) }
		, m_matrixUbo{ *reflectionMap.getEngine() }
		, m_opaquePass{ std::make_unique< ForwardRenderTechniquePass >( cuT( "environment_opaque" )
			, m_matrixUbo
			, *m_culler
			, true
			, &objectNode
			, SsaoConfig{} ) }
		, m_transparentPass{ std::make_unique< ForwardRenderTechniquePass >( cuT( "environment_transparent" )
			, m_matrixUbo
			, *m_culler
			, true
			, true
			, &objectNode
			, SsaoConfig{} ) }
		, m_mtxView{ m_camera->getView() }
		, m_modelMatrixUbo{ *reflectionMap.getEngine() }
		, m_hdrConfigUbo{ *reflectionMap.getEngine() }
	{
		castor::Logger::logTrace( "Created EnvironmentMapPass" + objectNode.getName() );
	}

	EnvironmentMapPass::~EnvironmentMapPass()
	{
	}

	bool EnvironmentMapPass::initialise( Size const & size
		, uint32_t face
		, ashes::RenderPass const & renderPass
		, SceneBackground const & background
		, ashes::DescriptorSetPool const & pool )
	{
		auto & device = getCurrentRenderDevice( *getOwner() );
		float const aspect = float( size.getWidth() ) / size.getHeight();
		float const nearZ = 0.1f;
		float const farZ = 1000.0f;
		m_camera->getViewport().setPerspective( 90.0_degrees
			, aspect
			, nearZ
			, farZ );
		m_camera->resize( size );

		static castor::Matrix4x4f const projection = convert( device->perspective( ( 45.0_degrees ).radians()
			, 1.0f
			, 0.0f, 2.0f ) );
		m_matrixUbo.initialise();
		m_matrixUbo.update( m_mtxView, projection );
		m_modelMatrixUbo.initialise();
		m_hdrConfigUbo.initialise();
		auto const & environmentLayout = getOwner()->getTexture().getTexture();
		m_envView = doCreateView( environmentLayout->getImage( face ), face );
		setDebugObjectName( device, m_envView, "EnvironmentMapPass" + castor::string::toString( face ) + "ColourView" );
		auto const & depthView = getOwner()->getDepthView();

		// Initialise opaque pass.
		m_opaquePass->initialiseRenderPass( m_envView
			, getOwner()->getDepthView()
			, size
			, true );
		m_opaquePass->initialise( size );

		// Create custom background pass.
		ashes::ImageViewCRefArray attaches;
		attaches.emplace_back( depthView );
		attaches.emplace_back( m_envView );
		m_frameBuffer = renderPass.createFrameBuffer( VkExtent2D{ size[0], size[1] }
			, std::move( attaches ) );
		setDebugObjectName( device, *m_frameBuffer, "EnvironmentMapPass" + castor::string::toString( face ) + "FrameBuffer" );
		m_backgroundCommands = device.graphicsCommandPool->createCommandBuffer( VK_COMMAND_BUFFER_LEVEL_SECONDARY );
		setDebugObjectName( device, *m_frameBuffer, "EnvironmentMapPass" + castor::string::toString( face ) + "CommandBuffer" );
		auto & commandBuffer = *m_backgroundCommands;
		m_renderPass = &renderPass;
		m_backgroundDescriptorSet = pool.createDescriptorSet( 0u );
		setDebugObjectName( device, *m_backgroundDescriptorSet, "EnvironmentMapPass" + castor::string::toString( face ) + "DescriptorSet" );
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
		m_transparentPass->initialiseRenderPass( m_envView
			, getOwner()->getDepthView()
			, size
			, false );
		m_transparentPass->initialise( size );

		m_commandBuffer = device.graphicsCommandPool->createCommandBuffer();
		m_finished = device->createSemaphore();
		return true;
	}

	void EnvironmentMapPass::cleanup()
	{
		m_finished.reset();
		m_commandBuffer.reset();
		m_backgroundCommands.reset();
		m_backgroundDescriptorSet.reset();
		m_frameBuffer.reset();
		m_opaquePass->cleanup();
		m_transparentPass->cleanup();
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
		castor::matrix::scale( m_mtxModel, castor::Point3f{ 1, -1, 1 } );
		m_culler->compute();
		static_cast< RenderTechniquePass & >( *m_opaquePass ).update( queues );
		static_cast< RenderTechniquePass & >( *m_transparentPass ).update( queues );
	}

	ashes::Semaphore const & EnvironmentMapPass::render( ashes::Semaphore const & toWait )
	{
		auto & device = getCurrentRenderDevice( *getOwner() );
		RenderInfo info;
		m_opaquePass->update( info, {} );
		m_transparentPass->update( info, {} );
		m_matrixUbo.update( m_camera->getView()
			, m_camera->getProjection() );
		m_modelMatrixUbo.update( m_mtxModel );
		ashes::Semaphore const * result = &toWait;

		m_commandBuffer->begin();
		m_commandBuffer->beginRenderPass( *m_renderPass
			, *m_frameBuffer
			, { defaultClearDepthStencil, opaqueBlackClearColor }
			, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS );
		ashes::CommandBufferCRefArray commandBuffers;

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

		device.graphicsQueue->submit( *m_commandBuffer
			, toWait
			, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
			, *m_finished
			, nullptr );
		result = m_finished.get();

		return *result;
	}
}
