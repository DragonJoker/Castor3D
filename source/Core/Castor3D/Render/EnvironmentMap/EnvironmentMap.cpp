#include "Castor3D/Render/EnvironmentMap/EnvironmentMap.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/SamplerCache.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/EnvironmentMap/EnvironmentMapPass.hpp"
#include "Castor3D/Render/PBR/IblTextures.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Background/Background.hpp"

#include <CastorUtils/Miscellaneous/BitSize.hpp>

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
			String const name = cuT( "EnvironmentMap_" ) + string::toString( id++, 10, std::locale{ "C" } );
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

		TextureUnitSPtr doCreateTexture( Engine & engine
			, Size const & size )
		{
			ashes::ImageCreateInfo colour
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
			auto texture = std::make_shared< TextureLayout >( *engine.getRenderSystem()
				, colour
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, cuT( "EnvironmentMap" ) );
			auto result = std::make_shared< TextureUnit >( engine );
			result->setTexture( texture );
			result->setSampler( doCreateSampler( engine, texture->getMipmapCount() ) );

			for ( auto & image : *texture )
			{
				image->initialiseSource();
			}

			return result;
		}

		EnvironmentMap::EnvironmentMapPasses doCreatePasses( EnvironmentMap & map
			, SceneNode & node )
		{
			static castor::Point3f const position;
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
		, m_environmentMap{ doCreateTexture( engine, MapSize ) }
		, m_node{ node }
		, m_index{ ++m_count }
		, m_passes( doCreatePasses( *this, node ) )
	{
		log::trace << "Created EnvironmentMap" << node.getName() << std::endl;
	}

	EnvironmentMap::~EnvironmentMap()
	{
	}

	bool EnvironmentMap::initialise()
	{
		auto & device = getCurrentRenderDevice( *this );
		m_environmentMap->initialise();
		ashes::ImageCreateInfo depthStencil
		{
			0u,
			VK_IMAGE_TYPE_2D,
			device.selectSuitableDepthStencilFormat( VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT ),
			{ MapSize[0], MapSize[1], 1u },
			1u,
			1u,
			VK_SAMPLE_COUNT_1_BIT,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		};
		m_depthBuffer = makeImage( device
			, std::move( depthStencil )
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, "EnvironmentMapImage" );
		m_depthBufferView = m_depthBuffer->createView( "EnvironmentMap"
			, VK_IMAGE_VIEW_TYPE_2D
			, m_depthBuffer->getFormat() );

		ashes::VkAttachmentDescriptionArray attaches
		{
			{
				0u,
				m_depthBuffer->getFormat(),
				VK_SAMPLE_COUNT_1_BIT,
				VK_ATTACHMENT_LOAD_OP_CLEAR,
				VK_ATTACHMENT_STORE_OP_STORE,
				VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				VK_ATTACHMENT_STORE_OP_DONT_CARE,
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			},
			{
				0u,
				m_environmentMap->getTexture()->getPixelFormat(),
				VK_SAMPLE_COUNT_1_BIT,
				VK_ATTACHMENT_LOAD_OP_CLEAR,
				VK_ATTACHMENT_STORE_OP_STORE,
				VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				VK_ATTACHMENT_STORE_OP_DONT_CARE,
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			}
		};
		ashes::SubpassDescriptionArray subpasses;
		subpasses.emplace_back( ashes::SubpassDescription
			{
				0u,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				{},
				{ { 1u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } },
				{},
				VkAttachmentReference{ 0u, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL },
				{},
			} );
		ashes::VkSubpassDependencyArray dependencies
		{
			{
				VK_SUBPASS_EXTERNAL,
				0u,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				VK_DEPENDENCY_BY_REGION_BIT,
			},
			{
				0u,
				VK_SUBPASS_EXTERNAL,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				VK_ACCESS_SHADER_READ_BIT,
				VK_DEPENDENCY_BY_REGION_BIT,
			}
		};
		ashes::RenderPassCreateInfo createInfo
		{
			0u,
			std::move( attaches ),
			std::move( subpasses ),
			std::move( dependencies ),
		};
		m_renderPass = device->createRenderPass( "EnvironmentMap"
			, std::move( createInfo ) );
		auto & background = *m_node.getScene()->getBackground();
		m_backgroundUboDescriptorPool = background.getUboDescriptorLayout().createPool( "EnvironmentMapUbo", 6u );
		m_backgroundTexDescriptorPool = background.getTexDescriptorLayout().createPool( "EnvironmentMapTex", 6u );
		uint32_t face = 0u;

		for ( auto & pass : m_passes )
		{
			pass->initialise( MapSize
				, face
				, *m_renderPass
				, background
				, *m_backgroundUboDescriptorPool
				, *m_backgroundTexDescriptorPool );
			++face;
		}

		return true;
	}

	void EnvironmentMap::cleanup()
	{
		m_ibl.reset();

		for ( auto & pass : m_passes )
		{
			pass->cleanup();
		}

		m_backgroundUboDescriptorPool.reset();
		m_backgroundTexDescriptorPool.reset();
		m_renderPass.reset();
		m_depthBuffer.reset();
		m_environmentMap->cleanup();
	}
	
	void EnvironmentMap::update( RenderQueueArray & queues )
	{
		for ( auto & pass : m_passes )
		{
			pass->update( m_node, queues );
		}
	}

	void EnvironmentMap::update()
	{
		for ( auto & pass : m_passes )
		{
			pass->update();
		}
	}

	ashes::Semaphore const & EnvironmentMap::render( ashes::Semaphore const & toWait )
	{
		ashes::Semaphore const * result = &toWait;
		m_render++;

		if ( m_render >= 5u )
		{
			for ( auto & pass : m_passes )
			{
				result = &pass->render( *result );
			}

			m_environmentMap->getTexture()->generateMipmaps();
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

	VkExtent3D const & EnvironmentMap::getSize()const
	{
		return m_environmentMap->getTexture()->getDimensions();
	}
}
