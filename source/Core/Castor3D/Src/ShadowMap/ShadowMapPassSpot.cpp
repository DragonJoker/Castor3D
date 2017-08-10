#include "ShadowMapPassSpot.hpp"

using namespace castor;

namespace castor3d
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

	bool ShadowMapPassSpot::doInitialise( Size const & p_size )
	{
		Viewport viewport{ *getEngine() };
		m_camera = std::make_shared< Camera >( cuT( "ShadowMap_" ) + m_light.getName()
			, *m_light.getScene()
			, m_light.getParent()
			, std::move( viewport ) );
		m_camera->resize( p_size );

		m_renderQueue.initialise( *m_light.getScene(), *m_camera );
		return true;
	}

	void ShadowMapPassSpot::doCleanup()
	{
		m_camera->detach();
		m_camera.reset();
	}

	void ShadowMapPassSpot::doUpdate( RenderQueueArray & p_queues )
	{
		m_light.update( m_camera->getParent()->getDerivedPosition()
			, m_camera->getViewport()
			, m_index );
		m_camera->update();
		p_queues.push_back( m_renderQueue );
	}

	void ShadowMapPassSpot::doRender( uint32_t p_face )
	{
		if ( m_camera && m_initialised )
		{
			m_camera->apply();
			m_matrixUbo.update( m_camera->getView()
				, m_camera->getViewport().getProjection() );
			doRenderNodes( m_renderQueue.getRenderNodes(), *m_camera );
		}
	}
}
