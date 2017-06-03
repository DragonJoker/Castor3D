#include "ShadowMapPoint.hpp"

#include "Engine.hpp"

#include "FrameBuffer/DepthStencilRenderBuffer.hpp"
#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/RenderBufferAttachment.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "Mesh/Submesh.hpp"
#include "Render/RenderPipeline.hpp"
#include "Scene/BillboardList.hpp"
#include "Scene/Light/Light.hpp"
#include "Shader/ShaderProgram.hpp"
#include "ShadowMap/ShadowMapPassPoint.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <GlslSource.hpp>

using namespace Castor;

namespace Castor3D
{
	namespace
	{
		static String const ShadowMapUbo = cuT( "ShadowMap" );
		static String const WorldLightPosition = cuT( "c3d_v3WorldLightPosition" );
		static String const FarPlane = cuT( "c3d_fFarPlane" );
		static String const ShadowMatrix = cuT( "c3d_mtxShadowMatrix" );

		std::vector< TextureUnit > DoInitialisePoint( Engine & p_engine, Size const & p_size )
		{
			std::vector< TextureUnit > l_result;
			String const l_name = cuT( "ShadowMap_Point_" );

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

	ShadowMapPoint::ShadowMapPoint( Engine & p_engine )
		: ShadowMap{ p_engine }
		, m_shadowMaps{ DoInitialisePoint( p_engine, Size{ 1024, 1024 } ) }
	{
	}

	ShadowMapPoint::~ShadowMapPoint()
	{
	}
	
	void ShadowMapPoint::Update( Camera const & p_camera
		, RenderQueueArray & p_queues )
	{
		if ( !m_passes.empty() )
		{
			const int32_t l_max = DoGetMaxPasses();
			m_sorted.clear();

			for ( auto & l_it : m_passes )
			{
				m_sorted.emplace( point::distance_squared( p_camera.GetParent()->GetDerivedPosition()
						, l_it.first->GetParent()->GetDerivedPosition() )
					, l_it.second );
			}

			auto l_it = m_sorted.begin();

			for ( auto i = 0; i < l_max && l_it != m_sorted.end(); ++i, ++l_it )
			{
				l_it->second->Update( p_queues, i );
			}
		}
	}

	void ShadowMapPoint::Render()
	{
		if ( !m_sorted.empty() )
		{
			auto l_it = m_sorted.begin();
			const int32_t l_max = DoGetMaxPasses();

			for ( int32_t i = 0; i < l_max && l_it != m_sorted.end(); ++i, ++l_it )
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

	int32_t ShadowMapPoint::DoGetMaxPasses()const
	{
		return int32_t( m_shadowMaps.size() );
	}

	Size ShadowMapPoint::DoGetSize()const
	{
		return m_shadowMaps[0].GetTexture()->GetDimensions();
	}

	void ShadowMapPoint::DoInitialise()
	{
		constexpr float l_component = std::numeric_limits< float >::max();
		m_frameBuffer->SetClearColour( l_component, l_component, l_component, l_component );
		m_colourAttachs.resize( DoGetMaxPasses() );
		auto l_it = m_colourAttachs.begin();

		for ( auto & l_map : m_shadowMaps )
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
		m_depthBuffer->Initialise( m_shadowMaps[0].GetTexture()->GetDimensions() );

		m_depthAttach = m_frameBuffer->CreateAttachment( m_depthBuffer );
		m_frameBuffer->Bind( FrameBufferTarget::eDraw );
		m_frameBuffer->Attach( AttachmentPoint::eDepth, m_depthAttach );
		m_frameBuffer->Unbind();
	}

	void ShadowMapPoint::DoCleanup()
	{
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

		for ( auto & l_map : m_shadowMaps )
		{
			l_map.Cleanup();
		}
	}

	ShadowMapPassSPtr ShadowMapPoint::DoCreatePass( Light & p_light )const
	{
		return std::make_shared< ShadowMapPassPoint >( *GetEngine(), p_light, *this );
	}

	void ShadowMapPoint::DoUpdateFlags( TextureChannels & p_textureFlags
		, ProgramFlags & p_programFlags
		, SceneFlags & p_sceneFlags )const
	{
		AddFlag( p_programFlags, ProgramFlag::eShadowMapPoint );
	}

	String ShadowMapPoint::DoGetPixelShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, SceneFlags const & p_sceneFlags )const
	{
		using namespace GLSL;
		GlslWriter l_writer = GetEngine()->GetRenderSystem()->CreateGlslWriter();

		// Fragment Intputs
		Ubo l_shadowMap{ l_writer, ShadowMapUbo };
		auto c3d_v3WordLightPosition( l_shadowMap.GetUniform< Vec3 >( WorldLightPosition ) );
		auto c3d_fFarPlane( l_shadowMap.GetUniform< Float >( FarPlane ) );
		l_shadowMap.End();

		auto vtx_texture = l_writer.DeclInput< Vec3 >( cuT( "vtx_texture" ) );
		auto vtx_position = l_writer.DeclInput< Vec3 >( cuT( "vtx_position" ) );
		auto c3d_mapOpacity( l_writer.DeclUniform< Sampler2D >( ShaderProgram::MapOpacity, CheckFlag( p_textureFlags, TextureChannel::eOpacity ) ) );

		// Fragment Outputs
		auto pxl_fFragColor = l_writer.DeclFragData< Float >( cuT( "pxl_fFragColor" ), 0u );

		auto l_main = [&]()
		{
			if ( CheckFlag( p_textureFlags, TextureChannel::eOpacity ) )
			{
				auto l_alpha = l_writer.DeclLocale( cuT( "l_alpha" ), texture( c3d_mapOpacity, vtx_texture.xy() ).r() );

				IF( l_writer, l_alpha < 0.2_f )
				{
					l_writer.Discard();
				}
				FI;
			}

			auto l_distance = l_writer.DeclLocale( cuT( "l_distance" ), length( vtx_position - c3d_v3WordLightPosition ) );
			pxl_fFragColor = l_distance / c3d_fFarPlane;
		};

		l_writer.ImplementFunction< void >( cuT( "main" ), l_main );
		return l_writer.Finalise();
	}
}
