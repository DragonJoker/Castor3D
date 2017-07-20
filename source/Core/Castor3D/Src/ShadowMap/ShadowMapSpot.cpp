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
			SamplerSPtr sampler;
			String const name = cuT( "ShadowMap_Spot" );

			if ( p_engine.GetSamplerCache().Has( name ) )
			{
				sampler = p_engine.GetSamplerCache().Find( name );
			}
			else
			{
				sampler = p_engine.GetSamplerCache().Add( name );
				sampler->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
				sampler->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
				sampler->SetWrappingMode( TextureUVW::eU, WrapMode::eClampToBorder );
				sampler->SetWrappingMode( TextureUVW::eV, WrapMode::eClampToBorder );
				sampler->SetWrappingMode( TextureUVW::eW, WrapMode::eClampToBorder );
				sampler->SetComparisonMode( ComparisonMode::eRefToTexture );
				sampler->SetComparisonFunc( ComparisonFunc::eLEqual );
			}

			auto texture = p_engine.GetRenderSystem()->CreateTexture(
				TextureType::eTwoDimensionsArray,
				AccessType::eNone,
				AccessType::eRead | AccessType::eWrite,
				PixelFormat::eD32F,
				Point3ui{ p_size.width(), p_size.height(), GLSL::SpotShadowMapCount } );
			TextureUnit unit{ p_engine };
			unit.SetTexture( texture );
			unit.SetSampler( sampler );

			for ( auto & image : *texture )
			{
				image->InitialiseSource();
			}

			return unit;
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
			const int32_t max = DoGetMaxPasses();
			m_sorted.clear();

			for ( auto & it : m_passes )
			{
				m_sorted.emplace( point::distance_squared( p_camera.GetParent()->GetDerivedPosition()
					, it.first->GetParent()->GetDerivedPosition() )
					, it.second );
			}

			auto it = m_sorted.begin();

			for ( int32_t i = 0; i < max && it != m_sorted.end(); ++i, ++it )
			{
				it->second->Update( p_queues, i );
			}
		}
	}

	void ShadowMapSpot::Render()
	{
		if ( !m_sorted.empty() )
		{
			m_frameBuffer->Bind( FrameBufferTarget::eDraw );
			auto it = m_sorted.begin();
			const int32_t max = DoGetMaxPasses();

			for ( int32_t i = 0; i < max && it != m_sorted.end(); ++i, ++it )
			{
				m_depthAttach[i]->Attach( AttachmentPoint::eDepth );
				m_frameBuffer->Clear( BufferComponent::eDepth );
				it->second->Render();
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

		auto texture = m_shadowMap.GetTexture();
		m_depthAttach.resize( DoGetMaxPasses() );
		int i = 0;

		for ( auto & attach : m_depthAttach )
		{
			attach = m_frameBuffer->CreateAttachment( texture );
			attach->SetTarget( texture->GetType() );
			attach->SetLayer( i++ );
		}
	}

	void ShadowMapSpot::DoCleanup()
	{
		for ( auto & attach : m_depthAttach )
		{
			attach.reset();
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
		GlslWriter writer = GetEngine()->GetRenderSystem()->CreateGlslWriter();

		// Fragment Intputs
		auto vtx_texture = writer.DeclInput< Vec3 >( cuT( "vtx_texture" ) );
		auto c3d_mapOpacity( writer.DeclUniform< Sampler2D >( ShaderProgram::MapOpacity, CheckFlag( p_textureFlags, TextureChannel::eOpacity ) ) );
		auto gl_FragCoord( writer.DeclBuiltin< Vec4 >( cuT( "gl_FragCoord" ) ) );

		// Fragment Outputs
		auto pxl_fFragDepth( writer.DeclFragData< Float >( cuT( "pxl_fFragDepth" ), 0 ) );

		writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			if ( CheckFlag( p_textureFlags, TextureChannel::eOpacity ) )
			{
				auto alpha = writer.DeclLocale( cuT( "alpha" ), texture( c3d_mapOpacity, vtx_texture.xy() ).r() );

				IF( writer, alpha < 0.2_f )
				{
					writer.Discard();
				}
				FI;
			}

			pxl_fFragDepth = gl_FragCoord.z();
		} );

		return writer.Finalise();
	}
}
