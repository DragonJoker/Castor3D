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
	ShadowMapPassSpot::ShadowMapPassSpot( Engine & p_engine
		, Light & p_light
		, ShadowMap const & p_shadowMap )
		: ShadowMapPass{ p_engine, p_light, p_shadowMap }
	{
	}

	ShadowMapPassSpot::~ShadowMapPassSpot()
	{
	}

	bool ShadowMapPassSpot::DoInitialise( Size const & p_size )
	{
		Viewport l_viewport{ *GetEngine() };
		m_camera = std::make_shared< Camera >( cuT( "ShadowMap_" ) + m_light.GetName()
			, *m_light.GetScene()
			, m_light.GetParent()
			, std::move( l_viewport ) );
		m_camera->Resize( p_size );

		m_renderQueue.Initialise( *m_light.GetScene(), *m_camera );
		return true;
	}

	void ShadowMapPassSpot::DoCleanup()
	{
		m_camera->Detach();
		m_camera.reset();
	}

	void ShadowMapPassSpot::DoUpdate( RenderQueueArray & p_queues )
	{
		m_light.Update( m_camera->GetParent()->GetDerivedPosition()
			, m_camera->GetViewport()
			, m_index );
		m_camera->Update();
		p_queues.push_back( m_renderQueue );
	}

	void ShadowMapPassSpot::DoRender( uint32_t p_face )
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
