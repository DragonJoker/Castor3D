#include "ShadowMapPassDirectional.hpp"

#include "Engine.hpp"

#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/Light/Light.hpp"
#include "Scene/Light/DirectionalLight.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureImage.hpp"
#include "Texture/TextureLayout.hpp"

#include <GlslSource.hpp>

#include <Graphics/Image.hpp>

using namespace Castor;

namespace Castor3D
{
	ShadowMapPassDirectional::ShadowMapPassDirectional( Engine & p_engine
		, Light & p_light
		, ShadowMap const & p_shadowMap )
		: ShadowMapPass{ p_engine, p_light, p_shadowMap }
	{
	}

	ShadowMapPassDirectional::~ShadowMapPassDirectional()
	{
	}

	bool ShadowMapPassDirectional::DoInitialise( Size const & p_size )
	{
		Viewport l_viewport{ *GetEngine() };
		real l_w = real( p_size.width() );
		real l_h = real( p_size.height() );
		l_viewport.SetOrtho( -l_w / 4, l_w / 4, l_h / 4, -l_h / 4, -512.0_r, 512.0_r );
		l_viewport.Update();
		m_camera = std::make_shared< Camera >( cuT( "ShadowMap_" ) + m_light.GetName()
			, *m_light.GetScene()
			, m_light.GetParent()
			, std::move( l_viewport ) );
		m_camera->Resize( p_size );

		m_renderQueue.Initialise( *m_light.GetScene(), *m_camera );
		return true;
	}

	void ShadowMapPassDirectional::DoCleanup()
	{
		m_camera->Detach();
		m_camera.reset();
	}

	void ShadowMapPassDirectional::DoUpdate( RenderQueueArray & p_queues )
	{
		m_light.Update( m_camera->GetParent()->GetDerivedPosition()
			, m_camera->GetViewport()
			, m_index );
		m_camera->Update();
		p_queues.push_back( m_renderQueue );
	}

	void ShadowMapPassDirectional::DoRender( uint32_t p_face )
	{
		if ( m_camera && m_initialised )
		{
			m_camera->Apply();
			m_projectionUniform->SetValue( m_camera->GetViewport().GetProjection() );
			m_viewUniform->SetValue( m_camera->GetView() );
			m_matrixUbo.Update();
			DoRenderNodes( m_renderQueue.GetRenderNodes(), *m_camera );
		}
	}
}
