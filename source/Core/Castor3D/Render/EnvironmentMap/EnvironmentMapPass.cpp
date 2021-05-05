#include "Castor3D/Render/EnvironmentMap/EnvironmentMapPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Material/Texture/TextureView.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/Viewport.hpp"
#include "Castor3D/Render/Culling/FrustumCuller.hpp"
#include "Castor3D/Render/EnvironmentMap/EnvironmentMap.hpp"
#include "Castor3D/Render/Technique/ForwardRenderTechniquePass.hpp"
#include "Castor3D/Render/Ssao/SsaoConfig.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Background/Background.hpp"

#include <CastorUtils/Graphics/RgbaColour.hpp>

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
				, node
				, std::move( viewport ) );
			camera->update();
			return camera;
		}

		ashes::ImageView doCreateView( castor::String const & name
			, TextureView const & image
			, uint32_t face )
		{
			ashes::ImageViewCreateInfo view
			{
				0u,
				image.getOwner()->getTexture(),
				VK_IMAGE_VIEW_TYPE_2D,
				image.getTargetView().getFormat(),
				VkComponentMapping{},
				{ VK_IMAGE_ASPECT_COLOR_BIT, 0u, 1u, face, 1u }
			};
			return image.getOwner()->getTexture().createView( name, view );
		}
	}

	EnvironmentMapPass::EnvironmentMapPass( RenderDevice const & device
		, EnvironmentMap & reflectionMap
		, SceneNodeSPtr node
		, SceneNode const & objectNode
		, CubeMapFace face )
		: OwnedBy< EnvironmentMap >{ reflectionMap }
		, m_device{ device }
		, m_node{ node }
		, m_face{ face }
		, m_camera{ doCreateCamera( *node ) }
		, m_culler{ std::make_unique< FrustumCuller >( *m_camera->getScene(), *m_camera ) }
		, m_matrixUbo{ device }
		, m_opaquePass{ std::make_shared< ForwardRenderTechniquePass >( device
			, cuT( "EnvironmentMap" )
			, m_node->getName() + cuT( " Opaque" )
			, SceneRenderPassDesc{ m_matrixUbo, *m_culler, &objectNode }
			, RenderTechniquePassDesc{ true, SsaoConfig{} } ) }
		, m_transparentPass{ std::make_shared< ForwardRenderTechniquePass >( device
			, cuT( "EnvironmentMap" )
			, m_node->getName() + cuT( " Transparent" )
			, SceneRenderPassDesc{ m_matrixUbo, *m_culler, true, &objectNode }
			, RenderTechniquePassDesc{ true, SsaoConfig{} } ) }
		, m_mtxView{ m_camera->getView() }
		, m_modelUbo{ m_device.uboPools->getBuffer< ModelUboConfiguration >( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT ) }
		, m_hdrConfigUbo{ m_device }
	{
		log::trace << "Created EnvironmentMapPass" << objectNode.getName() << std::endl;
	}

	EnvironmentMapPass::~EnvironmentMapPass()
	{
		m_hdrConfigUbo.cleanup();
		m_device.uboPools->putBuffer( m_modelUbo );
		m_transparentPass.reset();
		m_opaquePass.reset();
		m_matrixUbo.cleanup();
	}

	bool EnvironmentMapPass::initialise( Size const & size
		, uint32_t face
		, ashes::RenderPass const & renderPass
		, SceneBackground const & background
		, ashes::DescriptorSetPool const & uboPool
		, ashes::DescriptorSetPool const & texPool
		, RenderPassTimer & timer )
	{
		float const aspect = float( size.getWidth() ) / size.getHeight();
		float const nearZ = 0.1f;
		float const farZ = 1000.0f;
		m_camera->getViewport().setPerspective( 90.0_degrees
			, aspect
			, nearZ
			, farZ );
		m_camera->resize( size );

		auto name = "EnvironmentMapPass" + m_node->getName() + castor::string::toString( face );
		static castor::Matrix4x4f const projection = convert( m_device->perspective( ( 45.0_degrees ).radians()
			, 1.0f
			, 0.0f, 2.0f ) );
		m_matrixUbo.cpuUpdate( m_mtxView, projection );
		auto const & environmentLayout = getOwner()->getTexture().getTexture();
		m_envView = environmentLayout->getLayerCubeFaceMipView( 0u, CubeMapFace( face ), 0u ).getTargetView();
		auto const & depthView = getOwner()->getDepthView();

		// Initialise opaque pass.
		m_opaquePass->initialiseRenderPass( m_envView
			, getOwner()->getDepthView()
			, size
			, true );
		m_opaquePass->initialise( size, timer, uint32_t( m_face ) * 3u + 0u, nullptr );

		// Create custom background pass.
		ashes::ImageViewCRefArray attaches;
		attaches.emplace_back( depthView );
		attaches.emplace_back( m_envView );
		m_frameBuffer = renderPass.createFrameBuffer( name
			, VkExtent2D{ size[0], size[1] }
			, std::move( attaches ) );
		m_backgroundCommands = m_device.graphicsCommandPool->createCommandBuffer( name
			, VK_COMMAND_BUFFER_LEVEL_SECONDARY );
		auto & commandBuffer = *m_backgroundCommands;
		m_renderPass = &renderPass;
		m_backgroundUboDescriptorSet = uboPool.createDescriptorSet( name + "Ubo", 0u );
		m_backgroundTexDescriptorSet = texPool.createDescriptorSet( name + "Tex", 0u );
		background.initialiseDescriptorSets( m_matrixUbo
			, m_modelUbo
			, m_hdrConfigUbo
			, *m_backgroundUboDescriptorSet
			, *m_backgroundTexDescriptorSet );
		m_backgroundUboDescriptorSet->update();
		m_backgroundTexDescriptorSet->update();
		background.prepareFrame( commandBuffer
			, size
			, renderPass
			, *m_backgroundUboDescriptorSet
			, *m_backgroundTexDescriptorSet );

		// Initialise transparent pass.
		m_transparentPass->initialiseRenderPass( m_envView
			, getOwner()->getDepthView()
			, size
			, false );
		m_transparentPass->initialise( size, timer, uint32_t( m_face ) * 3u + 2u, nullptr );

		m_commandBuffer = m_device.graphicsCommandPool->createCommandBuffer( name );
		m_finished = m_device->createSemaphore( name );
		return true;
	}

	void EnvironmentMapPass::cleanup()
	{
		m_finished.reset();
		m_commandBuffer.reset();
		m_backgroundCommands.reset();
		m_backgroundUboDescriptorSet.reset();
		m_backgroundTexDescriptorSet.reset();
		m_frameBuffer.reset();
	}

	void EnvironmentMapPass::update( CpuUpdater & updater )
	{
		auto & node = *updater.node;
		auto position = node.getDerivedPosition();
		m_camera->getParent()->setPosition( position );
		m_camera->getParent()->update();
		m_camera->update();
		castor::matrix::setTranslate( m_mtxModel, position );
		castor::matrix::scale( m_mtxModel, castor::Point3f{ 1, -1, 1 } );
		m_culler->compute();
		static_cast< RenderTechniquePass & >( *m_opaquePass ).update( updater );
		static_cast< RenderTechniquePass & >( *m_transparentPass ).update( updater );
		m_matrixUbo.cpuUpdate( m_camera->getView()
			, m_camera->getProjection() );
		auto & configuration = m_modelUbo.getData();
		configuration.prvModel = configuration.curModel;
		configuration.curModel = m_mtxModel;
		m_hdrConfigUbo.cpuUpdate( m_camera->getHdrConfig() );
	}

	void EnvironmentMapPass::update( GpuUpdater & updater )
	{
		RenderInfo info;
		m_opaquePass->update( updater );
		m_transparentPass->update( updater );
	}

	ashes::Semaphore const & EnvironmentMapPass::render( ashes::Semaphore const & toWait )
	{
		ashes::Semaphore const * result = &toWait;
		auto timer = &getOwner()->getTimer();

		m_commandBuffer->begin();
		m_commandBuffer->beginDebugBlock(
			{
				"EnvironmentMapPass render",
				makeFloatArray( getOwner()->getEngine()->getNextRainbowColour() ),
			} );
		timer->beginPass( *m_commandBuffer, uint32_t( m_face ) );
		timer->notifyPassRender( uint32_t( m_face ) );
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
		timer->endPass( *m_commandBuffer, uint32_t( m_face ) );
		m_commandBuffer->endDebugBlock();
		m_commandBuffer->end();

		m_device.graphicsQueue->submit( *m_commandBuffer
			, toWait
			, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
			, *m_finished
			, nullptr );
		result = m_finished.get();

		return *result;
	}
}
