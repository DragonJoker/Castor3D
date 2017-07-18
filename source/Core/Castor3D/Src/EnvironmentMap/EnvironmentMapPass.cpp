#include "EnvironmentMapPass.hpp"

#include "EnvironmentMap/EnvironmentMap.hpp"
#include "Render/Viewport.hpp"
#include "Scene/Camera.hpp"
#include "Technique/ForwardRenderTechniquePass.hpp"

using namespace Castor;

namespace Castor3D
{
	namespace
	{
		CameraSPtr DoCreateCamera( SceneNode & p_node )
		{
			Viewport viewport{ *p_node.GetScene()->GetEngine() };
			return std::make_shared< Camera >( cuT( "EnvironmentMap_" ) + p_node.GetName()
				, *p_node.GetScene()
				, p_node.shared_from_this()
				, std::move( viewport ) );
		}
	}

	EnvironmentMapPass::EnvironmentMapPass( EnvironmentMap & p_reflectionMap
		, SceneNodeSPtr p_node
		, SceneNode const & p_objectNode )
		: OwnedBy< EnvironmentMap >{ p_reflectionMap }
		, m_node{ p_node }
		, m_camera{ DoCreateCamera( *p_node ) }
		, m_opaquePass{ std::make_unique< ForwardRenderTechniquePass >( cuT( "environment_opaque" )
			, *p_node->GetScene()
			, m_camera.get()
			, true
			, &p_objectNode
			, SsaoConfig{} ) }
		, m_transparentPass{ std::make_unique< ForwardRenderTechniquePass >( cuT( "environment_transparent" )
			, *p_node->GetScene()
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

	bool EnvironmentMapPass::Initialise( Size const & p_size )
	{
		real const aspect = real( p_size.width() ) / p_size.height();
		real const near = 1.0_r;
		real const far = 1000.0_r;
		m_camera->GetViewport().SetPerspective( Angle::from_degrees( 90.0_r )
			, aspect
			, near
			, far );
		m_camera->Resize( p_size );
		m_camera->GetViewport().Initialise();
		m_opaquePass->Initialise( p_size );
		m_transparentPass->Initialise( p_size );
		return true;
	}

	void EnvironmentMapPass::Cleanup()
	{
		m_opaquePass->Cleanup();
		m_transparentPass->Cleanup();
		m_camera->GetViewport().Cleanup();
	}

	void EnvironmentMapPass::Update( SceneNode const & p_node, RenderQueueArray & p_queues )
	{
		auto position = p_node.GetDerivedPosition();
		m_camera->GetParent()->SetPosition( position );
		m_camera->GetParent()->Update();
		m_camera->Update();
		m_opaquePass->Update( p_queues );
		m_transparentPass->Update( p_queues );
	}

	void EnvironmentMapPass::Render()
	{
		auto & scene = *m_camera->GetScene();
		RenderInfo info;
		m_camera->Apply();
		m_opaquePass->Render( info, false );
		scene.RenderBackground( GetOwner()->GetSize(), *m_camera );
		m_transparentPass->Render( info, false );
	}
}
