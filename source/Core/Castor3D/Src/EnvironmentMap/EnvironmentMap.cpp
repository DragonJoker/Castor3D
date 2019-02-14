#include "EnvironmentMap.hpp"

#include "Engine.hpp"

#include "EnvironmentMap/EnvironmentMapPass.hpp"
#include "Render/RenderPipeline.hpp"
#include "Scene/BillboardList.hpp"
#include "Scene/SceneNode.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <Miscellaneous/BitSize.hpp>

#include <Ashes/Image/Texture.hpp>
#include <Ashes/Image/TextureView.hpp>
#include <Ashes/RenderPass/RenderPass.hpp>
#include <Ashes/RenderPass/FrameBuffer.hpp>
#include <Ashes/RenderPass/FrameBufferAttachment.hpp>
#include <Ashes/RenderPass/RenderPassCreateInfo.hpp>
#include <Ashes/RenderPass/SubpassDependency.hpp>
#include <Ashes/RenderPass/SubpassDescription.hpp>

#include <ShaderWriter/Source.hpp>

using namespace castor;

namespace castor3d
{
	namespace
	{
		static Size const MapSize{ 128, 128 };

		SamplerSPtr doCreateSampler( Engine & engine
			, uint32_t levelCount )
		{
			static uint32_t id = 0u;
			String const name = cuT( "EnvironmentMap_" ) + string::toString( id++, 10, std::locale{ "C" } );
			SamplerSPtr sampler;

			if ( engine.getSamplerCache().has( name ) )
			{
				sampler = engine.getSamplerCache().find( name );
			}
			else
			{
				sampler = engine.getSamplerCache().add( name );
				sampler->setMinFilter( ashes::Filter::eLinear );
				sampler->setMagFilter( ashes::Filter::eLinear );
				sampler->setWrapS( ashes::WrapMode::eClampToEdge );
				sampler->setWrapT( ashes::WrapMode::eClampToEdge );
				sampler->setWrapR( ashes::WrapMode::eClampToEdge );

				if ( levelCount > 1u )
				{
					sampler->setMipFilter( ashes::MipmapMode::eLinear );
					sampler->setMinLod( 0.0f );
					sampler->setMaxLod( float( levelCount - 1u ) );
					sampler->enableAnisotropicFiltering( true );
					sampler->setMaxAnisotropy( sampler->getMaxLod() );
				}
			}

			return sampler;
		}

