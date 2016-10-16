#include "ShadowMapPassSpot.hpp"

#include "Engine.hpp"

#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/Light/Light.hpp"
#include "Scene/Light/SpotLight.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureImage.hpp"
#include "Texture/TextureLayout.hpp"

#include <GlslSource.hpp>

#include <Graphics/Image.hpp>

using namespace Castor;

namespace Castor3D
{
	ShadowMapPassSpot::ShadowMapPassSpot( Engine & p_engine, Scene & p_scene, Light & p_light )
		: ShadowMapPass{ p_engine, p_scene, p_light }
	{

	}

	ShadowMapPassSpot::~ShadowMapPassSpot()
	{
	}

	ShadowMapPassSPtr ShadowMapPassSpot::Create( Engine & p_engine, Scene & p_scene, Light & p_light )
	{
		return std::make_shared< ShadowMapPassSpot >( p_engine, p_scene, p_light );
	}

	bool ShadowMapPassSpot::DoInitialise( Size const & p_size )
	{
		Viewport l_viewport{ *GetEngine() };
		m_camera = std::make_shared< Camera >( cuT( "ShadowMap_" ) + m_light.GetName()
												, m_scene
												, m_light.GetParent()
												, std::move( l_viewport ) );
		m_camera->Resize( p_size );
		m_light.GetSpotLight()->SetViewport( m_camera->GetViewport() );

		auto l_sampler = GetEngine()->GetSamplerCache().Add( m_light.GetName() + cuT( "_SpotShadowMap" ) );
		l_sampler->SetInterpolationMode( InterpolationFilter::Min, InterpolationMode::Linear );
		l_sampler->SetInterpolationMode( InterpolationFilter::Mag, InterpolationMode::Linear );
		l_sampler->SetWrappingMode( TextureUVW::U, WrapMode::ClampToEdge );
		l_sampler->SetWrappingMode( TextureUVW::V, WrapMode::ClampToEdge );
		l_sampler->SetWrappingMode( TextureUVW::W, WrapMode::ClampToEdge );
		l_sampler->SetComparisonMode( ComparisonMode::RefToTexture );
		l_sampler->SetComparisonFunc( ComparisonFunc::GEqual );
		auto l_texture = GetEngine()->GetRenderSystem()->CreateTexture( TextureType::TwoDimensions, AccessType::None, AccessType::ReadWrite, PixelFormat::D32F, p_size );
		m_shadowMap.SetTexture( l_texture );
		m_shadowMap.SetSampler( l_sampler );

		l_texture->GetImage().InitialiseSource();
		auto l_return = m_shadowMap.Initialise();

		m_depthAttach = m_frameBuffer->CreateAttachment( l_texture );

		if ( l_return && m_frameBuffer->Bind( FrameBufferMode::Config ) )
		{
			m_frameBuffer->Attach( AttachmentPoint::Depth, 0, m_depthAttach, l_texture->GetType() );
			l_return = m_frameBuffer->IsComplete();
			m_frameBuffer->Unbind();
		}

		m_frameBuffer->SetClearColour( Colour::from_predef( Colour::Predefined::OpaqueBlack ) );
		return l_return;
	}

	void ShadowMapPassSpot::DoCleanup()
	{
		m_camera->Detach();
		m_camera.reset();

		m_frameBuffer->Bind( FrameBufferMode::Config );
		m_frameBuffer->DetachAll();
		m_frameBuffer->Unbind();
		m_depthAttach.reset();

		m_shadowMap.Cleanup();
	}
	
	void ShadowMapPassSpot::DoUpdate()
	{
		m_light.Update( m_camera->GetParent()->GetDerivedPosition() );
		m_camera->Update();
		m_renderQueue.Prepare( *m_camera, m_scene );
	}

	void ShadowMapPassSpot::DoRender()
	{
		if ( m_camera && m_frameBuffer->Bind( FrameBufferMode::Automatic, FrameBufferTarget::Draw ) )
		{
			m_frameBuffer->Clear();
			auto & l_nodes = m_renderQueue.GetRenderNodes( *m_camera, m_scene );
			m_camera->Apply();
			DoRenderOpaqueNodes( l_nodes, *m_camera );
			DoRenderTransparentNodes( l_nodes, *m_camera );
			m_frameBuffer->Unbind();
		}
	}

	void ShadowMapPassSpot::DoUpdateProgram( ShaderProgram & p_program )
	{
	}

	String ShadowMapPassSpot::DoGetOpaquePixelShaderSource( uint16_t p_textureFlags, uint16_t p_programFlags, uint8_t p_sceneFlags )const
	{
		using namespace GLSL;
		GlslWriter l_writer = m_renderSystem.CreateGlslWriter();

		// Fragment Intputs
		auto gl_FragCoord( l_writer.GetBuiltin< Vec4 >( cuT( "gl_FragCoord" ) ) );

		// Fragment Outputs
		auto pxl_fFragDepth( l_writer.GetFragData< Float >( cuT( "pxl_fFragDepth" ), 0 ) );

		l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			pxl_fFragDepth = gl_FragCoord.z();
		} );

		return l_writer.Finalise();
	}
}
