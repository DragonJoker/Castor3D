#include "EnvironmentMap.hpp"

#include "Engine.hpp"

#include "EnvironmentMap/EnvironmentMapPass.hpp"
#include "Render/RenderPipeline.hpp"
#include "Scene/BillboardList.hpp"
#include "Scene/SceneNode.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <Image/Texture.hpp>
#include <Image/TextureView.hpp>
#include <RenderPass/RenderPass.hpp>
#include <RenderPass/FrameBuffer.hpp>
#include <RenderPass/FrameBufferAttachment.hpp>
#include <RenderPass/RenderPassCreateInfo.hpp>
#include <RenderPass/SubpassDependency.hpp>
#include <RenderPass/SubpassDescription.hpp>

#include <GlslSource.hpp>

using namespace castor;

namespace castor3d
{
	namespace
	{
		static Size const MapSize{ 1024, 1024 };

		TextureUnit doInitialisePoint( Engine & engine
			, Size const & size
			, MaterialType type )
		{
			String const name = cuT( "EnvironmentMap" );

			SamplerSPtr sampler;

			if ( engine.getSamplerCache().has( name ) )
			{
				sampler = engine.getSamplerCache().find( name );
			}
			else
			{
				sampler = engine.getSamplerCache().add( name );
				sampler->setMinFilter( renderer::Filter::eLinear );
				sampler->setMagFilter( renderer::Filter::eLinear );

				if ( type == MaterialType::ePbrMetallicRoughness
					|| type == MaterialType::ePbrSpecularGlossiness )
				{
					sampler->setMipFilter( renderer::MipmapMode::eLinear );
				}

				sampler->setWrapS( renderer::WrapMode::eClampToEdge );
				sampler->setWrapT( renderer::WrapMode::eClampToEdge );
				sampler->setWrapR( renderer::WrapMode::eClampToEdge );
			}

			renderer::ImageCreateInfo colour{};
			colour.flags = 0u;
			colour.arrayLayers = 1u;
			colour.extent.width = size[0];
			colour.extent.height = size[1];
			colour.extent.depth = 1u;
			colour.format = renderer::Format::eR32G32B32A32_SFLOAT;
			colour.imageType = renderer::TextureType::e2D;
			colour.initialLayout = renderer::ImageLayout::eUndefined;
			colour.mipLevels = 1u;
			colour.samples = renderer::SampleCountFlag::e1;
			colour.sharingMode = renderer::SharingMode::eExclusive;
			colour.tiling = renderer::ImageTiling::eOptimal;
			colour.usage = renderer::ImageUsageFlag::eColourAttachment | renderer::ImageUsageFlag::eSampled;
			auto texture = std::make_shared< TextureLayout >( *engine.getRenderSystem()
				, colour
				, renderer::MemoryPropertyFlag::eDeviceLocal );
			TextureUnit unit{ engine };
			unit.setTexture( texture );
			unit.setSampler( sampler );

			for ( auto & image : *texture )
			{
				image->initialiseSource();
			}

			return unit;
		}

