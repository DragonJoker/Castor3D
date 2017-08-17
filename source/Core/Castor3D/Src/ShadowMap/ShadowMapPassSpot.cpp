#include "ShadowMapPassSpot.hpp"

using namespace castor;

namespace castor3d
{
	ShadowMapPassSpot::ShadowMapPassSpot( Engine & engine
		, Scene & scene
		, ShadowMap const & shadowMap )
		: ShadowMapPass{ engine, scene, shadowMap }
	{
	}

	ShadowMapPassSpot::~ShadowMapPassSpot()
	{
	}

	void ShadowMapPassSpot::update( Camera const & camera
		, RenderQueueArray & queues
		, Light & light
		, uint32_t index )
	{
		light.update( light.getParent()->getDerivedPosition()
			, m_camera->getViewport()
			, index );
		m_camera->attachTo( light.getParent() );
		m_camera->update();
		queues.push_back( m_renderQueue );
	}

	void ShadowMapPassSpot::render( uint32_t index )
	{
		if ( m_camera && m_initialised )
		{
			m_camera->apply();
			m_matrixUbo.update( m_camera->getView()
				, m_camera->getViewport().getProjection() );
			doRenderNodes( m_renderQueue.getRenderNodes(), *m_camera );
		}
	}

	bool ShadowMapPassSpot::doInitialise( Size const & size )
	{
		Viewport viewport{ *getEngine() };
		m_camera = std::make_shared< Camera >( cuT( "ShadowMapSpot" )
			, m_scene
			, m_scene.getCameraRootNode()
			, std::move( viewport ) );
		m_camera->resize( size );

		m_renderQueue.initialise( m_scene, *m_camera );
		return true;
	}

	void ShadowMapPassSpot::doCleanup()
	{
		m_camera->detach();
		m_camera.reset();
	}

	void ShadowMapPassSpot::doUpdate( RenderQueueArray & queues )
	{
		queues.push_back( m_renderQueue );
	}
}
