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
		static String const ReflectionMapUbo = cuT( "ReflectionMap" );
		static String const WorldLightPosition = cuT( "c3d_v3WorldLightPosition" );
		static Size const MapSize{ 512, 512 };

		std::vector< TextureUnit > DoInitialisePoint( Engine & p_engine, Size const & p_size )
		{
			std::vector< TextureUnit > l_result;
			String const l_name = cuT( "ReflectionMap_" );

			for ( auto i = 0u; i < GLSL::PointShadowMapCount; ++i )
			{
				SamplerSPtr l_sampler;

				if ( p_engine.GetSamplerCache().Has( l_name + string::to_string( i ) ) )
				{
					l_sampler = p_engine.GetSamplerCache().Find( l_name + string::to_string( i ) );
				}
				else
				{
					l_sampler = p_engine.GetSamplerCache().Add( l_name + string::to_string( i ) );
					l_sampler->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
					l_sampler->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
					l_sampler->SetWrappingMode( TextureUVW::eU, WrapMode::eClampToEdge );
					l_sampler->SetWrappingMode( TextureUVW::eV, WrapMode::eClampToEdge );
					l_sampler->SetWrappingMode( TextureUVW::eW, WrapMode::eClampToEdge );
					l_sampler->SetComparisonMode( ComparisonMode::eRefToTexture );
					l_sampler->SetComparisonFunc( ComparisonFunc::eLEqual );
				}

				auto l_texture = p_engine.GetRenderSystem()->CreateTexture(
					TextureType::eCube,
					AccessType::eNone,
					AccessType::eRead | AccessType::eWrite,
					PixelFormat::eL32F,
					p_size );
				l_result.emplace_back( p_engine );
				TextureUnit & l_unit = l_result.back();
				l_unit.SetTexture( l_texture );
				l_unit.SetSampler( l_sampler );

				for ( auto & l_image : *l_texture )
				{
					l_image->InitialiseSource();
				}
			}

			return l_result;
		}
	}

	ReflectionMap::ReflectionMap( Engine & p_engine )
		: OwnedBy< Engine >{ p_engine }
		, m_reflectionMaps{ DoInitialisePoint( p_engine, MapSize ) }
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
				m_colourAttachs.resize( m_reflectionMaps.size() );
				auto l_it = m_colourAttachs.begin();

				for ( auto & l_map : m_reflectionMaps )
				{
					auto l_texture = l_map.GetTexture();
					l_texture->Initialise();
					uint32_t i = 0;

					for ( auto & l_attach : *l_it )
					{
						l_attach = m_frameBuffer->CreateAttachment( l_texture, CubeMapFace( i++ ) );
						l_attach->SetTarget( TextureType::eTwoDimensions );
					}

					++l_it;
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
		}

		return l_return;
	}

	void ReflectionMap::Cleanup()
	{
		for ( auto & l_it : m_passes )
		{
			l_it.second->Cleanup();
		}

		if ( m_frameBuffer )
		{
			m_frameBuffer->Bind();
			m_frameBuffer->DetachAll();
			m_frameBuffer->Unbind();
			m_depthAttach.reset();

			for ( auto & l_attach : m_colourAttachs )
			{
				for ( auto & l_face : l_attach )
				{
					l_face.reset();
				}
			}

			m_depthBuffer->Cleanup();
			m_depthBuffer->Destroy();
			m_depthBuffer.reset();

			for ( auto & l_map : m_reflectionMaps )
			{
				l_map.Cleanup();
			}

			m_frameBuffer->Cleanup();
			m_frameBuffer->Destroy();
			m_frameBuffer.reset();
		}

		for ( auto l_buffer : m_geometryBuffers )
		{
			l_buffer->Cleanup();
		}

		m_geometryBuffers.clear();
	}
	
	void ReflectionMap::Update( RenderQueueArray & p_queues )
	{
		if ( !m_passes.empty() )
		{
			int32_t const l_max = m_reflectionMaps.size();
			auto l_it = m_passes.begin();

			for ( auto i = 0; i < l_max && l_it != m_passes.end(); ++i, ++l_it )
			{
				l_it->second->Update( p_queues, i );
			}
		}
	}

	void ReflectionMap::Render()
	{
		if ( !m_passes.empty() )
		{
			auto l_it = m_passes.begin();
			int32_t const l_max = m_reflectionMaps.size();

			for ( int32_t i = 0; i < l_max && l_it != m_passes.end(); ++i, ++l_it )
			{
				uint32_t l_face = 0;

				for ( auto & l_attach : m_colourAttachs[i] )
				{
					m_frameBuffer->Bind( FrameBufferTarget::eDraw );
					l_attach->Attach( AttachmentPoint::eColour, 0u );
					m_frameBuffer->SetDrawBuffer( l_attach );
					m_frameBuffer->Clear( BufferComponent::eDepth | BufferComponent::eColour );
					l_it->second->Render( l_face++ );
					m_frameBuffer->Unbind();
				}
			}
		}
	}

	void ReflectionMap::AddSource( SceneNode & p_node )
	{
		auto l_pass = std::make_shared< ReflectionMapPass >( *GetEngine(), p_node, *this );
		auto l_size = MapSize;
		GetEngine()->PostEvent( MakeFunctorEvent( EventType::ePreRender
			, [l_pass, l_size]()
			{
				l_pass->Initialise( l_size );
			} ) );
		m_passes.emplace( &p_node, l_pass );
	}
}
