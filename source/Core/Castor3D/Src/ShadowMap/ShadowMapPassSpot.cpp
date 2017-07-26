#include "ShadowMapPassSpot.hpp"

using namespace Castor;

namespace Castor3D
{
	ShadowMapPassSpot::ShadowMapPassSpot( Engine & engine
		, Light & p_light
		, ShadowMap const & p_shadowMap )
		: ShadowMapPass{ engine, p_light, p_shadowMap }
	{
	}

	ShadowMapPassSpot::~ShadowMapPassSpot()
	{
	}

	bool ShadowMapPassSpot::DoInitialise( Size const & p_size )
	{
		Viewport viewport{ *GetEngine() };
		m_camera = std::make_shared< Camera >( cuT( "ShadowMap_" ) + m_light.GetName()
			, *m_light.GetScene()
			, m_light.GetParent()
			, std::move( viewport ) );
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
			m_matrixUbo.Update( m_camera->GetView()
				, m_camera->GetViewport().GetProjection() );
			DoRenderNodes( m_renderQueue.GetRenderNodes(), *m_camera );
		}
	}
}
