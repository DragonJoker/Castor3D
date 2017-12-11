#include "EnvironmentMapPass.hpp"

#include "EnvironmentMap/EnvironmentMap.hpp"
#include "Render/Viewport.hpp"
#include "Scene/Camera.hpp"
#include "Technique/ForwardRenderTechniquePass.hpp"

using namespace castor;

namespace castor3d
{
	namespace
	{
		CameraSPtr doCreateCamera( SceneNode & node )
		{
			Viewport viewport{ *node.getScene()->getEngine() };
			return std::make_shared< Camera >( cuT( "EnvironmentMap_" ) + node.getName()
				, *node.getScene()
				, node.shared_from_this()
				, std::move( viewport ) );
		}
	}

	EnvironmentMapPass::EnvironmentMapPass( EnvironmentMap & reflectionMap
		, SceneNodeSPtr node
		, SceneNode const & objectNode )
		: OwnedBy< EnvironmentMap >{ reflectionMap }
		, m_node{ node }
		, m_camera{ doCreateCamera( *node ) }
		, m_opaquePass{ std::make_unique< ForwardRenderTechniquePass >( cuT( "environment_opaque" )
			, *node->getScene()
			, m_camera.get()
			, true
			, &objectNode
			, SsaoConfig{} ) }
		, m_transparentPass{ std::make_unique< ForwardRenderTechniquePass >( cuT( "environment_transparent" )
			, *node->getScene()
			, m_camera.get()
			, true
			, true
			, &objectNode
			, SsaoConfig{} ) }
	{
	}

	EnvironmentMapPass::~EnvironmentMapPass()
	{
	}

	bool EnvironmentMapPass::initialise( Size const & size )
	{
		real const aspect = real( size.getWidth() ) / size.getHeight();
		real const near = 0.1_r;
		real const far = 1000.0_r;
		m_camera->getViewport().setPerspective( 90.0_degrees
			, aspect
			, near
			, far );
		m_camera->resize( size );
		m_camera->getViewport().initialise();
		m_opaquePass->initialise( size );
		m_transparentPass->initialise( size );
		return true;
	}

	void EnvironmentMapPass::cleanup()
	{
		m_opaquePass->cleanup();
		m_transparentPass->cleanup();
		m_camera->getViewport().cleanup();
	}

	void EnvironmentMapPass::update( SceneNode const & node, RenderQueueArray & queues )
	{
		auto position = node.getDerivedPosition();
		m_camera->getParent()->setPosition( position );
		m_camera->getParent()->update();
		m_camera->update();
		m_opaquePass->update( queues );
		m_transparentPass->update( queues );
	}

	void EnvironmentMapPass::render()
	{
		auto & scene = *m_camera->getScene();
		RenderInfo info;
		m_camera->apply();
		ShadowMapLightTypeArray shadowMaps;
		m_opaquePass->render( info, shadowMaps, Point2r{} );
		scene.renderBackground( getOwner()->getSize(), *m_camera );
		m_transparentPass->render( info, shadowMaps, Point2r{} );
	}
}
