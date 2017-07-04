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

		TextureUnit DoInitialisePoint( Engine & p_engine, Size const & p_size )
		{
			String const l_name = cuT( "EnvironmentMap" );

			SamplerSPtr l_sampler;

			if ( p_engine.GetSamplerCache().Has( l_name ) )
			{
				l_sampler = p_engine.GetSamplerCache().Find( l_name );
			}
			else
			{
				l_sampler = p_engine.GetSamplerCache().Add( l_name );
				l_sampler->SetInterpolationMode( InterpolationFilter::eMin
					, InterpolationMode::eLinear );
				l_sampler->SetInterpolationMode( InterpolationFilter::eMag
					, InterpolationMode::eLinear );
				l_sampler->SetWrappingMode( TextureUVW::eU
					, WrapMode::eClampToEdge );
				l_sampler->SetWrappingMode( TextureUVW::eV
					, WrapMode::eClampToEdge );
				l_sampler->SetWrappingMode( TextureUVW::eW
					, WrapMode::eClampToEdge );
			}

			auto l_texture = p_engine.GetRenderSystem()->CreateTexture( TextureType::eCube
				, AccessType::eNone
				, AccessType::eRead | AccessType::eWrite
				, PixelFormat::eA8R8G8B8
				, p_size );
			TextureUnit l_unit{ p_engine };
			l_unit.SetTexture( l_texture );
			l_unit.SetSampler( l_sampler );

			for ( auto & l_image : *l_texture )
			{
				l_image->InitialiseSource();
			}

			return l_unit;
		}

		EnvironmentMap::EnvironmentMapPasses DoCreatePasses( EnvironmentMap & p_map
			, SceneNode & p_node )
		{
			static Point3r const l_position;
			std::array< SceneNodeSPtr, size_t( CubeMapFace::eCount ) > l_nodes
			{
				std::make_shared< SceneNode >( cuT( "EnvironmentMap_PosX" ), *p_node.GetScene() ),
				std::make_shared< SceneNode >( cuT( "EnvironmentMap_NegX" ), *p_node.GetScene() ),
				std::make_shared< SceneNode >( cuT( "EnvironmentMap_PosY" ), *p_node.GetScene() ),
				std::make_shared< SceneNode >( cuT( "EnvironmentMap_NegY" ), *p_node.GetScene() ),
				std::make_shared< SceneNode >( cuT( "EnvironmentMap_PosZ" ), *p_node.GetScene() ),
				std::make_shared< SceneNode >( cuT( "EnvironmentMap_NegZ" ), *p_node.GetScene() ),
			};
			std::array< Quaternion, size_t( CubeMapFace::eCount ) > l_orients
			{
				Quaternion::from_matrix( matrix::look_at( l_position, Point3r{ -1, +0, +0 }, Point3r{ +0, -1, +0 } ) ),// Positive X
				Quaternion::from_matrix( matrix::look_at( l_position, Point3r{ +1, +0, +0 }, Point3r{ +0, -1, +0 } ) ),// Negative X
				Quaternion::from_matrix( matrix::look_at( l_position, Point3r{ +0, -1, +0 }, Point3r{ +0, +0, +1 } ) ),// Positive Y
				Quaternion::from_matrix( matrix::look_at( l_position, Point3r{ +0, +1, +0 }, Point3r{ +0, +0, -1 } ) ),// Negative Y
				Quaternion::from_matrix( matrix::look_at( l_position, Point3r{ +0, +0, -1 }, Point3r{ +0, -1, +0 } ) ),// Positive Z
				Quaternion::from_matrix( matrix::look_at( l_position, Point3r{ +0, +0, +1 }, Point3r{ +0, -1, +0 } ) ),// Negative Z
			};

			auto i = 0u;

			for ( auto & l_node : l_nodes )
			{
				l_node->SetOrientation( l_orients[i] );
				++i;
			}

			return EnvironmentMap::EnvironmentMapPasses
			{
				{
					std::make_unique< EnvironmentMapPass >( p_map, l_nodes[0], p_node ),
					std::make_unique< EnvironmentMapPass >( p_map, l_nodes[1], p_node ),
					std::make_unique< EnvironmentMapPass >( p_map, l_nodes[2], p_node ),
					std::make_unique< EnvironmentMapPass >( p_map, l_nodes[3], p_node ),
					std::make_unique< EnvironmentMapPass >( p_map, l_nodes[4], p_node ),
					std::make_unique< EnvironmentMapPass >( p_map, l_nodes[5], p_node ),
				}
			};
		}
	}

	uint32_t EnvironmentMap::m_count = 0u;

	EnvironmentMap::EnvironmentMap( Engine & p_engine
		, SceneNode  & p_node )
		: OwnedBy< Engine >{ p_engine }
		, m_environmentMap{ DoInitialisePoint( p_engine, MapSize ) }
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
		bool l_return = true;

		if ( !m_frameBuffer )
		{
			m_frameBuffer = GetEngine()->GetRenderSystem()->CreateFrameBuffer();
			l_return = m_frameBuffer->Create();

			if ( l_return )
			{
				l_return = m_frameBuffer->Initialise( MapSize );
			}

			if ( l_return )
			{
				constexpr float l_component = std::numeric_limits< float >::max();
				m_frameBuffer->SetClearColour( l_component, l_component, l_component, l_component );
				auto l_texture = m_environmentMap.GetTexture();
				l_texture->Initialise();
				uint32_t i = 0;

				for ( auto & l_attach : m_colourAttachs )
				{
					l_attach = m_frameBuffer->CreateAttachment( l_texture, CubeMapFace( i++ ) );
					l_attach->SetTarget( TextureType::eTwoDimensions );
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

			for ( auto & l_pass : m_passes )
			{
				l_pass->Initialise( MapSize );
			}
		}

		return l_return;
	}

	void EnvironmentMap::Cleanup()
	{
		if ( m_frameBuffer )
		{
			for ( auto & l_pass : m_passes )
			{
				l_pass->Cleanup();
			}

			m_frameBuffer->Bind();
			m_frameBuffer->DetachAll();
			m_frameBuffer->Unbind();
			m_depthAttach.reset();

			for ( auto & l_attach : m_colourAttachs )
			{
				l_attach.reset();
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
		for ( auto & l_pass : m_passes )
		{
			l_pass->Update( m_node, p_queues );
		}
	}

	void EnvironmentMap::Render()
	{
		uint32_t l_face = 0;

		for ( auto & l_attach : m_colourAttachs )
		{
			m_frameBuffer->Bind( FrameBufferTarget::eDraw );
			l_attach->Attach( AttachmentPoint::eColour, 0u );
			m_frameBuffer->SetDrawBuffer( l_attach );
			m_frameBuffer->Clear( BufferComponent::eDepth | BufferComponent::eColour );
			m_passes[l_face]->Render();
			m_frameBuffer->Unbind();
			l_face++;
		}
	}
}
