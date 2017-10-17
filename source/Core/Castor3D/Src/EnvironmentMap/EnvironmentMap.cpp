#include "EnvironmentMap.hpp"

#include "Engine.hpp"

#include "EnvironmentMap/EnvironmentMapPass.hpp"
#include "FrameBuffer/DepthStencilRenderBuffer.hpp"
#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/RenderBufferAttachment.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "Render/RenderPipeline.hpp"
#include "Scene/BillboardList.hpp"
#include "Scene/SceneNode.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <GlslSource.hpp>

using namespace castor;

namespace castor3d
{
	namespace
	{
		static Size const MapSize{ 1024, 1024 };

		TextureUnit doInitialisePoint( Engine & engine
			, Size const & p_size
			, MaterialType p_type )
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
				sampler->setInterpolationMode( InterpolationFilter::eMin
					, InterpolationMode::eLinear );
				sampler->setInterpolationMode( InterpolationFilter::eMag
					, InterpolationMode::eLinear );

				if ( p_type == MaterialType::ePbrMetallicRoughness
					|| p_type == MaterialType::ePbrSpecularGlossiness )
				{
					sampler->setInterpolationMode( InterpolationFilter::eMip
						, InterpolationMode::eLinear );
				}

				sampler->setWrappingMode( TextureUVW::eU
					, WrapMode::eClampToEdge );
				sampler->setWrappingMode( TextureUVW::eV
					, WrapMode::eClampToEdge );
				sampler->setWrappingMode( TextureUVW::eW
					, WrapMode::eClampToEdge );
			}

			auto texture = engine.getRenderSystem()->createTexture( TextureType::eCube
				, AccessType::eNone
				, AccessType::eRead | AccessType::eWrite
				, PixelFormat::eRGBA32F
				, p_size );
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
		bool result = true;

		if ( !m_frameBuffer )
		{
			auto & scene = *m_node.getScene();
			m_frameBuffer = getEngine()->getRenderSystem()->createFrameBuffer();
			result = m_frameBuffer->initialise();

			if ( result )
			{
				constexpr float PixelComponents = std::numeric_limits< float >::max();
				m_frameBuffer->setClearColour( PixelComponents, PixelComponents, PixelComponents, PixelComponents );
				auto texture = m_environmentMap.getTexture();
				texture->initialise();

				if ( scene.getMaterialsType() == MaterialType::ePbrMetallicRoughness
					|| scene.getMaterialsType() == MaterialType::ePbrSpecularGlossiness )
				{
					texture->bind( 0 );
					texture->generateMipmaps();
					texture->unbind( 0 );
				}

				uint32_t i = 0;

				for ( auto & attach : m_colourAttachs )
				{
					attach = m_frameBuffer->createAttachment( texture, CubeMapFace( i++ ) );
					attach->setTarget( TextureType::eTwoDimensions );
				}

				m_depthBuffer = m_frameBuffer->createDepthStencilRenderBuffer( PixelFormat::eD32F );
				m_depthBuffer->create();
				m_depthBuffer->initialise( MapSize );

				m_depthAttach = m_frameBuffer->createAttachment( m_depthBuffer );
				m_frameBuffer->bind( FrameBufferTarget::eDraw );
				m_frameBuffer->attach( AttachmentPoint::eDepth, m_depthAttach );
				m_frameBuffer->unbind();
			}

			m_frameBuffer->bind();
			m_frameBuffer->setDrawBuffers( FrameBuffer::AttachArray{} );
			m_frameBuffer->unbind();

			for ( auto & pass : m_passes )
			{
				pass->initialise( MapSize );
			}
		}

		return result;
	}

	void EnvironmentMap::cleanup()
	{
		if ( m_frameBuffer )
		{
			for ( auto & pass : m_passes )
			{
				pass->cleanup();
			}

			m_frameBuffer->bind();
			m_frameBuffer->detachAll();
			m_frameBuffer->unbind();
			m_depthAttach.reset();

			for ( auto & attach : m_colourAttachs )
			{
				attach.reset();
			}

			m_depthBuffer->cleanup();
			m_depthBuffer->destroy();
			m_depthBuffer.reset();
			m_environmentMap.cleanup();
			m_frameBuffer->cleanup();
			m_frameBuffer.reset();
		}
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
		uint32_t face = 0u;
		m_render++;

		if ( m_render == 5u )
		{
			for ( auto & attach : m_colourAttachs )
			{
				m_frameBuffer->bind( FrameBufferTarget::eDraw );
				attach->attach( AttachmentPoint::eColour, 0u );
				m_frameBuffer->setDrawBuffer( attach );
				m_frameBuffer->clear( BufferComponent::eDepth | BufferComponent::eColour );
				m_passes[face]->render();
				m_frameBuffer->unbind();
				face++;
			}

			auto & scene = *m_node.getScene();

			if ( scene.getMaterialsType() == MaterialType::ePbrMetallicRoughness
				|| scene.getMaterialsType() == MaterialType::ePbrSpecularGlossiness )
			{
				m_environmentMap.getTexture()->bind( 0 );
				m_environmentMap.getTexture()->generateMipmaps();
				m_environmentMap.getTexture()->unbind( 0 );
			}

			m_render = 0u;
		}
	}

	void EnvironmentMap::debugDisplay( castor::Size const & size, uint32_t index )
	{
		Size displaySize{ 128u, 128u };
		Position position{ int32_t( displaySize.getWidth() * 4 * index ), int32_t( displaySize.getHeight() * 4 ) };
		getEngine()->getRenderSystem()->getCurrentContext()->renderTextureCube( position
			, displaySize
			, *m_environmentMap.getTexture() );
	}
}