		TextureUnit doInitialisePoint( Engine & engine
			, Size const & size
			, MaterialType type )
		{
			ashes::ImageCreateInfo colour{};
			colour.flags = ashes::ImageCreateFlag::eCubeCompatible;
			colour.arrayLayers = 6u;
			colour.extent.width = size[0];
			colour.extent.height = size[1];
			colour.extent.depth = 1u;
			colour.format = ashes::Format::eR16G16B16A16_SFLOAT;
			colour.imageType = ashes::TextureType::e2D;
			colour.initialLayout = ashes::ImageLayout::eUndefined;
			colour.mipLevels = castor::getBitSize( std::min( size[0], size[1] ) );
			colour.samples = ashes::SampleCountFlag::e1;
			colour.sharingMode = ashes::SharingMode::eExclusive;
			colour.tiling = ashes::ImageTiling::eOptimal;
			colour.usage = ashes::ImageUsageFlag::eColourAttachment
				| ashes::ImageUsageFlag::eSampled
				| ashes::ImageUsageFlag::eTransferDst
				| ashes::ImageUsageFlag::eTransferSrc;
			auto texture = std::make_shared< TextureLayout >( *engine.getRenderSystem()
				, colour
				, ashes::MemoryPropertyFlag::eDeviceLocal );
			TextureUnit unit{ engine };
			unit.setTexture( texture );
			unit.setSampler( doCreateSampler( engine, texture->getMipmapCount() ) );

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
		auto & device = getCurrentDevice( *this );
		m_environmentMap.initialise();
		ashes::ImageCreateInfo depthStencil{};
		depthStencil.flags = 0u;
		depthStencil.arrayLayers = 1u;
		depthStencil.extent.width = MapSize[0];
		depthStencil.extent.height = MapSize[1];
		depthStencil.extent.depth = 1u;
		depthStencil.format = ashes::Format::eD24_UNORM_S8_UINT;
		depthStencil.imageType = ashes::TextureType::e2D;
		depthStencil.initialLayout = ashes::ImageLayout::eUndefined;
		depthStencil.mipLevels = 1u;
		depthStencil.samples = ashes::SampleCountFlag::e1;
		depthStencil.sharingMode = ashes::SharingMode::eExclusive;
		depthStencil.tiling = ashes::ImageTiling::eOptimal;
		depthStencil.usage = ashes::ImageUsageFlag::eDepthStencilAttachment;
		m_depthBuffer = device.createTexture( depthStencil, ashes::MemoryPropertyFlag::eDeviceLocal );
		m_depthBufferView = m_depthBuffer->createView( ashes::TextureViewType::e2D
			, m_depthBuffer->getFormat() );

		ashes::RenderPassCreateInfo createInfo{};
		createInfo.flags = 0u;

		createInfo.attachments.resize( 2u );
		createInfo.attachments[0].format = m_depthBuffer->getFormat();
		createInfo.attachments[0].samples = ashes::SampleCountFlag::e1;
		createInfo.attachments[0].loadOp = ashes::AttachmentLoadOp::eClear;
		createInfo.attachments[0].storeOp = ashes::AttachmentStoreOp::eStore;
		createInfo.attachments[0].stencilLoadOp = ashes::AttachmentLoadOp::eDontCare;
		createInfo.attachments[0].stencilStoreOp = ashes::AttachmentStoreOp::eDontCare;
		createInfo.attachments[0].initialLayout = ashes::ImageLayout::eUndefined;
		createInfo.attachments[0].finalLayout = ashes::ImageLayout::eDepthStencilAttachmentOptimal;

		createInfo.attachments[1].format = m_environmentMap.getTexture()->getPixelFormat();
		createInfo.attachments[1].samples = ashes::SampleCountFlag::e1;
		createInfo.attachments[1].loadOp = ashes::AttachmentLoadOp::eClear;
		createInfo.attachments[1].storeOp = ashes::AttachmentStoreOp::eStore;
		createInfo.attachments[1].stencilLoadOp = ashes::AttachmentLoadOp::eDontCare;
		createInfo.attachments[1].stencilStoreOp = ashes::AttachmentStoreOp::eDontCare;
		createInfo.attachments[1].initialLayout = ashes::ImageLayout::eUndefined;
		createInfo.attachments[1].finalLayout = ashes::ImageLayout::eShaderReadOnlyOptimal;

		createInfo.subpasses.resize( 1u );
		createInfo.subpasses[0].flags = 0u;
		createInfo.subpasses[0].colorAttachments.push_back( { 1u, ashes::ImageLayout::eColourAttachmentOptimal } );
		createInfo.subpasses[0].depthStencilAttachment = { 0u, ashes::ImageLayout::eDepthStencilAttachmentOptimal };

		createInfo.dependencies.resize( 2u );
		createInfo.dependencies[0].srcSubpass = ashes::ExternalSubpass;
		createInfo.dependencies[0].dstSubpass = 0u;
		createInfo.dependencies[0].srcStageMask = ashes::PipelineStageFlag::eColourAttachmentOutput;
		createInfo.dependencies[0].dstStageMask = ashes::PipelineStageFlag::eColourAttachmentOutput;
		createInfo.dependencies[0].srcAccessMask = ashes::AccessFlag::eColourAttachmentWrite;
		createInfo.dependencies[0].dstAccessMask = ashes::AccessFlag::eColourAttachmentWrite;
		createInfo.dependencies[0].dependencyFlags = ashes::DependencyFlag::eByRegion;

		createInfo.dependencies[1].srcSubpass = 0u;
		createInfo.dependencies[1].dstSubpass = ashes::ExternalSubpass;
		createInfo.dependencies[1].srcStageMask = ashes::PipelineStageFlag::eColourAttachmentOutput;
		createInfo.dependencies[1].dstStageMask = ashes::PipelineStageFlag::eFragmentShader;
		createInfo.dependencies[1].srcAccessMask = ashes::AccessFlag::eColourAttachmentWrite;
		createInfo.dependencies[1].dstAccessMask = ashes::AccessFlag::eShaderRead;
		createInfo.dependencies[1].dependencyFlags = ashes::DependencyFlag::eByRegion;

		m_renderPass = device.createRenderPass( createInfo );
		auto & background = *m_node.getScene()->getBackground();
		m_backgroundDescriptorPool = background.getDescriptorLayout().createPool( 6u );
		uint32_t face = 0u;

		for ( auto & pass : m_passes )
		{
			pass->initialise( MapSize
				, face
				, *m_renderPass
				, background
				, *m_backgroundDescriptorPool );
			++face;
		}
		
		return true;
	}

	void EnvironmentMap::cleanup()
	{
		for ( auto & pass : m_passes )
		{
			pass->cleanup();
		}

		m_backgroundDescriptorPool.reset();
		m_renderPass.reset();
		m_depthBufferView.reset();
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

	ashes::Semaphore const & EnvironmentMap::render( ashes::Semaphore const & toWait )
	{
		ashes::Semaphore const * result = &toWait;
		m_render++;

		if ( m_render == 5u )
		{
			for ( auto & pass : m_passes )
			{
				result = &pass->render( *result );
			}

			auto & scene = *m_node.getScene();

			if ( m_environmentMap.getTexture()->getMipmapCount() > 1u
				|| scene.getMaterialsType() == MaterialType::ePbrMetallicRoughness
				|| scene.getMaterialsType() == MaterialType::ePbrSpecularGlossiness )
			{
				m_environmentMap.getTexture()->generateMipmaps();
			}

			m_render = 0u;
		}

		return *result;
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
