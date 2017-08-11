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
		CameraSPtr doCreateCamera( SceneNode & p_node )
		{
			Viewport viewport{ *p_node.getScene()->getEngine() };
			return std::make_shared< Camera >( cuT( "EnvironmentMap_" ) + p_node.getName()
				, *p_node.getScene()
				, p_node.shared_from_this()
				, std::move( viewport ) );
		}
	}

	EnvironmentMapPass::EnvironmentMapPass( EnvironmentMap & p_reflectionMap
		, SceneNodeSPtr p_node
		, SceneNode const & p_objectNode )
		: OwnedBy< EnvironmentMap >{ p_reflectionMap }
		, m_node{ p_node }
		, m_camera{ doCreateCamera( *p_node ) }
		, m_opaquePass{ std::make_unique< ForwardRenderTechniquePass >( cuT( "environment_opaque" )
			, *p_node->getScene()
			, m_camera.get()
			, true
			, &p_objectNode
			, SsaoConfig{} ) }
		, m_transparentPass{ std::make_unique< ForwardRenderTechniquePass >( cuT( "environment_transparent" )
			, *p_node->getScene()
			, m_camera.get()
			, true
			, true
			, &p_objectNode
			, SsaoConfig{} ) }
	{
	}

	EnvironmentMapPass::~EnvironmentMapPass()
	{
	}

	bool EnvironmentMapPass::initialise( Size const & p_size )
	{
		real const aspect = real( p_size.getWidth() ) / p_size.getHeight();
		real const near = 1.0_r;
		real const far = 1000.0_r;
		m_camera->getViewport().setPerspective( Angle::fromDegrees( 90.0_r )
			, aspect
			, near
			, far );
		m_camera->resize( p_size );
		m_camera->getViewport().initialise();
		m_opaquePass->initialise( p_size );
		m_transparentPass->initialise( p_size );
		return true;
	}

	void EnvironmentMapPass::cleanup()
	{
		m_opaquePass->cleanup();
		m_transparentPass->cleanup();
		m_camera->getViewport().cleanup();
	}

	void EnvironmentMapPass::update( SceneNode const & p_node, RenderQueueArray & p_queues )
	{
		auto position = p_node.getDerivedPosition();
		m_camera->getParent()->setPosition( position );
		m_camera->getParent()->update();
		m_camera->update();
		m_opaquePass->update( p_queues );
		m_transparentPass->update( p_queues );
	}

	void EnvironmentMapPass::render()
	{
		auto & scene = *m_camera->getScene();
		RenderInfo info;
		m_camera->apply();
		ShadowMapLightTypeArray shadowMaps;
		m_opaquePass->render( info, shadowMaps );
		scene.renderBackground( getOwner()->getSize(), *m_camera );
		m_transparentPass->render( info, shadowMaps );
	}
}
