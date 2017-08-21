#include "ShadowMapPassDirectional.hpp"

#include "Shader/ShaderProgram.hpp"
#include "Texture/TextureImage.hpp"
#include "Render/RenderPipeline.hpp"

#include <Graphics/Image.hpp>

using namespace castor;

namespace castor3d
{
	ShadowMapPassDirectional::ShadowMapPassDirectional( Engine & engine
		, Scene & scene
		, ShadowMap const & shadowMap )
		: ShadowMapPass{ engine, scene, shadowMap }
	{
	}

	ShadowMapPassDirectional::~ShadowMapPassDirectional()
	{
	}

	void ShadowMapPassDirectional::update( Camera const & camera
		, RenderQueueArray & queues
		, Light & light
		, uint32_t index )
	{
		light.update( light.getParent()->getDerivedPosition()
			, m_camera->getViewport()
			, index );
		m_camera->attachTo( light.getParent() );
		m_camera->update();
		doUpdate( queues );
	}

	void ShadowMapPassDirectional::render( uint32_t index )
	{
		if ( m_camera && m_initialised )
		{
			m_camera->apply();
			m_matrixUbo.update( m_camera->getView()
				, m_camera->getViewport().getProjection() );
			doRenderNodes( m_renderQueue.getRenderNodes(), *m_camera );
		}
	}

	bool ShadowMapPassDirectional::doInitialise( Size const & size )
	{
		Viewport viewport{ *getEngine() };
		real w = real( size.getWidth() );
		real h = real( size.getHeight() );
		viewport.setOrtho( -w / 2, w / 2, h / 2, -h / 2, -5120.0_r, 5120.0_r );
		viewport.update();
		m_camera = std::make_shared< Camera >( cuT( "ShadowMapDirectional" )
			, m_scene
			, m_scene.getCameraRootNode()
			, std::move( viewport ) );
		m_camera->resize( size );

		m_renderQueue.initialise( m_scene, *m_camera );
		return true;
	}

	void ShadowMapPassDirectional::doCleanup()
	{
		m_camera->detach();
		m_camera.reset();
	}

	void ShadowMapPassDirectional::doUpdate( RenderQueueArray & queues )
	{
		queues.push_back( m_renderQueue );
	}
}
