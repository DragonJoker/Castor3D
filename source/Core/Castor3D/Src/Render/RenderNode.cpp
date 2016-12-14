#include "RenderNode.hpp"

#include "Engine.hpp"
#include "Material/Pass.hpp"
#include "Render/RenderPipeline.hpp"
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
#include "Shader/UniformBuffer.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"
#include "Texture/TextureUnit.hpp"

using namespace Castor;

namespace Castor3D
{
	namespace
	{
		inline void DoBindDepthMaps( DepthMapArray const & p_depthMaps )
		{
			for ( auto & l_depthMap : p_depthMaps )
			{
				l_depthMap.get().Bind();
			}
		}

		inline void DoUnbindDepthMaps( DepthMapArray const & p_depthMaps )
		{
			for ( auto & l_depthMap : p_depthMaps )
			{
				l_depthMap.get().Unbind();
			}
		}

		template< typename NodeType >
		inline void DoRender( NodeType & p_node, DepthMapArray const & p_depthMaps )
		{
			p_node.m_pass.m_pipeline.SetModelMatrix( p_node.m_sceneNode.GetDerivedTransformationMatrix() );
			p_node.BindPass( p_depthMaps, 0 );
			p_node.m_data.Draw( p_node.m_buffers );
			p_node.UnbindPass( p_depthMaps );
		}

		inline void DoUnbind( GeometryRenderNode const & p_node, DepthMapArray const & p_depthMaps )
		{
			DoUnbindDepthMaps( p_depthMaps );
			p_node.m_pass.m_pass.UnbindTextures();

			if ( CheckFlag( p_node.m_pass.m_pipeline.GetFlags().m_programFlags, ProgramFlag::eLighting ) )
			{
				p_node.m_instance.GetScene()->GetLightCache().UnbindLights();
			}
		}

		inline void DoUnbind( BillboardListRenderNode const & p_node, DepthMapArray const & p_depthMaps )
		{
			DoUnbindDepthMaps( p_depthMaps );
			p_node.m_pass.m_pass.UnbindTextures();

			if ( CheckFlag( p_node.m_pass.m_pipeline.GetFlags().m_programFlags, ProgramFlag::eLighting ) )
			{
				p_node.m_data.GetParentScene().GetLightCache().UnbindLights();
			}
		}
	}

	SceneRenderNode::SceneRenderNode( UniformBuffer & p_sceneUbo, Uniform3r & p_cameraPos )
		: m_sceneUbo{ p_sceneUbo }
		, m_cameraPos{ p_cameraPos }
	{
	}

	ObjectRenderNodeBase::ObjectRenderNodeBase( SceneRenderNode const & p_scene
		, PassRenderNode const & p_pass
		, GeometryBuffers & p_buffers
		, SceneNode & p_sceneNode
		, Uniform1i & p_shadowReceiver )
		: m_scene{ p_scene }
		, m_pass{ p_pass.m_pass
				  , p_pass.m_pipeline
				  , p_pass.m_matrixUbo
				  , p_pass.m_ambient
				  , p_pass.m_diffuse
				  , p_pass.m_specular
				  , p_pass.m_emissive
				  , p_pass.m_shininess
				  , p_pass.m_opacity
				  , p_pass.m_textures }
		, m_buffers{ p_buffers }
		, m_sceneNode{ p_sceneNode }
		, m_shadowReceiver{ p_shadowReceiver }
	{
	}

	template<>
	void SubmeshRenderNode::Render( DepthMapArray const & p_depthMaps )
	{
		REQUIRE( false && "Did you forget to implement an appropriate SubmeshRenderNode derivated class?" );
	}

	template<>
	void SubmeshRenderNode::UnbindPass( DepthMapArray const & p_depthMaps )const
	{
		REQUIRE( false && "Did you forget to implement an appropriate SubmeshRenderNode derivated class?" );
	}

	template<>
	void BillboardListRenderNode::Render( DepthMapArray const & p_depthMaps )
	{
		DoRender( *this, p_depthMaps );
	}

	template<>
	void BillboardListRenderNode::UnbindPass( DepthMapArray const & p_depthMaps )const
	{
		DoUnbind( *this, p_depthMaps );
	}

	void GeometryRenderNode::Render( DepthMapArray const & p_depthMaps )
	{
		DoRender( *this, p_depthMaps );
	}

	void GeometryRenderNode::UnbindPass( DepthMapArray const & p_depthMaps )const
	{
		DoUnbind( *this, p_depthMaps );
	}

	void StaticGeometryRenderNode::BindPass( DepthMapArray const & p_depthMaps, uint64_t p_excludedMtxFlags )
	{
		if ( CheckFlag( m_pass.m_pipeline.GetFlags().m_programFlags, ProgramFlag::eLighting ) )
		{
			m_instance.GetScene()->GetLightCache().BindLights();
		}

		m_shadowReceiver.SetValue( m_instance.IsShadowReceiver() ? 1 : 0 );
		m_pass.m_pipeline.ApplyMatrices( m_pass.m_matrixUbo, ~p_excludedMtxFlags );
		m_pass.m_pass.UpdateRenderNode( m_pass );
		m_pass.m_pass.BindTextures();
		DoBindDepthMaps( p_depthMaps );
	}

	void AnimatedGeometryRenderNode::BindPass( DepthMapArray const & p_depthMaps, uint64_t p_excludedMtxFlags )
	{
		if ( CheckFlag( m_pass.m_pipeline.GetFlags().m_programFlags, ProgramFlag::eLighting ) )
		{
			m_instance.GetScene()->GetLightCache().BindLights();
		}

		m_shadowReceiver.SetValue( m_instance.IsShadowReceiver() ? 1 : 0 );
		m_pass.m_pipeline.ApplyMatrices( m_pass.m_matrixUbo, ~p_excludedMtxFlags );

		if ( m_skeleton )
		{
			auto l_variable = m_animationUbo.GetUniform< UniformType::eMat4x4f >( ShaderProgram::Bones );

			if ( l_variable )
			{
				m_skeleton->FillShader( *l_variable );
			}
		}

		if ( m_mesh )
		{
			auto l_variable = m_animationUbo.GetUniform< UniformType::eFloat >( ShaderProgram::Time );

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

		m_pass.m_pass.UpdateRenderNode( m_pass );
		m_pass.m_pass.BindTextures();
		DoBindDepthMaps( p_depthMaps );
	}

	void BillboardRenderNode::BindPass( DepthMapArray const & p_depthMaps, uint64_t p_excludedMtxFlags )
	{
		if ( CheckFlag( m_pass.m_pipeline.GetFlags().m_programFlags, ProgramFlag::eLighting ) )
		{
			m_data.GetParentScene().GetLightCache().BindLights();
		}

		m_shadowReceiver.SetValue( m_data.IsShadowReceiver() ? 1 : 0 );
		m_pass.m_pipeline.ApplyMatrices( m_pass.m_matrixUbo, ~p_excludedMtxFlags );
		auto const & l_dimensions = m_data.GetDimensions();
		m_dimensions.SetValue( Point2i( l_dimensions.width(), l_dimensions.height() ) );
		m_pass.m_pass.UpdateRenderNode( m_pass );
		m_pass.m_pass.BindTextures();
		DoBindDepthMaps( p_depthMaps );
	}
}
