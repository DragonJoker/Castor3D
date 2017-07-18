#include "RenderNode.hpp"

#include "Engine.hpp"
#include "Material/Pass.hpp"
#include "EnvironmentMap/EnvironmentMap.hpp"
#include "Render/RenderPipeline.hpp"
#include "Scene/BillboardList.hpp"
#include "Scene/Geometry.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Skybox.hpp"
#include "Scene/Animation/AnimatedMesh.hpp"
#include "Scene/Animation/AnimatedSkeleton.hpp"
#include "Scene/Animation/Mesh/MeshAnimationInstance.hpp"
#include "Scene/Animation/Mesh/MeshAnimationInstanceSubmesh.hpp"
#include "Scene/Animation/Skeleton/SkeletonAnimationInstance.hpp"
#include "Shader/BillboardUbo.hpp"
#include "Shader/MatrixUbo.hpp"
#include "Shader/ModelMatrixUbo.hpp"
#include "Shader/ModelUbo.hpp"
#include "Shader/SceneUbo.hpp"
#include "Shader/PushUniform.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Shader/UniformBuffer.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"
#include "Texture/TextureUnit.hpp"

using namespace Castor;

namespace Castor3D
{
	inline uint32_t DoFillShaderDepthMaps( RenderPipeline & p_pipeline
		, DepthMapArray & p_depthMaps )
	{
		uint32_t index = p_pipeline.GetTexturesCount() + Pass::MinTextureIndex;

		if ( !p_depthMaps.empty() )
		{
			auto layer = 0u;

			if ( GetShadowType( p_pipeline.GetFlags().m_sceneFlags ) != GLSL::ShadowType::eNone )
			{
				for ( auto & depthMap : p_depthMaps )
				{
					switch ( depthMap.get().GetType() )
					{
					case TextureType::eTwoDimensions:
						depthMap.get().SetIndex( index );
						p_pipeline.GetDirectionalShadowMapsVariable().SetValue( index++ );
						break;

					case TextureType::eTwoDimensionsArray:
						depthMap.get().SetIndex( index );
						p_pipeline.GetSpotShadowMapsVariable().SetValue( index++ );
						break;

					case TextureType::eCube:
						depthMap.get().SetIndex( index );
						p_pipeline.GetPointShadowMapsVariable().SetValue( index++, layer++ );
						break;

					case TextureType::eCubeArray:
						depthMap.get().SetIndex( index );
						p_pipeline.GetPointShadowMapsVariable().SetValue( index++ );
						break;
					}
				}
			}
		}

		return index;
	}

	inline uint32_t DoFillShaderPbrMaps( RenderPipeline & p_pipeline
		, Scene & p_scene
		, SceneNode & p_sceneNode
		, uint32_t p_index )
	{
		p_scene.GetIbl( p_sceneNode ).GetIrradiance().GetTexture()->Bind( p_index );
		p_scene.GetIbl( p_sceneNode ).GetIrradiance().GetSampler()->Bind( p_index );
		p_pipeline.GetIrradianceMapVariable().SetValue( p_index++ );
		p_scene.GetIbl( p_sceneNode ).GetPrefilteredEnvironment().GetTexture()->Bind( p_index );
		p_scene.GetIbl( p_sceneNode ).GetPrefilteredEnvironment().GetSampler()->Bind( p_index );
		p_pipeline.GetPrefilteredMapVariable().SetValue( p_index++ );
		p_scene.GetSkybox().GetIbl().GetPrefilteredBrdf().GetTexture()->Bind( p_index );
		p_scene.GetSkybox().GetIbl().GetPrefilteredBrdf().GetSampler()->Bind( p_index );
		p_pipeline.GetBrdfMapVariable().SetValue( p_index++ );
		return p_index;
	}