		EnvironmentMap::EnvironmentMapPasses doCreatePasses( EnvironmentMap & map
			, SceneNode & node )
		{
			static Point3r const position;
			std::array< SceneNodeSPtr, size_t( CubeMapFace::eCount ) > nodes
			{
				std::make_shared< SceneNode >( cuT( "EnvironmentMap_PosX" ), *node.getScene() ),
				std::make_shared< SceneNode >( cuT( "EnvironmentMap_NegX" ), *node.getScene() ),
				std::make_shared< SceneNode >( cuT( "EnvironmentMap_PosY" ), *node.getScene() ),
				std::make_shared< SceneNode >( cuT( "EnvironmentMap_NegY" ), *node.getScene() ),
				std::make_shared< SceneNode >( cuT( "EnvironmentMap_PosZ" ), *node.getScene() ),
				std::make_shared< SceneNode >( cuT( "EnvironmentMap_NegZ" ), *node.getScene() ),
			};
			std::array< Quaternion, size_t( CubeMapFace::eCount ) > orients
			{
				Quaternion::fromMatrix( matrix::lookAt( position, Point3r{ -1, +0, +0 }, Point3r{ +0, -1, +0 } ) ),// Positive X
				Quaternion::fromMatrix( matrix::lookAt( position, Point3r{ +1, +0, +0 }, Point3r{ +0, -1, +0 } ) ),// Negative X
				Quaternion::fromMatrix( matrix::lookAt( position, Point3r{ +0, -1, +0 }, Point3r{ +0, +0, +1 } ) ),// Positive Y
				Quaternion::fromMatrix( matrix::lookAt( position, Point3r{ +0, +1, +0 }, Point3r{ +0, +0, -1 } ) ),// Negative Y
				Quaternion::fromMatrix( matrix::lookAt( position, Point3r{ +0, +0, -1 }, Point3r{ +0, -1, +0 } ) ),// Positive Z
				Quaternion::fromMatrix( matrix::lookAt( position, Point3r{ +0, +0, +1 }, Point3r{ +0, -1, +0 } ) ),// Negative Z
			};

			auto i = 0u;

			for ( auto & node : nodes )
			{
				node->setOrientation( orients[i] );
				++i;
			}

			return EnvironmentMap::EnvironmentMapPasses
			{
				{
					std::make_unique< EnvironmentMapPass >( map, nodes[0], node ),
					std::make_unique< EnvironmentMapPass >( map, nodes[1], node ),
					std::make_unique< EnvironmentMapPass >( map, nodes[2], node ),
					std::make_unique< EnvironmentMapPass >( map, nodes[3], node ),
					std::make_unique< EnvironmentMapPass >( map, nodes[4], node ),
					std::make_unique< EnvironmentMapPass >( map, nodes[5], node ),
				}
			};
		}
	}

	uint32_t EnvironmentMap::m_count = 0u;

	EnvironmentMap::EnvironmentMap( Engine & engine
		, SceneNode & node )
		: OwnedBy< Engine >{ engine }
		, m_environmentMap{ doInitialisePoint( engine, MapSize, node.getScene()->getMaterialsType() ) }
		, m_node{ node }
		, m_index{ ++m_count }
		, m_passes( doCreatePasses( *this, node ) )
	{
	}

	EnvironmentMap::~EnvironmentMap()
	{
	}

