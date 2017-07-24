#include "DeferredShadowMapSpot.hpp"

#include <Engine.hpp>

#include <FrameBuffer/FrameBuffer.hpp>
#include <FrameBuffer/TextureAttachment.hpp>
#include <Render/RenderSystem.hpp>
#include <Scene/Light/Light.hpp>
#include <Scene/Light/SpotLight.hpp>
#include <Shader/ShaderProgram.hpp>
#include <ShadowMap/ShadowMapPassSpot.hpp>
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
		TextureUnit DoInitialiseSpot( Engine & engine, Size const & p_size )
		{
			auto sampler = engine.GetSamplerCache().Add( cuT( "ShadowMap_Spot" ) );
			sampler->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
			sampler->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
			sampler->SetWrappingMode( TextureUVW::eU, WrapMode::eClampToBorder );
			sampler->SetWrappingMode( TextureUVW::eV, WrapMode::eClampToBorder );
			sampler->SetWrappingMode( TextureUVW::eW, WrapMode::eClampToBorder );
			sampler->SetComparisonMode( ComparisonMode::eRefToTexture );
			sampler->SetComparisonFunc( ComparisonFunc::eLEqual );

			auto texture = engine.GetRenderSystem()->CreateTexture( TextureType::eTwoDimensions
				, AccessType::eNone
				, AccessType::eRead | AccessType::eWrite
				, PixelFormat::eD32F
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

		void DoSavePic( TextureLayout & p_texture
			, String const & p_name )
		{
			auto data = p_texture.Lock( AccessType::eRead );

			if ( data )
			{
				auto buffer = PxBuffer< PixelFormat::eA8R8G8B8 >{ p_texture.GetDimensions()
					, data
					, p_texture.GetPixelFormat() };

				for ( auto & pixel : buffer )
				{
					auto value = uint8_t( 255.0f - ( 255.0f - *pixel.begin() ) * 25.0f );

					for ( auto & component : pixel )
					{
						component = value;
					}
				}

				Image::BinaryWriter()( Image{ cuT( "tmp" ), buffer }
					, Engine::GetEngineDirectory() / p_name );
				p_texture.Unlock( false );
			}
		}
	}

	DeferredShadowMapSpot::DeferredShadowMapSpot( Engine & engine )
		: ShadowMap{ engine }
		, m_shadowMap{ DoInitialiseSpot( engine, Size{ 1024, 1024 } ) }
	{
	}

	DeferredShadowMapSpot::~DeferredShadowMapSpot()
	{
	}
	
	void DeferredShadowMapSpot::Update( Camera const & p_camera
		, RenderQueueArray & p_queues )
	{
		for ( auto & pass : m_passes )
		{
			pass.second->Update( p_queues, 0 );
		}
	}

	void DeferredShadowMapSpot::Render( SpotLight const & p_light )
	{
		auto it = m_passes.find( &p_light.GetLight() );
		REQUIRE( it != m_passes.end() && "Light not found, call AddLight..." );
		m_frameBuffer->Bind( FrameBufferTarget::eDraw );
		m_frameBuffer->Clear( BufferComponent::eDepth );
		it->second->Render();
		m_frameBuffer->Unbind();
	}

	int32_t DeferredShadowMapSpot::DoGetMaxPasses()const
	{
		return 1;
	}

	Size DeferredShadowMapSpot::DoGetSize()const
	{
		return m_shadowMap.GetTexture()->GetDimensions();
	}

	void DeferredShadowMapSpot::DoInitialise()
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

	void DeferredShadowMapSpot::DoCleanup()
	{
		m_depthAttach.reset();
		m_shadowMap.Cleanup();
	}

	ShadowMapPassSPtr DeferredShadowMapSpot::DoCreatePass( Light & p_light )const
	{
		return std::make_shared< ShadowMapPassSpot >( *GetEngine(), p_light, *this );
	}

	void DeferredShadowMapSpot::DoUpdateFlags( TextureChannels & textureFlags
		, ProgramFlags & programFlags
		, SceneFlags & sceneFlags )const
	{
		AddFlag( programFlags, ProgramFlag::eShadowMapSpot );
	}

	GLSL::Shader DeferredShadowMapSpot::DoGetPixelShaderSource( TextureChannels const & textureFlags
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
