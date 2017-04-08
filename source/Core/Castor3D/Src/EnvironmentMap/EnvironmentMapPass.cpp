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
			Viewport l_viewport{ *p_node.GetScene()->GetEngine() };
			return std::make_shared< Camera >( cuT( "EnvironmentMap_" ) + p_node.GetName()
				, *p_node.GetScene()
				, p_node.shared_from_this()
				, std::move( l_viewport ) );
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
			, false
			, true
			, &p_objectNode ) }
		, m_transparentPass{ std::make_unique< ForwardRenderTechniquePass >( cuT( "environment_transparent" )
			, *p_node->GetScene()
			, m_camera.get()
			, false
			, false
			, true
			, &p_objectNode ) }
	{
	}

	EnvironmentMapPass::~EnvironmentMapPass()
	{
	}

	bool EnvironmentMapPass::Initialise( Size const & p_size )
	{
		real const l_aspect = real( p_size.width() ) / p_size.height();
		real const l_near = 1.0_r;
		real const l_far = 1000.0_r;
		m_camera->GetViewport().SetPerspective( Angle::from_degrees( 90.0_r )
			, l_aspect
			, l_near
			, l_far );
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
		m_camera->GetParent()->SetPosition( p_node.GetDerivedPosition() );
		m_camera->Update();
		m_opaquePass->Update( p_queues );
		m_transparentPass->Update( p_queues );
	}

	void EnvironmentMapPass::Render()
	{
		auto & l_scene = *m_camera->GetScene();
		RenderInfo l_info;
		m_camera->Apply();
		m_opaquePass->Render( l_info, false );
		
		if ( l_scene.GetFog().GetType() == GLSL::FogType::eDisabled )
		{
			l_scene.RenderBackground( GetOwner()->GetSize(), *m_camera );
		}

		m_transparentPass->Render( l_info, false );
	}
}
