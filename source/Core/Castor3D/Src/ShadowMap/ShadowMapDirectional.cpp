#include "ShadowMapDirectional.hpp"

#include "Engine.hpp"

#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/Light/Light.hpp"
#include "Scene/Light/DirectionalLight.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Shader/UniformBuffer.hpp"
#include "ShadowMap/ShadowMapPassDirectional.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureImage.hpp"
#include "Texture/TextureLayout.hpp"

#include <GlslSource.hpp>

#include <Graphics/Image.hpp>

using namespace Castor;

namespace Castor3D
{
	ShadowMapDirectional::ShadowMapDirectional( Engine & p_engine
		, TextureUnit && p_texture )
		: ShadowMap{ p_engine }
		, m_shadowMap{ std::move( p_texture ) }
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
			for ( auto & l_it : m_passes )
			{
				l_it.second->Update( p_queues, 0u );
			}
		}
	}

	void ShadowMapDirectional::Render()
	{
		if ( !m_passes.empty() )
		{
			m_frameBuffer->Bind( FrameBufferMode::eManual, FrameBufferTarget::eDraw );
			m_depthAttach->Attach( AttachmentPoint::eDepth );
			m_frameBuffer->Clear( BufferComponent::eDepth );
			m_passes.begin()->second->Render();
			m_depthAttach->Detach();
			m_frameBuffer->Unbind();
		}
	}

	void ShadowMapDirectional::Render( DirectionalLight const & p_light )
	{
		auto l_it = m_passes.find( &p_light.GetLight() );
		REQUIRE( l_it != m_passes.end() && "Light not found, call AddLight..." );
		m_frameBuffer->Bind( FrameBufferMode::eManual, FrameBufferTarget::eDraw );
		m_depthAttach->Attach( AttachmentPoint::eDepth );
		m_frameBuffer->Clear( BufferComponent::eDepth );
		l_it->second->Render();
		m_depthAttach->Detach();
		m_frameBuffer->Unbind();
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

		auto l_texture = m_shadowMap.GetTexture();
		m_depthAttach = m_frameBuffer->CreateAttachment( l_texture );
		m_depthAttach->SetTarget( l_texture->GetType() );
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

	void ShadowMapDirectional::DoUpdateFlags( TextureChannels & p_textureFlags
		, ProgramFlags & p_programFlags
		, SceneFlags & p_sceneFlags )const
	{
		AddFlag( p_programFlags, ProgramFlag::eShadowMapDirectional );
	}

	String ShadowMapDirectional::DoGetPixelShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, SceneFlags const & p_sceneFlags )const
	{
		using namespace GLSL;
		GlslWriter l_writer = GetEngine()->GetRenderSystem()->CreateGlslWriter();

		// Fragment Intputs
		auto vtx_texture = l_writer.GetInput< Vec3 >( cuT( "vtx_texture" ) );
		auto c3d_mapOpacity( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapOpacity, CheckFlag( p_textureFlags, TextureChannel::eOpacity ) ) );
		auto gl_FragCoord( l_writer.GetBuiltin< Vec4 >( cuT( "gl_FragCoord" ) ) );

		// Fragment Outputs
		auto pxl_fFragDepth( l_writer.GetFragData< Float >( cuT( "pxl_fFragDepth" ), 0 ) );

		l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			Float l_fAlpha;

			if ( CheckFlag( p_textureFlags, TextureChannel::eOpacity ) )
			{
				l_fAlpha = l_writer.GetLocale( cuT( "l_fAlpha" ), texture( c3d_mapOpacity, vtx_texture.xy() ).r() );

				IF( l_writer, l_fAlpha < 1.0_f )
				{
					l_writer.Discard();
				}
				FI;
			}
			else
			{
				pxl_fFragDepth = gl_FragCoord.z();
			}
		} );

		return l_writer.Finalise();
	}
}
