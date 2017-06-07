#include "ShadowMapDirectional.hpp"

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
			auto l_sampler = p_engine.GetSamplerCache().Add( cuT( "ShadowMap_Directional" ) );
			l_sampler->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
			l_sampler->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
			l_sampler->SetWrappingMode( TextureUVW::eU, WrapMode::eClampToBorder );
			l_sampler->SetWrappingMode( TextureUVW::eV, WrapMode::eClampToBorder );
			l_sampler->SetWrappingMode( TextureUVW::eW, WrapMode::eClampToBorder );
			l_sampler->SetComparisonMode( ComparisonMode::eRefToTexture );
			l_sampler->SetComparisonFunc( ComparisonFunc::eLEqual );

			auto l_texture = p_engine.GetRenderSystem()->CreateTexture(
				TextureType::eTwoDimensions,
				AccessType::eNone,
				AccessType::eRead | AccessType::eWrite,
				PixelFormat::eD32F, p_size );
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
			for ( auto & l_it : m_passes )
			{
				l_it.second->Update( p_queues, 0u );
			}
		}
	}

	void DeferredShadowMapDirectional::Render( DirectionalLight const & p_light )
	{
		auto l_it = m_passes.find( &p_light.GetLight() );
		REQUIRE( l_it != m_passes.end() && "Light not found, call AddLight..." );
		m_frameBuffer->Bind( FrameBufferTarget::eDraw );
		m_frameBuffer->Clear( BufferComponent::eDepth );
		l_it->second->Render();
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

		auto l_texture = m_shadowMap.GetTexture();
		m_depthAttach = m_frameBuffer->CreateAttachment( l_texture );
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

	String DeferredShadowMapDirectional::DoGetPixelShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, SceneFlags const & p_sceneFlags )const
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
