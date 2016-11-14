#include "ShadowMapPassDirectional.hpp"

#include "Engine.hpp"

#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/Light/Light.hpp"
#include "Scene/Light/DirectionalLight.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureImage.hpp"
#include "Texture/TextureLayout.hpp"

#include <GlslSource.hpp>

#include <Graphics/Image.hpp>

using namespace Castor;

namespace Castor3D
{
	ShadowMapPassDirectional::ShadowMapPassDirectional( Engine & p_engine, Scene & p_scene, Light & p_light, TextureUnit & p_shadowMap, uint32_t p_index )
		: ShadowMapPass{ p_engine, p_scene, p_light, p_shadowMap, p_index }
	{

	}

	ShadowMapPassDirectional::~ShadowMapPassDirectional()
	{
	}

	ShadowMapPassSPtr ShadowMapPassDirectional::Create( Engine & p_engine, Scene & p_scene, Light & p_light, TextureUnit & p_shadowMap, uint32_t p_index )
	{
		return std::make_shared< ShadowMapPassDirectional >( p_engine, p_scene, p_light, p_shadowMap, p_index );
	}

	bool ShadowMapPassDirectional::DoInitialise( Size const & p_size )
	{
		Viewport l_viewport{ *GetEngine() };
		//real l_w = real( p_size.width() );
		//real l_h = real( p_size.height() );
		real l_w = 200.0_r;
		real l_h = 200.0_r;
		l_viewport.SetOrtho( -l_w / 2, l_w / 2, l_h / 2, -l_h / 2, -1024.0_r, 10.0_r );
		l_viewport.Update();
		m_node = std::make_shared< SceneNode >( cuT( "ShadowMap" ), m_scene );
		m_camera = std::make_shared< Camera >( cuT( "ShadowMap_" ) + m_light.GetName()
											   , m_scene
											   , m_node
											   , std::move( l_viewport ) );
		m_camera->AttachTo( m_node );
		m_camera->Resize( p_size );
		m_light.GetDirectionalLight()->SetViewport( m_camera->GetViewport() );

		auto l_texture = m_shadowMap.GetTexture();
		m_depthAttach = m_frameBuffer->CreateAttachment( l_texture );
		bool l_return{ false };

		if ( m_frameBuffer->Bind( FrameBufferMode::eConfig ) )
		{
			m_frameBuffer->Attach( AttachmentPoint::eDepth, 0, m_depthAttach, l_texture->GetType() );
			l_return = m_frameBuffer->IsComplete();
			m_frameBuffer->Unbind();
		}

		m_frameBuffer->SetClearColour( Colour::from_predef( PredefinedColour::eOpaqueBlack ) );
		m_renderQueue.Initialise( m_scene, *m_camera );
		return l_return;
	}

	void ShadowMapPassDirectional::DoCleanup()
	{
		m_camera->Detach();
		m_camera.reset();
		m_node.reset();

		m_frameBuffer->Bind( FrameBufferMode::eConfig );
		m_frameBuffer->DetachAll();
		m_frameBuffer->Unbind();
		m_depthAttach.reset();
	}

	void ShadowMapPassDirectional::DoUpdate()
	{
		Point3r l_front = point::get_normalised( m_light.GetDirectionalLight()->GetDirection() );
		l_front[2] = -l_front[2];
		l_front = -l_front;
		Point3r l_right{ 1, 0, 0 };
		Point3r l_up{ l_right ^ l_front };
		l_right = l_up ^ l_front;
		Quaternion l_orientation;
		l_orientation.from_axes( l_right, l_up, l_front );
		m_node->SetOrientation( l_orientation );
		m_node->Update();
		m_light.Update( m_camera->GetParent()->GetDerivedPosition() );
		m_camera->Update();
		m_renderQueue.Update();
		auto l_mtx = m_camera->GetViewport().GetProjection() * m_camera->GetView();
	}

	void ShadowMapPassDirectional::DoRender()
	{
		if ( m_camera && m_frameBuffer->Bind( FrameBufferMode::eAutomatic, FrameBufferTarget::eDraw ) )
		{
			m_frameBuffer->Clear();
			auto & l_nodes = m_renderQueue.GetRenderNodes();
			m_camera->Apply();
			DoRenderOpaqueNodes( l_nodes, *m_camera );
			DoRenderTransparentNodes( l_nodes, *m_camera );
			m_frameBuffer->Unbind();
		}
	}

	void ShadowMapPassDirectional::DoUpdateProgram( ShaderProgram & p_program )
	{
	}

	String ShadowMapPassDirectional::DoGetGeometryShaderSource( uint16_t p_textureFlags, uint16_t p_programFlags, uint8_t p_sceneFlags )const
	{
		return String{};
	}

	String ShadowMapPassDirectional::DoGetOpaquePixelShaderSource( uint16_t p_textureFlags, uint16_t p_programFlags, uint8_t p_sceneFlags )const
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
