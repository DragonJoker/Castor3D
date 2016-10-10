#include "RenderNode.hpp"

#include "Engine.hpp"
#include "Material/Pass.hpp"
#include "Render/Pipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Mesh/Submesh.hpp"
#include "Scene/BillboardList.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Geometry.hpp"
#include "Scene/Scene.hpp"
#include "Scene/SceneNode.hpp"
#include "Scene/Animation/AnimatedMesh.hpp"
#include "Scene/Animation/AnimatedSkeleton.hpp"
#include "Scene/Animation/Mesh/MeshAnimationInstance.hpp"
#include "Scene/Animation/Mesh/MeshAnimationInstanceSubmesh.hpp"
#include "Scene/Animation/Skeleton/SkeletonAnimationInstance.hpp"
#include "Shader/FrameVariableBuffer.hpp"
#include "Shader/OneFrameVariable.hpp"
#include "Shader/ShaderProgram.hpp"

using namespace Castor;

namespace Castor3D
{
	namespace
	{
		template< typename NodeType >
		void DoRender( NodeType & p_node )
		{
			p_node.m_pass.m_pipeline.SetModelMatrix( p_node.m_sceneNode.GetDerivedTransformationMatrix() );
			p_node.BindPass( 0 );
			p_node.m_data.Draw( p_node.m_buffers );
			p_node.UnbindPass();
		}

		void DoUnbind( GeometryRenderNode const & p_node )
		{
			p_node.m_pass.m_pass.EndRender();
			p_node.m_pass.m_pipeline.GetProgram().UnbindUbos();

			if ( p_node.m_geometry.GetScene()->GetEngine()->GetPerObjectLighting() )
			{
				p_node.m_geometry.GetScene()->GetLightCache().UnbindLights();
			}
		}

		void DoUnbind( BillboardListRenderNode const & p_node )
		{
			p_node.m_pass.m_pass.EndRender();
			p_node.m_pass.m_pipeline.GetProgram().UnbindUbos();

			if ( p_node.m_data.GetScene()->GetEngine()->GetPerObjectLighting() )
			{
				p_node.m_data.GetScene()->GetLightCache().UnbindLights();
			}
		}
	}

	SceneRenderNode::SceneRenderNode( FrameVariableBuffer & p_sceneUbo, Point3rFrameVariable & p_cameraPos )
		: m_sceneUbo{ p_sceneUbo }
		, m_cameraPos{ p_cameraPos }
	{
	}

	ObjectRenderNodeBase::ObjectRenderNodeBase( SceneRenderNode const & p_scene, PassRenderNode const & p_pass, GeometryBuffers & p_buffers, SceneNode & p_sceneNode )
		: m_scene{ p_scene }
		, m_pass{ p_pass.m_pass
				  , p_pass.m_pipeline
				  , p_pass.m_matrixUbo
				  , p_pass.m_passUbo
				  , p_pass.m_ambient
				  , p_pass.m_diffuse
				  , p_pass.m_specular
				  , p_pass.m_emissive
				  , p_pass.m_shininess
				  , p_pass.m_opacity
				  , p_pass.m_textures }
		, m_buffers{ p_buffers }
		, m_sceneNode{ p_sceneNode }
	{
	}

	template<>
	void SubmeshRenderNode::Render()
	{
		REQUIRE( false && "Did you forget to implement an appropriate SubmeshRenderNode derivated class?" );
	}

	template<>
	void SubmeshRenderNode::UnbindPass()const
	{
		REQUIRE( false && "Did you forget to implement an appropriate SubmeshRenderNode derivated class?" );
	}

	template<>
	void BillboardListRenderNode::Render()
	{
		DoRender( *this );
	}

	template<>
	void BillboardListRenderNode::UnbindPass()const
	{
		DoUnbind( *this );
	}

	void GeometryRenderNode::Render()
	{
		DoRender( *this );
	}

	void GeometryRenderNode::UnbindPass()const
	{
		DoUnbind( *this );
	}

	void StaticGeometryRenderNode::BindPass( uint64_t p_excludedMtxFlags )
	{
		if ( m_geometry.GetScene()->GetEngine()->GetPerObjectLighting() )
		{
			m_geometry.GetScene()->GetLightCache().BindLights();
		}

		m_pass.m_pipeline.ApplyMatrices( m_pass.m_matrixUbo, ~p_excludedMtxFlags );
		m_pass.m_pass.FillShaderVariables( m_pass );
		m_pass.m_pipeline.GetProgram().BindUbos();
		m_pass.m_pass.Render();
	}

	void AnimatedGeometryRenderNode::BindPass( uint64_t p_excludedMtxFlags )
	{
		if ( m_geometry.GetScene()->GetEngine()->GetPerObjectLighting() )
		{
			m_geometry.GetScene()->GetLightCache().BindLights();
		}

		m_pass.m_pipeline.ApplyMatrices( m_pass.m_matrixUbo, ~p_excludedMtxFlags );

		if ( m_skeleton )
		{
			Matrix4x4rFrameVariableSPtr l_variable;
			m_animationUbo.GetVariable( ShaderProgram::Bones, l_variable );

			if ( l_variable )
			{
				m_skeleton->FillShader( *l_variable );
			}
		}

		if ( m_mesh )
		{
			OneFloatFrameVariableSPtr l_variable;
			m_animationUbo.GetVariable( ShaderProgram::Time, l_variable );

			if ( l_variable )
			{
				if ( m_mesh->IsPlayingAnimation() )
				{
					auto l_submesh = m_mesh->GetPlayingAnimation().GetAnimationSubmesh( m_data.GetId() );

					if ( l_submesh )
					{
						l_submesh->FillShader( *l_variable );
					}
				}
				else
				{
					l_variable->SetValue( 1.0f );
				}
			}
		}

		m_pass.m_pass.FillShaderVariables( m_pass );
		m_pass.m_pipeline.GetProgram().BindUbos();
		m_pass.m_pass.Render();
	}

	void BillboardRenderNode::BindPass( uint64_t p_excludedMtxFlags )
	{
		if ( m_data.GetScene()->GetEngine()->GetPerObjectLighting() )
		{
			m_data.GetScene()->GetLightCache().BindLights();
		}

		m_pass.m_pipeline.ApplyMatrices( m_pass.m_matrixUbo, ~p_excludedMtxFlags );
		auto const & l_dimensions = m_data.GetDimensions();
		m_dimensions.SetValue( Point2i( l_dimensions.width(), l_dimensions.height() ) );
		m_pass.m_pass.FillShaderVariables( m_pass );
		m_pass.m_pipeline.GetProgram().BindUbos();
		m_pass.m_pass.Render();
	}
}
