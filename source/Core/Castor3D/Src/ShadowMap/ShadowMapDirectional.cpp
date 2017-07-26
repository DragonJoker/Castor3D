#include "ShadowMapDirectional.hpp"

#include "Engine.hpp"

#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "Scene/Light/Light.hpp"
#include "Shader/ShaderProgram.hpp"
#include "ShadowMap/ShadowMapPassDirectional.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureImage.hpp"
#include "Texture/TextureLayout.hpp"

#include <GlslSource.hpp>

#include <Graphics/Image.hpp>

using namespace Castor;

namespace Castor3D
{
	namespace
	{
		TextureUnit DoInitialiseDirectional( Engine & engine, Size const & p_size )
		{
			SamplerSPtr sampler;
			String const name = cuT( "ShadowMap_Directional" );

			if ( engine.GetSamplerCache().Has( name ) )
			{
				sampler = engine.GetSamplerCache().Find( name );
			}
			else
			{
				sampler = engine.GetSamplerCache().Add( name );
				sampler->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
				sampler->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
				sampler->SetWrappingMode( TextureUVW::eU, WrapMode::eClampToBorder );
				sampler->SetWrappingMode( TextureUVW::eV, WrapMode::eClampToBorder );
				sampler->SetWrappingMode( TextureUVW::eW, WrapMode::eClampToBorder );
				sampler->SetComparisonMode( ComparisonMode::eRefToTexture );
				sampler->SetComparisonFunc( ComparisonFunc::eLEqual );
			}

			auto texture = engine.GetRenderSystem()->CreateTexture(
				TextureType::eTwoDimensions,
				AccessType::eNone,
				AccessType::eRead | AccessType::eWrite,
				PixelFormat::eD32F, p_size );
			TextureUnit unit{ engine };
			unit.SetTexture( texture );
			unit.SetSampler( sampler );

			for ( auto & image : *texture )
			{
				image->InitialiseSource();
			}

			return unit;
		}
	}

	ShadowMapDirectional::ShadowMapDirectional( Engine & engine )
		: ShadowMap{ engine }
		, m_shadowMap{ DoInitialiseDirectional( engine, Size{ 4096, 4096 } ) }
	{
	}

	ShadowMapDirectional::~ShadowMapDirectional()
	{
	}

	void ShadowMapDirectional::Update( Camera const & p_camera
		, RenderQueueArray & p_queues )
	{
		if ( !m_passes.empty() )
		{
			for ( auto & it : m_passes )
			{
				it.second->Update( p_queues, 0u );
			}
		}
	}

	void ShadowMapDirectional::Render()
	{
		if ( !m_passes.empty() )
		{
			m_frameBuffer->Bind( FrameBufferTarget::eDraw );
			m_depthAttach->Attach( AttachmentPoint::eDepth );
			m_frameBuffer->Clear( BufferComponent::eDepth );
			m_passes.begin()->second->Render();
			m_depthAttach->Detach();
			m_frameBuffer->Unbind();
		}
	}

	int32_t ShadowMapDirectional::DoGetMaxPasses()const
	{
		return 1;
	}

	Size ShadowMapDirectional::DoGetSize()const
	{
		return m_shadowMap.GetTexture()->GetDimensions();
	}

	void ShadowMapDirectional::DoInitialise()
	{
		m_shadowMap.Initialise();
		m_frameBuffer->SetClearColour( Colour::from_predef( PredefinedColour::eOpaqueBlack ) );

		auto texture = m_shadowMap.GetTexture();
		m_depthAttach = m_frameBuffer->CreateAttachment( texture );
		m_depthAttach->SetTarget( texture->GetType() );
	}

	void ShadowMapDirectional::DoCleanup()
	{
		m_depthAttach.reset();
		m_shadowMap.Cleanup();
	}

	ShadowMapPassSPtr ShadowMapDirectional::DoCreatePass( Light & p_light )const
	{
		return std::make_shared< ShadowMapPassDirectional >( *GetEngine(), p_light, *this );
	}

	void ShadowMapDirectional::DoUpdateFlags( TextureChannels & textureFlags
		, ProgramFlags & programFlags
		, SceneFlags & sceneFlags )const
	{
		AddFlag( programFlags, ProgramFlag::eShadowMapDirectional );
	}

	GLSL::Shader ShadowMapDirectional::DoGetPixelShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ComparisonFunc alphaFunc )const
	{
		using namespace GLSL;
		GlslWriter writer = GetEngine()->GetRenderSystem()->CreateGlslWriter();

		// Fragment Intputs
		auto vtx_texture = writer.DeclInput< Vec3 >( cuT( "vtx_texture" ) );
		auto c3d_mapOpacity( writer.DeclUniform< Sampler2D >( ShaderProgram::MapOpacity, CheckFlag( textureFlags, TextureChannel::eOpacity ) ) );
		auto gl_FragCoord( writer.DeclBuiltin< Vec4 >( cuT( "gl_FragCoord" ) ) );

		// Fragment Outputs
		auto pxl_fFragDepth( writer.DeclFragData< Float >( cuT( "pxl_fFragDepth" ), 0 ) );

		writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			if ( CheckFlag( textureFlags, TextureChannel::eOpacity ) )
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
