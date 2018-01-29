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
#include <RenderPass/RenderPassState.hpp>
#include <RenderPass/RenderSubpass.hpp>
#include <RenderPass/RenderSubpassState.hpp>
#include <RenderPass/TextureAttachment.hpp>

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

			auto texture = std::make_shared< TextureLayout >( *engine.getRenderSystem()
				, renderer::TextureType::e2D
				, renderer::ImageUsageFlag::eColourAttachment | renderer::ImageUsageFlag::eSampled
				, renderer::MemoryPropertyFlag::eDeviceLocal
				, PixelFormat::eRGBA32F
				, size );
			TextureUnit unit{ engine };
			unit.setTexture( texture );
			unit.setSampler( sampler );

			for ( auto & image : *texture )
			{
				image->initialiseSource();
			}

			return unit;
		}

		EnvironmentMap::EnvironmentMapPasses doCreatePasses( EnvironmentMap & p_map
			, SceneNode & p_node )
		{
			static Point3r const position;
			std::array< SceneNodeSPtr, size_t( CubeMapFace::eCount ) > nodes
			{
				std::make_shared< SceneNode >( cuT( "EnvironmentMap_PosX" ), *p_node.getScene() ),
				std::make_shared< SceneNode >( cuT( "EnvironmentMap_NegX" ), *p_node.getScene() ),
				std::make_shared< SceneNode >( cuT( "EnvironmentMap_PosY" ), *p_node.getScene() ),
				std::make_shared< SceneNode >( cuT( "EnvironmentMap_NegY" ), *p_node.getScene() ),
				std::make_shared< SceneNode >( cuT( "EnvironmentMap_PosZ" ), *p_node.getScene() ),
				std::make_shared< SceneNode >( cuT( "EnvironmentMap_NegZ" ), *p_node.getScene() ),
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
					std::make_unique< EnvironmentMapPass >( p_map, nodes[0], p_node ),
					std::make_unique< EnvironmentMapPass >( p_map, nodes[1], p_node ),
					std::make_unique< EnvironmentMapPass >( p_map, nodes[2], p_node ),
					std::make_unique< EnvironmentMapPass >( p_map, nodes[3], p_node ),
					std::make_unique< EnvironmentMapPass >( p_map, nodes[4], p_node ),
					std::make_unique< EnvironmentMapPass >( p_map, nodes[5], p_node ),
				}
			};
		}
	}

	uint32_t EnvironmentMap::m_count = 0u;

	EnvironmentMap::EnvironmentMap( Engine & engine
		, SceneNode  & p_node )
		: OwnedBy< Engine >{ engine }
		, m_environmentMap{ doInitialisePoint( engine, MapSize, p_node.getScene()->getMaterialsType() ) }
		, m_node{ p_node }
		, m_index{ ++m_count }
		, m_passes( doCreatePasses( *this, p_node ) )
	{
	}

	EnvironmentMap::~EnvironmentMap()
	{
	}

	bool EnvironmentMap::initialise()
	{
		auto & device = *getEngine()->getRenderSystem()->getCurrentDevice();
		m_depthBuffer = device.createTexture();
		m_depthBuffer->setImage( PixelFormat::eD32F
			, renderer::UIVec2{ MapSize }
			, renderer::ImageUsageFlag::eDepthStencilAttachment );
		m_depthBufferView = m_depthBuffer->createView( renderer::TextureType::e2D
			, m_depthBuffer->getFormat()
			, 0u
			, 1u
			, 0u
			, 1u );
		std::vector< renderer::PixelFormat > formats{ { m_depthBuffer->getFormat(), m_environmentMap.getTexture()->getPixelFormat() } };
		renderer::RenderSubpassPtrArray subpasses;
		subpasses.emplace_back( device.createRenderSubpass( formats
			, renderer::RenderSubpassState{ renderer::PipelineStageFlag::eColourAttachmentOutput
			, renderer::AccessFlag::eColourAttachmentWrite } ) );
		auto & environment = m_environmentMap.getTexture()->getTexture();
		uint32_t face = 0u;

		for ( auto & frameBuffer : m_frameBuffers )
		{
			frameBuffer.view = environment.createView( renderer::TextureType::e2D
				, environment.getFormat()
				, 0u
				, 1u
				, face
				, 1u );
			frameBuffer.renderPass = device.createRenderPass( formats
				, subpasses
				, renderer::RenderPassState{ renderer::PipelineStageFlag::eColourAttachmentOutput
					, renderer::AccessFlag::eColourAttachmentWrite
					, { renderer::ImageLayout::eDepthStencilAttachmentOptimal, renderer::ImageLayout::eColourAttachmentOptimal } }
				, renderer::RenderPassState{ renderer::PipelineStageFlag::eColourAttachmentOutput
					, renderer::AccessFlag::eColourAttachmentWrite
					, { renderer::ImageLayout::eDepthStencilAttachmentOptimal, renderer::ImageLayout::eColourAttachmentOptimal } } );
			renderer::TextureAttachmentPtrArray attaches;
			attaches.emplace_back( std::make_unique< renderer::TextureAttachment >( *m_depthBufferView ) );
			attaches.emplace_back( std::make_unique< renderer::TextureAttachment >( *frameBuffer.view ) );
			frameBuffer.frameBuffer = frameBuffer.renderPass->createFrameBuffer( renderer::UIVec2{ MapSize }
				, std::move( attaches ) );
			++face;
		}

		m_commandBuffer = device.getGraphicsCommandPool().createCommandBuffer();

		for ( auto & pass : m_passes )
		{
			pass->initialise( MapSize );
		}
		
		static float constexpr component = std::numeric_limits< float >::max();
		static renderer::RgbaColour const white{ component, component, component, component };
		face = 0u;

		if ( m_commandBuffer->begin( renderer::CommandBufferUsageFlag::eSimultaneousUse ) )
		{
			for ( auto & frameBuffer : m_frameBuffers )
			{
				m_commandBuffer->beginRenderPass( *frameBuffer.renderPass
					, *frameBuffer.frameBuffer
					, { white, white }
					, renderer::SubpassContents::eSecondaryCommandBuffers );
				m_commandBuffer->executeCommands( m_passes[face]->getCommandBuffers() );
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
	
	void EnvironmentMap::update( RenderQueueArray & p_queues )
	{
		for ( auto & pass : m_passes )
		{
			pass->update( m_node, p_queues );
		}
	}

	void EnvironmentMap::render()
	{
		m_render++;

		if ( m_render == 5u )
		{
			auto & device = *getEngine()->getRenderSystem()->getCurrentDevice();
			device.getGraphicsQueue().submit( *m_commandBuffer, nullptr );
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
