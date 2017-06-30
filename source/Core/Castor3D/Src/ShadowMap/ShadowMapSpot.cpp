#include "ShadowMapSpot.hpp"

#include "Engine.hpp"

#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "Scene/Light/Light.hpp"
#include "Shader/ShaderProgram.hpp"
#include "ShadowMap/ShadowMapPassSpot.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <GlslSource.hpp>

using namespace Castor;

namespace Castor3D
{
	namespace
	{
		TextureUnit DoInitialiseSpot( Engine & p_engine, Size const & p_size )
		{
			SamplerSPtr l_sampler;
			String const l_name = cuT( "ShadowMap_Spot" );

			if ( p_engine.GetSamplerCache().Has( l_name ) )
			{
				l_sampler = p_engine.GetSamplerCache().Find( l_name );
			}
			else
			{
				l_sampler = p_engine.GetSamplerCache().Add( l_name );
				l_sampler->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
				l_sampler->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
				l_sampler->SetWrappingMode( TextureUVW::eU, WrapMode::eClampToBorder );
				l_sampler->SetWrappingMode( TextureUVW::eV, WrapMode::eClampToBorder );
				l_sampler->SetWrappingMode( TextureUVW::eW, WrapMode::eClampToBorder );
				l_sampler->SetComparisonMode( ComparisonMode::eRefToTexture );
				l_sampler->SetComparisonFunc( ComparisonFunc::eLEqual );
			}

			auto l_texture = p_engine.GetRenderSystem()->CreateTexture(
				TextureType::eTwoDimensionsArray,
				AccessType::eNone,
				AccessType::eRead | AccessType::eWrite,
				PixelFormat::eD32F,
				Point3ui{ p_size.width(), p_size.height(), GLSL::SpotShadowMapCount } );
			TextureUnit l_unit{ p_engine };
			l_unit.SetTexture( l_texture );
			l_unit.SetSampler( l_sampler );

			for ( auto & l_image : *l_texture )
			{
				l_image->InitialiseSource();
			}

			return l_unit;
		}
	}

	ShadowMapSpot::ShadowMapSpot( Engine & p_engine )
		: ShadowMap{ p_engine }
		, m_shadowMap{ DoInitialiseSpot( p_engine, Size{ 1024, 1024 } ) }
	{
	}

	ShadowMapSpot::~ShadowMapSpot()
	{
	}
	
	void ShadowMapSpot::Update( Camera const & p_camera
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

			for ( int32_t i = 0; i < l_max && l_it != m_sorted.end(); ++i, ++l_it )
			{
				l_it->second->Update( p_queues, i );
			}
		}
	}

	void ShadowMapSpot::Render()
	{
		if ( !m_sorted.empty() )
		{
			m_frameBuffer->Bind( FrameBufferTarget::eDraw );
			auto l_it = m_sorted.begin();
			const int32_t l_max = DoGetMaxPasses();

			for ( int32_t i = 0; i < l_max && l_it != m_sorted.end(); ++i, ++l_it )
			{
				m_depthAttach[i]->Attach( AttachmentPoint::eDepth );
				m_frameBuffer->Clear( BufferComponent::eDepth );
				l_it->second->Render();
				m_depthAttach[i]->Detach();
			}

			m_frameBuffer->Unbind();
		}
	}

	int32_t ShadowMapSpot::DoGetMaxPasses()const
	{
		return int32_t( m_shadowMap.GetTexture()->GetLayersCount() );
	}

	Size ShadowMapSpot::DoGetSize()const
	{
		return m_shadowMap.GetTexture()->GetDimensions();
	}

	void ShadowMapSpot::DoInitialise()
	{
		m_shadowMap.Initialise();
		m_frameBuffer->SetClearColour( Colour::from_predef( PredefinedColour::eOpaqueBlack ) );

		auto l_texture = m_shadowMap.GetTexture();
		m_depthAttach.resize( DoGetMaxPasses() );
		int i = 0;

		for ( auto & l_attach : m_depthAttach )
		{
			l_attach = m_frameBuffer->CreateAttachment( l_texture );
			l_attach->SetTarget( l_texture->GetType() );
			l_attach->SetLayer( i++ );
		}
	}

	void ShadowMapSpot::DoCleanup()
	{
		for ( auto & l_attach : m_depthAttach )
		{
			l_attach.reset();
		}

		m_shadowMap.Cleanup();
	}

	ShadowMapPassSPtr ShadowMapSpot::DoCreatePass( Light & p_light )const
	{
		return std::make_shared< ShadowMapPassSpot >( *GetEngine(), p_light, *this );
	}

	void ShadowMapSpot::DoUpdateFlags( TextureChannels & p_textureFlags
		, ProgramFlags & p_programFlags
		, SceneFlags & p_sceneFlags )const
	{
		AddFlag( p_programFlags, ProgramFlag::eShadowMapSpot );
	}

	GLSL::Shader ShadowMapSpot::DoGetPixelShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, SceneFlags const & p_sceneFlags
		, ComparisonFunc p_alphaFunc )const
	{
		using namespace GLSL;
		GlslWriter l_writer = GetEngine()->GetRenderSystem()->CreateGlslWriter();

		// Fragment Intputs
		auto vtx_texture = l_writer.DeclInput< Vec3 >( cuT( "vtx_texture" ) );
		auto c3d_mapOpacity( l_writer.DeclUniform< Sampler2D >( ShaderProgram::MapOpacity, CheckFlag( p_textureFlags, TextureChannel::eOpacity ) ) );
		auto gl_FragCoord( l_writer.DeclBuiltin< Vec4 >( cuT( "gl_FragCoord" ) ) );

		// Fragment Outputs
		auto pxl_fFragDepth( l_writer.DeclFragData< Float >( cuT( "pxl_fFragDepth" ), 0 ) );

		l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
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

			pxl_fFragDepth = gl_FragCoord.z();
		} );

		return l_writer.Finalise();
	}
}