	bool EnvironmentMap::initialise()
	{
		auto & device = *getEngine()->getRenderSystem()->getCurrentDevice();
		renderer::ImageCreateInfo depthStencil{};
		depthStencil.flags = 0u;
		depthStencil.arrayLayers = 1u;
		depthStencil.extent.width = MapSize[0];
		depthStencil.extent.height = MapSize[1];
		depthStencil.extent.depth = 1u;
		depthStencil.format = renderer::Format::eD32_SFLOAT;
		depthStencil.imageType = renderer::TextureType::e2D;
		depthStencil.initialLayout = renderer::ImageLayout::eUndefined;
		depthStencil.mipLevels = 1u;
		depthStencil.samples = renderer::SampleCountFlag::e1;
		depthStencil.sharingMode = renderer::SharingMode::eExclusive;
		depthStencil.tiling = renderer::ImageTiling::eOptimal;
		depthStencil.usage = renderer::ImageUsageFlag::eDepthStencilAttachment;
		m_depthBuffer = device.createTexture( depthStencil, renderer::MemoryPropertyFlag::eDeviceLocal );
		m_depthBufferView = m_depthBuffer->createView( renderer::TextureViewType::e2D
			, m_depthBuffer->getFormat() );

		renderer::RenderPassCreateInfo createInfo{};
		createInfo.flags = 0u;

		createInfo.attachments.resize( 2u );
		createInfo.attachments[0].index = 0u;
		createInfo.attachments[0].format = m_environmentMap.getTexture()->getPixelFormat();
		createInfo.attachments[0].samples = renderer::SampleCountFlag::e1;
		createInfo.attachments[0].loadOp = renderer::AttachmentLoadOp::eClear;
		createInfo.attachments[0].storeOp = renderer::AttachmentStoreOp::eStore;
		createInfo.attachments[0].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
		createInfo.attachments[0].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
		createInfo.attachments[0].initialLayout = renderer::ImageLayout::eUndefined;
		createInfo.attachments[0].finalLayout = renderer::ImageLayout::eShaderReadOnlyOptimal;

		createInfo.attachments[1].index = 1u;
		createInfo.attachments[1].format = m_depthBuffer->getFormat();
		createInfo.attachments[1].samples = renderer::SampleCountFlag::e1;
		createInfo.attachments[1].loadOp = renderer::AttachmentLoadOp::eClear;
		createInfo.attachments[1].storeOp = renderer::AttachmentStoreOp::eStore;
		createInfo.attachments[1].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
		createInfo.attachments[1].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
		createInfo.attachments[1].initialLayout = renderer::ImageLayout::eUndefined;
		createInfo.attachments[1].finalLayout = renderer::ImageLayout::eDepthStencilAttachmentOptimal;

		renderer::AttachmentReference colourReference;
		colourReference.attachment = 0u;
		colourReference.layout = renderer::ImageLayout::eColourAttachmentOptimal;

		renderer::AttachmentReference depthStencilReference;
		depthStencilReference.attachment = 1u;
		depthStencilReference.layout = renderer::ImageLayout::eDepthStencilAttachmentOptimal;

		createInfo.subpasses.resize( 1u );
		createInfo.subpasses[0].flags = 0u;
		createInfo.subpasses[0].colorAttachments = { colourReference };
		createInfo.subpasses[0].depthStencilAttachment = depthStencilReference;

		createInfo.dependencies.resize( 2u );
		createInfo.dependencies[0].srcSubpass = renderer::ExternalSubpass;
		createInfo.dependencies[0].dstSubpass = 0u;
		createInfo.dependencies[0].srcStageMask = renderer::PipelineStageFlag::eBottomOfPipe;
		createInfo.dependencies[0].dstStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		createInfo.dependencies[0].srcAccessMask = renderer::AccessFlag::eMemoryRead;
		createInfo.dependencies[0].dstAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
		createInfo.dependencies[0].dependencyFlags = renderer::DependencyFlag::eByRegion;

		createInfo.dependencies[1].srcSubpass = 0u;
		createInfo.dependencies[1].dstSubpass = renderer::ExternalSubpass;
		createInfo.dependencies[1].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		createInfo.dependencies[1].dstStageMask = renderer::PipelineStageFlag::eBottomOfPipe;
		createInfo.dependencies[1].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
		createInfo.dependencies[1].dstAccessMask = renderer::AccessFlag::eMemoryRead;
		createInfo.dependencies[1].dependencyFlags = renderer::DependencyFlag::eByRegion;

		auto & environment = m_environmentMap.getTexture()->getTexture();
		renderer::ImageViewCreateInfo view;
		view.components.r = renderer::ComponentSwizzle::eIdentity;
		view.components.g = renderer::ComponentSwizzle::eIdentity;
		view.components.b = renderer::ComponentSwizzle::eIdentity;
		view.components.a = renderer::ComponentSwizzle::eIdentity;
		view.format = environment.getFormat();
		view.viewType = renderer::TextureViewType::e2D;
		view.subresourceRange.aspectMask = renderer::getAspectMask( environment.getFormat() );
		view.subresourceRange.baseMipLevel = 0u;
		view.subresourceRange.levelCount = 1u;
		view.subresourceRange.layerCount = 1u;
		uint32_t face = 0u;

		for ( auto & frameBuffer : m_frameBuffers )
		{
			view.subresourceRange.baseArrayLayer = face;
			frameBuffer.view = environment.createView( view );
			frameBuffer.renderPass = device.createRenderPass( createInfo );
			renderer::FrameBufferAttachmentArray attaches;
			attaches.emplace_back( *( frameBuffer.renderPass->getAttachments().begin() + 0u ), *m_depthBufferView );
			attaches.emplace_back( *( frameBuffer.renderPass->getAttachments().begin() + 1u ), *frameBuffer.view );
			frameBuffer.frameBuffer = frameBuffer.renderPass->createFrameBuffer( renderer::Extent2D{ MapSize[0], MapSize[1] }
				, std::move( attaches ) );
			frameBuffer.backgroundCommands = device.getGraphicsCommandPool().createCommandBuffer( false );
			m_node.getScene()->getBackground().prepareFrame( *frameBuffer.backgroundCommands
				, *frameBuffer.renderPass );
			++face;
		}

		m_commandBuffer = device.getGraphicsCommandPool().createCommandBuffer();

		for ( auto & pass : m_passes )
		{
			pass->initialise( MapSize );
		}
		
		static float constexpr component = std::numeric_limits< float >::max();
		static renderer::ClearColorValue const white{ component, component, component, component };
		face = 0u;

		if ( m_commandBuffer->begin( renderer::CommandBufferUsageFlag::eSimultaneousUse ) )
		{
			for ( auto & frameBuffer : m_frameBuffers )
			{
				m_commandBuffer->beginRenderPass( *frameBuffer.renderPass
					, *frameBuffer.frameBuffer
					, { white, white }
					, renderer::SubpassContents::eSecondaryCommandBuffers );
				m_commandBuffer->executeCommands( {
					m_passes[face]->getOpaqueCommandBuffer(),
					*frameBuffer.backgroundCommands,
					m_passes[face]->getTransparentCommandBuffer(),
				} );
				m_commandBuffer->endRenderPass();
				++face;
			}

			m_commandBuffer->end();
		}
		return true;
	}

