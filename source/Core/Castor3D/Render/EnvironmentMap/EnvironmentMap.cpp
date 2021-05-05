#include "Castor3D/Render/EnvironmentMap/EnvironmentMap.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/SamplerCache.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/EnvironmentMap/EnvironmentMapPass.hpp"
#include "Castor3D/Render/PBR/IblTextures.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Background/Background.hpp"

#include <CastorUtils/Miscellaneous/BitSize.hpp>

#include <ashespp/Image/Image.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

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
			String const name = cuT( "EnvironmentMap_" ) + string::toString( id++ );
			SamplerSPtr sampler;

			if ( engine.getSamplerCache().has( name ) )
			{
				sampler = engine.getSamplerCache().find( name );
			}
			else
			{
				sampler = engine.getSamplerCache().add( name );
				sampler->setMinFilter( VK_FILTER_LINEAR );
				sampler->setMagFilter( VK_FILTER_LINEAR );
				sampler->setWrapS( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
				sampler->setWrapT( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
				sampler->setWrapR( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );

				if ( levelCount > 1u )
				{
					sampler->setMipFilter( VK_SAMPLER_MIPMAP_MODE_LINEAR );
					sampler->setMinLod( 0.0f );
					sampler->setMaxLod( float( levelCount - 1u ) );
					sampler->enableAnisotropicFiltering( true );
					sampler->setMaxAnisotropy( sampler->getMaxLod() );
				}
			}

			return sampler;
		}

		TextureUnitSPtr doCreateTexture( RenderDevice const & device
			, Size const & size )
		{
			ashes::ImageCreateInfo createInfo
			{
				VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT,
				VK_IMAGE_TYPE_2D,
				VK_FORMAT_R16G16B16A16_SFLOAT,
				{ size[0], size[1], 1u },
				uint32_t( castor::getBitSize( std::min( size[0], size[1] ) ) ),
				6u,
				VK_SAMPLE_COUNT_1_BIT,
				VK_IMAGE_TILING_OPTIMAL,
				( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
					| VK_IMAGE_USAGE_SAMPLED_BIT
					| VK_IMAGE_USAGE_TRANSFER_DST_BIT
					| VK_IMAGE_USAGE_TRANSFER_SRC_BIT ),
			};
			auto texture = std::make_shared< TextureLayout >( device.renderSystem
				, createInfo
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, cuT( "EnvironmentMap" ) );
			auto result = std::make_shared< TextureUnit >( *device.renderSystem.getEngine() );
			result->setTexture( texture );
			result->setSampler( doCreateSampler( *device.renderSystem.getEngine(), texture->getMipmapCount() ) );
			result->initialise( device );
			return result;
		}

		ashes::ImagePtr doCreateDepthBuffer( RenderDevice const & device
			, Size const & size )
		{
			ashes::ImageCreateInfo depthStencil
			{
				0u,
				VK_IMAGE_TYPE_2D,
				device.selectSuitableDepthStencilFormat( VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT ),
				{ size[0], size[1], 1u },
				1u,
				1u,
				VK_SAMPLE_COUNT_1_BIT,
				VK_IMAGE_TILING_OPTIMAL,
				VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			};
			return makeImage( device
				, std::move( depthStencil )
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, "EnvironmentMapDepth" );
		}

		ashes::RenderPassPtr doCreateRenderPass( RenderDevice const & device
			, VkFormat depthFormat
			, VkFormat colourFormat )
		{
			ashes::VkAttachmentDescriptionArray attaches
			{
				{ 0u
					, depthFormat
					, VK_SAMPLE_COUNT_1_BIT
					, VK_ATTACHMENT_LOAD_OP_CLEAR
					, VK_ATTACHMENT_STORE_OP_STORE
					, VK_ATTACHMENT_LOAD_OP_DONT_CARE
					, VK_ATTACHMENT_STORE_OP_DONT_CARE
					, VK_IMAGE_LAYOUT_UNDEFINED
					, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL }
				, { 0u
					, colourFormat
					, VK_SAMPLE_COUNT_1_BIT
					, VK_ATTACHMENT_LOAD_OP_CLEAR
					, VK_ATTACHMENT_STORE_OP_STORE
					, VK_ATTACHMENT_LOAD_OP_DONT_CARE
					, VK_ATTACHMENT_STORE_OP_DONT_CARE
					, VK_IMAGE_LAYOUT_UNDEFINED
					, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL } };
			ashes::SubpassDescriptionArray subpasses;
			subpasses.push_back( { 0u
				, VK_PIPELINE_BIND_POINT_GRAPHICS
				, {}
				, { { 1u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } }
				, {}
				, VkAttachmentReference{ 0u, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL }
				, {} } );
			ashes::VkSubpassDependencyArray dependencies
			{
				{ VK_SUBPASS_EXTERNAL
					, 0u
					, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
					, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
					, VK_ACCESS_SHADER_READ_BIT
					, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
					, VK_DEPENDENCY_BY_REGION_BIT }
				, { 0u
					, VK_SUBPASS_EXTERNAL
					, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
					, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
					, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
					, VK_ACCESS_SHADER_READ_BIT
					, VK_DEPENDENCY_BY_REGION_BIT } };
			ashes::RenderPassCreateInfo createInfo{ 0u
				, std::move( attaches )
				, std::move( subpasses )
				, std::move( dependencies ) };
			return device->createRenderPass( "EnvironmentMap"
				, std::move( createInfo ) );
		}

		EnvironmentMap::EnvironmentMapPasses doCreatePasses( RenderDevice const & device
			, EnvironmentMap & map
			, SceneNode & node
			, RenderPassTimer & timer )
		{
			static castor::Point3f const position;
			std::array< SceneNodeSPtr, size_t( CubeMapFace::eCount ) > nodes
			{
				std::make_shared< SceneNode >( node.getName() + cuT( " PosX" ), *node.getScene() ),
				std::make_shared< SceneNode >( node.getName() + cuT( " NegX" ), *node.getScene() ),
				std::make_shared< SceneNode >( node.getName() + cuT( " PosY" ), *node.getScene() ),
				std::make_shared< SceneNode >( node.getName() + cuT( " NegY" ), *node.getScene() ),
				std::make_shared< SceneNode >( node.getName() + cuT( " PosZ" ), *node.getScene() ),
				std::make_shared< SceneNode >( node.getName() + cuT( " NegZ" ), *node.getScene() ),
			};
			std::array< Quaternion, size_t( CubeMapFace::eCount ) > orients
			{
				Quaternion::fromMatrix( matrix::lookAt( position, castor::Point3f{ -1.0f, +0.0f, +0.0f }, castor::Point3f{ +0.0f, -1.0f, +0.0f } ) ),// Positive X
				Quaternion::fromMatrix( matrix::lookAt( position, castor::Point3f{ +1.0f, +0.0f, +0.0f }, castor::Point3f{ +0.0f, -1.0f, +0.0f } ) ),// Negative X
				Quaternion::fromMatrix( matrix::lookAt( position, castor::Point3f{ +0.0f, -1.0f, +0.0f }, castor::Point3f{ +0.0f, +0.0f, +1.0f } ) ),// Positive Y
				Quaternion::fromMatrix( matrix::lookAt( position, castor::Point3f{ +0.0f, +1.0f, +0.0f }, castor::Point3f{ +0.0f, +0.0f, -1.0f } ) ),// Negative Y
				Quaternion::fromMatrix( matrix::lookAt( position, castor::Point3f{ +0.0f, +0.0f, -1.0f }, castor::Point3f{ +0.0f, -1.0f, +0.0f } ) ),// Positive Z
				Quaternion::fromMatrix( matrix::lookAt( position, castor::Point3f{ +0.0f, +0.0f, +1.0f }, castor::Point3f{ +0.0f, -1.0f, +0.0f } ) ),// Negative Z
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
					std::make_unique< EnvironmentMapPass >( device, map, nodes[0], node, CubeMapFace::ePositiveX, timer ),
					std::make_unique< EnvironmentMapPass >( device, map, nodes[1], node, CubeMapFace::eNegativeX, timer ),
					std::make_unique< EnvironmentMapPass >( device, map, nodes[2], node, CubeMapFace::ePositiveY, timer ),
					std::make_unique< EnvironmentMapPass >( device, map, nodes[3], node, CubeMapFace::eNegativeY, timer ),
					std::make_unique< EnvironmentMapPass >( device, map, nodes[4], node, CubeMapFace::ePositiveZ, timer ),
					std::make_unique< EnvironmentMapPass >( device, map, nodes[5], node, CubeMapFace::eNegativeZ, timer ),
				}
			};
		}
	}

	uint32_t EnvironmentMap::m_count = 0u;

	EnvironmentMap::EnvironmentMap( RenderDevice const & device
		, SceneNode & node )
		: OwnedBy< Engine >{ *device.renderSystem.getEngine() }
		, m_device{ device }
		, m_environmentMap{ doCreateTexture( device, MapSize ) }
		, m_depthBuffer{ doCreateDepthBuffer( device, MapSize ) }
		, m_depthBufferView{ m_depthBuffer->createView( "EnvironmentMapDepth"
			, VK_IMAGE_VIEW_TYPE_2D
			, m_depthBuffer->getFormat() ) }
		, m_renderPass{ doCreateRenderPass( device
			, m_depthBuffer->getFormat()
			, m_environmentMap->getTexture()->getPixelFormat() ) }
		, m_node{ node }
		, m_index{ ++m_count }
		, m_render{ ( m_count % 5u ) }
		, m_timer{ std::make_shared< RenderPassTimer >( m_device
			, cuT( "EnvironmentMap" )
			, m_node.getName()
			, uint32_t( 6u * 3u + 1u ) ) } // passes + mipmap generation
		, m_passes( doCreatePasses( m_device , *this, node, *m_timer ) )
		, m_generateMipmaps{ m_device, cuT( "EnvironmentMapMipmaps" ) }
	{
		log::trace << "Created EnvironmentMap" << node.getName() << std::endl;
	}

	EnvironmentMap::~EnvironmentMap()
	{
		m_generateMipmaps = {};
		m_ibl.reset();
		m_renderPass.reset();
		m_depthBuffer.reset();
		m_environmentMap->cleanup();
	}

	bool EnvironmentMap::initialise()
	{
		if ( !m_backgroundUboDescriptorPool )
		{
			auto & background = *m_node.getScene()->getBackground();
			m_backgroundUboDescriptorPool = background.getUboDescriptorLayout().createPool( "EnvironmentMapUbo", 6u );
			m_backgroundTexDescriptorPool = background.getTexDescriptorLayout().createPool( "EnvironmentMapTex", 6u );
			uint32_t face = 0u;

			for ( auto & pass : m_passes )
			{
				pass->initialise( *m_renderPass
					, background
					, *m_backgroundUboDescriptorPool
					, *m_backgroundTexDescriptorPool );
			}

			auto & cmd = *m_generateMipmaps.commandBuffer;
			cmd.begin();
			m_timer->beginPass( cmd, 6u );
			cmd.beginDebugBlock( { m_environmentMap->getTexture()->getName() + " Mipmaps Generation"
				, makeFloatArray( getEngine()->getNextRainbowColour() ) } );
			auto & image = m_environmentMap->getTexture()->getTexture();
			image.generateMipmaps( cmd
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				, VK_IMAGE_LAYOUT_UNDEFINED
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
			cmd.endDebugBlock();
			m_timer->endPass( cmd, 6u );
			cmd.end();
		}

		return true;
	}

	void EnvironmentMap::cleanup()
	{
		for ( auto & pass : m_passes )
		{
			pass->cleanup();
		}

		m_backgroundUboDescriptorPool.reset();
		m_backgroundTexDescriptorPool.reset();
	}
	
	void EnvironmentMap::update( CpuUpdater & updater )
	{
		// Compute for next frame (or first)
		if ( m_first || ( m_render % 5u ) == 4u )
		{
			updater.node = &m_node;

			for ( auto & pass : m_passes )
			{
				pass->update( updater );
			}
		}
	}

	void EnvironmentMap::update( GpuUpdater & updater )
	{
		// Compute for next frame (or first)
		if ( m_first || ( m_render % 5u ) == 4u )
		{
			for ( auto & pass : m_passes )
			{
				pass->update( updater );
			}
		}
	}

	ashes::Semaphore const & EnvironmentMap::render( ashes::Semaphore const & toWait )
	{
		ashes::Semaphore const * result = &toWait;
		auto timerBlock = m_timer->start();

		// Render current frame (or first)
		if ( m_first || ( m_render % 5u ) == 0u )
		{
			for ( auto & pass : m_passes )
			{
				result = &pass->render( *result );
			}

			timerBlock->notifyPassRender( uint32_t( m_passes.size() ) );
			result = &m_generateMipmaps.submit( *m_device.graphicsQueue, *result );
			m_render = 0u;
		}

		m_render = m_first
			? m_render
			: m_render + 1u;
		m_first = false;
		return *result;
	}

	VkExtent3D const & EnvironmentMap::getSize()const
	{
		return m_environmentMap->getTexture()->getDimensions();
	}
}
