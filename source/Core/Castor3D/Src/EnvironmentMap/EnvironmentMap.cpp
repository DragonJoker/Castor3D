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

using namespace Castor;

namespace Castor3D
{
	namespace
	{
		static Size const MapSize{ 1024, 1024 };

		TextureUnit DoInitialisePoint( Engine & engine
			, Size const & p_size
			, MaterialType p_type )
		{
			String const name = cuT( "EnvironmentMap" );

			SamplerSPtr sampler;

			if ( engine.GetSamplerCache().Has( name ) )
			{
				sampler = engine.GetSamplerCache().Find( name );
			}
			else
			{
				sampler = engine.GetSamplerCache().Add( name );
				sampler->SetInterpolationMode( InterpolationFilter::eMin
					, InterpolationMode::eLinear );
				sampler->SetInterpolationMode( InterpolationFilter::eMag
					, InterpolationMode::eLinear );

				if ( p_type == MaterialType::ePbrMetallicRoughness
					|| p_type == MaterialType::ePbrSpecularGlossiness )
				{
					sampler->SetInterpolationMode( InterpolationFilter::eMip
						, InterpolationMode::eLinear );
				}

				sampler->SetWrappingMode( TextureUVW::eU
					, WrapMode::eClampToEdge );
				sampler->SetWrappingMode( TextureUVW::eV
					, WrapMode::eClampToEdge );
				sampler->SetWrappingMode( TextureUVW::eW
					, WrapMode::eClampToEdge );
			}

			auto texture = engine.GetRenderSystem()->CreateTexture( TextureType::eCube
				, AccessType::eNone
				, AccessType::eRead | AccessType::eWrite
				, PixelFormat::eA8R8G8B8
				, p_size );
			TextureUnit unit{ engine };
			unit.SetTexture( texture );
			unit.SetSampler( sampler );

			for ( auto & image : *texture )
			{
				image->InitialiseSource();
			}

			return unit;
		}