	inline void DoBindPass( SceneNode & p_sceneNode
		, PassRenderNode & p_node
		, Scene & p_scene
		, RenderPipeline & p_pipeline
		, DepthMapArray & p_depthMaps
		, ModelUbo & p_model
		, EnvironmentMap *& p_envMap )
	{
		auto index = DoFillShaderDepthMaps( p_pipeline, p_depthMaps );

		p_node.m_pass.BindTextures();

		if ( ( CheckFlag( p_pipeline.GetFlags().m_programFlags, ProgramFlag::ePbrMetallicRoughness )
				|| CheckFlag( p_pipeline.GetFlags().m_programFlags, ProgramFlag::ePbrSpecularGlossiness ) )
			&& CheckFlag( p_pipeline.GetFlags().m_programFlags, ProgramFlag::eLighting ) )
		{
			index = DoFillShaderPbrMaps( p_pipeline
				, p_scene
				, p_sceneNode
				, index );
		}

		for ( auto pair : p_node.m_textures )
		{
			auto texture = pair.first;
			auto & variable = pair.second;

			if ( texture )
			{
				variable.get().SetValue( texture );
			}
		}

		for ( auto & depthMap : p_depthMaps )
		{
			depthMap.get().Bind();
		}

		if ( p_node.m_pass.HasEnvironmentMapping() )
		{
			p_envMap = &p_scene.GetEnvironmentMap( p_sceneNode );

			if ( CheckFlag( p_pipeline.GetFlags().m_programFlags, ProgramFlag::eLighting ) )
			{
				p_pipeline.GetEnvironmentMapVariable().SetValue( index );
				p_envMap->GetTexture().SetIndex( index );
				p_envMap->GetTexture().Bind();
			}
			else
			{
				p_model.SetEnvMapIndex( p_envMap->GetIndex() );
			}
		}
		else
		{
			p_model.SetEnvMapIndex( 0 );
			p_envMap = nullptr;
		}
	}

	inline void DoUnbindPass( SceneNode & p_sceneNode
		, PassRenderNode & p_node
		, Scene & p_scene
		, RenderPipeline & p_pipeline
		, DepthMapArray const & p_depthMaps
		, EnvironmentMap * p_envMap )
	{
		if ( p_envMap )
		{
			p_envMap->GetTexture().Unbind();
		}

		for ( auto & depthMap : p_depthMaps )
		{
			depthMap.get().Unbind();
		}

		p_node.m_pass.UnbindTextures();
	}

	inline void DoBindPassOpacityMap( PassRenderNode & p_node
		, Pass & p_pass )
	{
		auto unit = p_pass.GetTextureUnit( TextureChannel::eOpacity );

		if ( unit )
		{
			p_node.m_textures.find( unit->GetIndex() )->second.get().SetValue( 0 );
			unit->GetTexture()->Bind( 0u );
			unit->GetSampler()->Bind( 0u );
		}
	}

	inline void DoUnbindPassOpacityMap( PassRenderNode & p_node
		, Pass & p_pass )
	{
		auto unit = p_pass.GetTextureUnit( TextureChannel::eOpacity );

		if ( unit )
		{
			unit->GetSampler()->Unbind( 0u );
			unit->GetTexture()->Unbind( 0u );
		}
	}

	template< typename DataType, typename InstanceType >
	inline void DoRenderObjectNode( ObjectRenderNode< DataType, InstanceType > & p_node )
	{
		auto & model = p_node.m_sceneNode.GetDerivedTransformationMatrix();
		p_node.m_modelMatrixUbo.Update( model );
		p_node.m_data.Draw( p_node.m_buffers );
	}

	inline void DoRenderNodeNoPass( StaticRenderNode & p_node )
	{
		DoRenderObjectNode( p_node );
	}

	inline void DoRenderNodeNoPass( BillboardRenderNode & p_node )
	{
		p_node.m_billboardUbo.Update( p_node.m_instance.GetDimensions() );
		DoRenderObjectNode( p_node );
	}

	inline void DoRenderNodeNoPass( MorphingRenderNode & p_node )
	{
		if ( p_node.m_mesh.IsPlayingAnimation() )
		{
			auto submesh = p_node.m_mesh.GetPlayingAnimation().GetAnimationSubmesh( p_node.m_data.GetId() );

			if ( submesh )
			{
				p_node.m_morphingUbo.Update( submesh->GetCurrentFactor() );
			}
			else
			{
				p_node.m_morphingUbo.Update( 1.0f );
			}
		}
		else
		{
			p_node.m_morphingUbo.Update( 1.0f );
		}

		DoRenderObjectNode( p_node );
	}

	inline void DoRenderNodeNoPass( SkinningRenderNode & p_node )
	{
		if ( !CheckFlag( p_node.m_pipeline.GetFlags().m_programFlags, ProgramFlag::eInstantiation ) )
		{
			p_node.m_skinningUbo.Update( p_node.m_skeleton );
		}

		DoRenderObjectNode( p_node );
	}

	template< typename NodeType >
	inline void DoRenderNode( NodeType & p_node )
	{
		p_node.m_modelUbo.Update( p_node.m_instance.IsShadowReceiver()
			, p_node.m_passNode.m_pass.GetId() );
		DoRenderNodeNoPass( p_node );
	}
}
