#include "DeferredShadowMapDirectional.hpp"

#include <Engine.hpp>

#include <FrameBuffer/FrameBuffer.hpp>
#include <FrameBuffer/TextureAttachment.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Scene/Light/Light.hpp>
#include <Scene/Light/DirectionalLight.hpp>
#include <Shader/ShaderProgram.hpp>
#include <Shader/UniformBuffer.hpp>
#include <ShadowMap/ShadowMapPassDirectional.hpp>
#include <Texture/Sampler.hpp>
#include <Texture/TextureImage.hpp>
#include <Texture/TextureLayout.hpp>

#include <GlslSource.hpp>

#include <Graphics/Image.hpp>
#include <Miscellaneous/BlockTracker.hpp>

using namespace Castor;
using namespace Castor3D;

namespace Castor3D
{
	namespace
	{
		TextureUnit DoInitialiseDirectional( Engine & p_engine, Size const & p_size )
		{
			auto sampler = p_engine.GetSamplerCache().Add( cuT( "ShadowMap_Directional" ) );
			sampler->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
			sampler->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
			sampler->SetWrappingMode( TextureUVW::eU, WrapMode::eClampToBorder );
			sampler->SetWrappingMode( TextureUVW::eV, WrapMode::eClampToBorder );
			sampler->SetWrappingMode( TextureUVW::eW, WrapMode::eClampToBorder );
			sampler->SetComparisonMode( ComparisonMode::eRefToTexture );
			sampler->SetComparisonFunc( ComparisonFunc::eLEqual );

			auto texture = p_engine.GetRenderSystem()->CreateTexture(
				TextureType::eTwoDimensions,
				AccessType::eNone,
				AccessType::eRead | AccessType::eWrite,
				PixelFormat::eD32F, p_size );
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

	DeferredShadowMapDirectional::DeferredShadowMapDirectional( Engine & p_engine )
		: ShadowMap{ p_engine }
		, m_shadowMap{ DoInitialiseDirectional( p_engine, Size{ 4096, 4096 } ) }
	{
	}

	DeferredShadowMapDirectional::~DeferredShadowMapDirectional()
	{
	}

	void DeferredShadowMapDirectional::Update( Camera const & p_camera
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

	void DeferredShadowMapDirectional::Render( DirectionalLight const & p_light )
	{
		auto it = m_passes.find( &p_light.GetLight() );
		REQUIRE( it != m_passes.end() && "Light not found, call AddLight..." );
		m_frameBuffer->Bind( FrameBufferTarget::eDraw );
		m_frameBuffer->Clear( BufferComponent::eDepth );
		it->second->Render();
		m_frameBuffer->Unbind();
	}

	int32_t DeferredShadowMapDirectional::DoGetMaxPasses()const
	{
		return 1;
	}

	Size DeferredShadowMapDirectional::DoGetSize()const
	{
		return m_shadowMap.GetTexture()->GetDimensions();
	}

	void DeferredShadowMapDirectional::DoInitialise()
	{
		m_shadowMap.Initialise();
		m_frameBuffer->SetClearColour( Colour::from_predef( PredefinedColour::eOpaqueBlack ) );

		auto texture = m_shadowMap.GetTexture();
		m_depthAttach = m_frameBuffer->CreateAttachment( texture );
		m_frameBuffer->Bind();
		m_frameBuffer->Attach( AttachmentPoint::eDepth, m_depthAttach, m_shadowMap.GetTexture()->GetType() );
		ENSURE( m_frameBuffer->IsComplete() );
		m_frameBuffer->Unbind();
	}

	void DeferredShadowMapDirectional::DoCleanup()
	{
		m_depthAttach.reset();
		m_shadowMap.Cleanup();
	}

	ShadowMapPassSPtr DeferredShadowMapDirectional::DoCreatePass( Light & p_light )const
	{
		return std::make_shared< ShadowMapPassDirectional >( *GetEngine(), p_light, *this );
	}

	void DeferredShadowMapDirectional::DoUpdateFlags( TextureChannels & p_textureFlags
		, ProgramFlags & p_programFlags
		, SceneFlags & p_sceneFlags )const
	{
		AddFlag( p_programFlags, ProgramFlag::eShadowMapDirectional );
	}

	GLSL::Shader DeferredShadowMapDirectional::DoGetPixelShaderSource( TextureChannels const & p_textureFlags
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