		EnvironmentMap::EnvironmentMapPasses DoCreatePasses( EnvironmentMap & p_map
			, SceneNode & p_node )
		{
			static Point3r const position;
			std::array< SceneNodeSPtr, size_t( CubeMapFace::eCount ) > nodes
			{
				std::make_shared< SceneNode >( cuT( "EnvironmentMap_PosX" ), *p_node.GetScene() ),
				std::make_shared< SceneNode >( cuT( "EnvironmentMap_NegX" ), *p_node.GetScene() ),
				std::make_shared< SceneNode >( cuT( "EnvironmentMap_PosY" ), *p_node.GetScene() ),
				std::make_shared< SceneNode >( cuT( "EnvironmentMap_NegY" ), *p_node.GetScene() ),
				std::make_shared< SceneNode >( cuT( "EnvironmentMap_PosZ" ), *p_node.GetScene() ),
				std::make_shared< SceneNode >( cuT( "EnvironmentMap_NegZ" ), *p_node.GetScene() ),
			};
			std::array< Quaternion, size_t( CubeMapFace::eCount ) > orients
			{
				Quaternion::from_matrix( matrix::look_at( position, Point3r{ -1, +0, +0 }, Point3r{ +0, -1, +0 } ) ),// Positive X
				Quaternion::from_matrix( matrix::look_at( position, Point3r{ +1, +0, +0 }, Point3r{ +0, -1, +0 } ) ),// Negative X
				Quaternion::from_matrix( matrix::look_at( position, Point3r{ +0, -1, +0 }, Point3r{ +0, +0, +1 } ) ),// Positive Y
				Quaternion::from_matrix( matrix::look_at( position, Point3r{ +0, +1, +0 }, Point3r{ +0, +0, -1 } ) ),// Negative Y
				Quaternion::from_matrix( matrix::look_at( position, Point3r{ +0, +0, -1 }, Point3r{ +0, -1, +0 } ) ),// Positive Z
				Quaternion::from_matrix( matrix::look_at( position, Point3r{ +0, +0, +1 }, Point3r{ +0, -1, +0 } ) ),// Negative Z
			};

			auto i = 0u;

			for ( auto & node : nodes )
			{
				node->SetOrientation( orients[i] );
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
		, m_environmentMap{ DoInitialisePoint( engine, MapSize, p_node.GetScene()->GetMaterialsType() ) }
		, m_node{ p_node }
		, m_index{ ++m_count }
		, m_passes( DoCreatePasses( *this, p_node ) )
	{
	}

	EnvironmentMap::~EnvironmentMap()
	{
	}

	bool EnvironmentMap::Initialise()
	{
		bool result = true;

		if ( !m_frameBuffer )
		{
			auto & scene = *m_node.GetScene();
			m_frameBuffer = GetEngine()->GetRenderSystem()->CreateFrameBuffer();
			result = m_frameBuffer->Create();

			if ( result )
			{
				result = m_frameBuffer->Initialise( MapSize );
			}

			if ( result )
			{
				constexpr float component = std::numeric_limits< float >::max();
				m_frameBuffer->SetClearColour( component, component, component, component );
				auto texture = m_environmentMap.GetTexture();
				texture->Initialise();

				if ( scene.GetMaterialsType() == MaterialType::ePbrMetallicRoughness
					|| scene.GetMaterialsType() == MaterialType::ePbrSpecularGlossiness )
				{
					texture->Bind( 0 );
					texture->GenerateMipmaps();
					texture->Unbind( 0 );
				}

				uint32_t i = 0;

				for ( auto & attach : m_colourAttachs )
				{
					attach = m_frameBuffer->CreateAttachment( texture, CubeMapFace( i++ ) );
					attach->SetTarget( TextureType::eTwoDimensions );
				}

				m_depthBuffer = m_frameBuffer->CreateDepthStencilRenderBuffer( PixelFormat::eD32F );
				m_depthBuffer->Create();
				m_depthBuffer->Initialise( MapSize );

				m_depthAttach = m_frameBuffer->CreateAttachment( m_depthBuffer );
				m_frameBuffer->Bind( FrameBufferTarget::eDraw );
				m_frameBuffer->Attach( AttachmentPoint::eDepth, m_depthAttach );
				m_frameBuffer->Unbind();
			}

			m_frameBuffer->Bind();
			m_frameBuffer->SetDrawBuffers( FrameBuffer::AttachArray{} );
			m_frameBuffer->Unbind();

			for ( auto & pass : m_passes )
			{
				pass->Initialise( MapSize );
			}

			if ( scene.GetMaterialsType() == MaterialType::ePbrMetallicRoughness
				|| scene.GetMaterialsType() == MaterialType::ePbrSpecularGlossiness )
			{
				m_ibl = std::make_unique< IblTextures >( scene );
			}
		}

		return result;
	}

	void EnvironmentMap::Cleanup()
	{
		m_ibl.reset();

		if ( m_frameBuffer )
		{
			for ( auto & pass : m_passes )
			{
				pass->Cleanup();
			}

			m_frameBuffer->Bind();
			m_frameBuffer->DetachAll();
			m_frameBuffer->Unbind();
			m_depthAttach.reset();

			for ( auto & attach : m_colourAttachs )
			{
				attach.reset();
			}

			m_depthBuffer->Cleanup();
			m_depthBuffer->Destroy();
			m_depthBuffer.reset();
			m_environmentMap.Cleanup();
			m_frameBuffer->Cleanup();
			m_frameBuffer->Destroy();
			m_frameBuffer.reset();
		}
	}
	
	void EnvironmentMap::Update( RenderQueueArray & p_queues )
	{
		for ( auto & pass : m_passes )
		{
			pass->Update( m_node, p_queues );
		}
	}

	void EnvironmentMap::Render()
	{
		uint32_t face = 0u;
		m_render++;

		if ( m_render == 5u )
		{
			for ( auto & attach : m_colourAttachs )
			{
				m_frameBuffer->Bind( FrameBufferTarget::eDraw );
				attach->Attach( AttachmentPoint::eColour, 0u );
				m_frameBuffer->SetDrawBuffer( attach );
				m_frameBuffer->Clear( BufferComponent::eDepth | BufferComponent::eColour );
				m_passes[face]->Render();
				m_frameBuffer->Unbind();
				face++;
			}

			if ( m_ibl )
			{
				m_ibl->Update( *m_environmentMap.GetTexture() );
			}

			m_render = 0u;
		}
	}
}