	void EnvironmentMap::cleanup()
	{
		for ( auto & frameBuffer : m_frameBuffers )
		{
			frameBuffer.frameBuffer.reset();
			frameBuffer.renderPass.reset();
			frameBuffer.view.reset();
		}

		for ( auto & pass : m_passes )
		{
			pass->cleanup();
		}

		m_depthBuffer.reset();
		m_environmentMap.cleanup();
	}
	
	void EnvironmentMap::update( RenderQueueArray & queues )
	{
		for ( auto & pass : m_passes )
		{
			pass->update( m_node, queues );
		}
	}

	void EnvironmentMap::render( renderer::Semaphore const & toWait )
	{
		m_render++;

		if ( m_render == 5u )
		{
			auto & device = *getEngine()->getRenderSystem()->getCurrentDevice();
			device.getGraphicsQueue().submit( *m_commandBuffer
				, toWait
				, renderer::PipelineStageFlag::eBottomOfPipe
				, *m_finished
				, nullptr );
			auto & scene = *m_node.getScene();

			if ( scene.getMaterialsType() == MaterialType::ePbrMetallicRoughness
				|| scene.getMaterialsType() == MaterialType::ePbrSpecularGlossiness )
			{
				m_environmentMap.getTexture()->generateMipmaps();
			}

			m_render = 0u;
		}
	}

	void EnvironmentMap::debugDisplay( castor::Size const & size, uint32_t index )
	{
		//Size displaySize{ 128u, 128u };
		//Position position{ int32_t( displaySize.getWidth() * 4 * index ), int32_t( displaySize.getHeight() * 4 ) };
		//getEngine()->getRenderSystem()->getCurrentContext()->renderTextureCube( position
		//	, displaySize
		//	, *m_environmentMap.getTexture() );
	}
}
