#include "ReflectionMap.hpp"

#include "Engine.hpp"

#include "FrameBuffer/DepthStencilRenderBuffer.hpp"
#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/RenderBufferAttachment.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "Render/RenderPipeline.hpp"
#include "Scene/BillboardList.hpp"
#include "Scene/SceneNode.hpp"
#include "Shader/ShaderProgram.hpp"
#include "ReflectionMap/ReflectionMapPass.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <GlslSource.hpp>

using namespace Castor;

namespace Castor3D
{
	namespace
	{
		static Size const MapSize{ 512, 512 };

		TextureUnit DoInitialisePoint( Engine & p_engine, Size const & p_size )
		{
			String const l_name = cuT( "ReflectionMap" );

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

		ReflectionMap::ReflectionMapPasses DoCreatePasses( ReflectionMap & p_map
			, SceneNode & p_node )
		{
			std::array< SceneNodeSPtr, size_t( CubeMapFace::eCount ) > l_nodes
			{
				std::make_shared< SceneNode >( cuT( "ReflectionMap_PosX" ), *p_node.GetScene() ),
				std::make_shared< SceneNode >( cuT( "ReflectionMap_NegX" ), *p_node.GetScene() ),
				std::make_shared< SceneNode >( cuT( "ReflectionMap_PosY" ), *p_node.GetScene() ),
				std::make_shared< SceneNode >( cuT( "ReflectionMap_NegY" ), *p_node.GetScene() ),
				std::make_shared< SceneNode >( cuT( "ReflectionMap_PosZ" ), *p_node.GetScene() ),
				std::make_shared< SceneNode >( cuT( "ReflectionMap_NegZ" ), *p_node.GetScene() ),
			};
			std::array< Quaternion, size_t( CubeMapFace::eCount ) > l_orients
			{
				Quaternion::from_axes( Point3r{ +0, +0, +1 }, Point3r{ +0, -1, +0 }, Point3r{ +1, +0, +0 } ),
				Quaternion::from_axes( Point3r{ +0, +0, -1 }, Point3r{ +0, -1, +0 }, Point3r{ -1, +0, +0 } ),
				Quaternion::from_axes( Point3r{ +1, +0, +0 }, Point3r{ +0, +0, +1 }, Point3r{ +0, +1, +0 } ),
				Quaternion::from_axes( Point3r{ -1, +0, +0 }, Point3r{ +0, +0, -1 }, Point3r{ +0, -1, +0 } ),
				Quaternion::from_axes( Point3r{ -1, +0, +0 }, Point3r{ +0, -1, +0 }, Point3r{ +0, +0, +1 } ),
				Quaternion::from_axes( Point3r{ +1, +0, +0 }, Point3r{ +0, -1, +0 }, Point3r{ +0, +0, -1 } ),
			};

			auto i = 0u;

			for ( auto & l_node : l_nodes )
			{
				l_node->SetOrientation( l_orients[i] );
				++i;
			}

			return ReflectionMap::ReflectionMapPasses
			{
				{
					ReflectionMapPass{ p_map, l_nodes[0] },
					ReflectionMapPass{ p_map, l_nodes[1] },
					ReflectionMapPass{ p_map, l_nodes[2] },
					ReflectionMapPass{ p_map, l_nodes[3] },
					ReflectionMapPass{ p_map, l_nodes[4] },
					ReflectionMapPass{ p_map, l_nodes[5] },
				}
			};
		}
	}

	ReflectionMap::ReflectionMap( Engine & p_engine
		, SceneNode  & p_node )
		: OwnedBy< Engine >{ p_engine }
		, m_reflectionMap{ DoInitialisePoint( p_engine, MapSize ) }
		, m_node{ p_node }
		, m_passes{ DoCreatePasses( *this, p_node ) }
	{
	}

	ReflectionMap::~ReflectionMap()
	{
	}

	bool ReflectionMap::Initialise()
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
				auto l_texture = m_reflectionMap.GetTexture();
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
				l_pass.Initialise( MapSize );
			}
		}

		return l_return;
	}

	void ReflectionMap::Cleanup()
	{
		if ( m_frameBuffer )
		{
			for ( auto & l_pass : m_passes )
			{
				l_pass.Cleanup();
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
			m_reflectionMap.Cleanup();
			m_frameBuffer->Cleanup();
			m_frameBuffer->Destroy();
			m_frameBuffer.reset();
		}
	}
	
	void ReflectionMap::Update( RenderQueueArray & p_queues )
	{
		for ( auto & l_pass : m_passes )
		{
			l_pass.Update( m_node, p_queues );
		}
	}

	void ReflectionMap::Render()
	{
		uint32_t l_face = 0;

		for ( auto & l_attach : m_colourAttachs )
		{
			m_frameBuffer->Bind( FrameBufferTarget::eDraw );
			l_attach->Attach( AttachmentPoint::eColour, 0u );
			m_frameBuffer->SetDrawBuffer( l_attach );
			m_frameBuffer->Clear( BufferComponent::eDepth | BufferComponent::eColour );
			m_passes[l_face].Render();
			m_frameBuffer->Unbind();
		}
	}
}
