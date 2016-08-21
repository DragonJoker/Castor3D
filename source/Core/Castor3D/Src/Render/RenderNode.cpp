#include "RenderNode.hpp"

#include "Engine.hpp"
#include "Material/Pass.hpp"
#include "Render/Pipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Mesh/Submesh.hpp"
#include "Scene/BillboardList.hpp"
#include "Scene/Camera.hpp"
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
		void DoRender( Scene const & p_scene, Camera const & p_camera, Pipeline & p_pipeline, NodeType & p_node )
		{
			p_pipeline.SetModelMatrix( p_node.m_sceneNode.GetDerivedTransformationMatrix() );
			p_node.BindPass( p_scene, p_camera, p_pipeline, 0 );
			p_node.m_data.Draw( p_node.m_buffers );
			p_node.UnbindPass( p_scene );
		}

		template< typename NodeType >
		void DoUnbind( Scene const & p_scene, NodeType & p_node )
		{
			p_node.m_scene.m_node.m_pass.EndRender();
			p_node.m_scene.m_node.m_program.Unbind();

			if ( p_scene.GetEngine()->GetPerObjectLighting() )
			{
				p_scene.GetLightCache().UnbindLights( p_node.m_scene.m_node.m_program, p_node.m_scene.m_sceneUbo );
			}
		}
	}

	SceneRenderNode::SceneRenderNode( RenderNode const & p_node, FrameVariableBuffer & p_sceneUbo, Point3rFrameVariable & p_cameraPos )
		: m_node{ p_node.m_pass
				  , p_node.m_program
				  , p_node.m_matrixUbo
				  , p_node.m_passUbo
				  , p_node.m_ambient
				  , p_node.m_diffuse
				  , p_node.m_specular
				  , p_node.m_emissive
				  , p_node.m_shininess
				  , p_node.m_opacity
				  , p_node.m_textures }
		, m_sceneUbo{ p_sceneUbo }
		, m_cameraPos{ p_cameraPos }
	{
	}

	ObjectRenderNodeBase::ObjectRenderNodeBase( SceneRenderNode const & p_scene, GeometryBuffers & p_buffers, SceneNode & p_sceneNode )
		: m_scene{ p_scene }
		, m_buffers{ p_buffers }
		, m_sceneNode{ p_sceneNode }
	{
	}

	template<>
	void SubmeshRenderNode::Render( Scene const & p_scene, Camera const & p_camera, Pipeline & p_pipeline )
	{
		DoRender( p_scene, p_camera, p_pipeline, *this );
	}

	template<>
	void SubmeshRenderNode::UnbindPass( Scene const & p_scene )const
	{
		DoUnbind( p_scene, *this );
	}

	template<>
	void BillboardListRenderNode::Render( Scene const & p_scene, Camera const & p_camera, Pipeline & p_pipeline )
	{
		DoRender( p_scene, p_camera, p_pipeline, *this );
	}

	template<>
	void BillboardListRenderNode::UnbindPass( Scene const & p_scene )const
	{
		DoUnbind( p_scene, *this );
	}

	void StaticGeometryRenderNode::BindPass( Scene const & p_scene, Camera const & p_camera, Pipeline & p_pipeline, uint64_t p_excludedMtxFlags )
	{
		if ( p_scene.GetEngine()->GetPerObjectLighting() )
		{
			p_scene.GetLightCache().BindLights( m_scene.m_node.m_program, m_scene.m_sceneUbo );
			p_camera.FillShader( m_scene.m_sceneUbo );
		}

		p_pipeline.ApplyMatrices( m_scene.m_node.m_matrixUbo, ~p_excludedMtxFlags );
		m_scene.m_node.m_pass.FillShaderVariables( m_scene.m_node );
		m_scene.m_node.m_program.Bind();
		m_scene.m_node.m_pass.Render();
	}

	void AnimatedGeometryRenderNode::BindPass( Scene const & p_scene, Camera const & p_camera, Pipeline & p_pipeline, uint64_t p_excludedMtxFlags )
	{
		if ( p_scene.GetEngine()->GetPerObjectLighting() )
		{
			p_scene.GetLightCache().BindLights( m_scene.m_node.m_program, m_scene.m_sceneUbo );
			p_camera.FillShader( m_scene.m_sceneUbo );
		}

		p_pipeline.ApplyMatrices( m_scene.m_node.m_matrixUbo, ~p_excludedMtxFlags );

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

		m_scene.m_node.m_pass.FillShaderVariables( m_scene.m_node );
		m_scene.m_node.m_program.Bind();
		m_scene.m_node.m_pass.Render();
	}

	void BillboardRenderNode::BindPass( Scene const & p_scene, Camera const & p_camera, Pipeline & p_pipeline, uint64_t p_excludedMtxFlags )
	{
		if ( p_scene.GetEngine()->GetPerObjectLighting() )
		{
			p_scene.GetLightCache().BindLights( m_scene.m_node.m_program, m_scene.m_sceneUbo );
			p_camera.FillShader( m_scene.m_sceneUbo );
		}

		p_pipeline.ApplyMatrices( m_scene.m_node.m_matrixUbo, ~p_excludedMtxFlags );
		auto const & l_dimensions = m_data.GetDimensions();
		m_dimensions.SetValue( Point2i( l_dimensions.width(), l_dimensions.height() ) );
		m_scene.m_node.m_pass.FillShaderVariables( m_scene.m_node );
		m_scene.m_node.m_program.Bind();
		m_scene.m_node.m_pass.Render();
	}
}
