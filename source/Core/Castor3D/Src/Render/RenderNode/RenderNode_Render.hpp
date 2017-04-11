#include "RenderNode.hpp"

#include "Engine.hpp"
#include "Material/Pass.hpp"
#include "EnvironmentMap/EnvironmentMap.hpp"
#include "Render/RenderPipeline.hpp"
#include "Scene/BillboardList.hpp"
#include "Scene/Geometry.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Animation/AnimatedMesh.hpp"
#include "Scene/Animation/AnimatedSkeleton.hpp"
#include "Scene/Animation/Mesh/MeshAnimationInstance.hpp"
#include "Scene/Animation/Mesh/MeshAnimationInstanceSubmesh.hpp"
#include "Scene/Animation/Skeleton/SkeletonAnimationInstance.hpp"
#include "Shader/PushUniform.hpp"
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
		uint32_t l_index = p_pipeline.GetTexturesCount() + 1;

		if ( !p_depthMaps.empty() )
		{
			auto l_layer = 0u;

			if ( GetShadowType( p_pipeline.GetFlags().m_sceneFlags ) != GLSL::ShadowType::eNone )
			{
				for ( auto & l_depthMap : p_depthMaps )
				{
					switch ( l_depthMap.get().GetType() )
					{
					case TextureType::eTwoDimensions:
						l_depthMap.get().SetIndex( l_index );
						p_pipeline.GetDirectionalShadowMapsVariable().SetValue( l_index++ );
						break;

					case TextureType::eTwoDimensionsArray:
						l_depthMap.get().SetIndex( l_index );
						p_pipeline.GetSpotShadowMapsVariable().SetValue( l_index++ );
						break;

					case TextureType::eCube:
						l_depthMap.get().SetIndex( l_index );
						p_pipeline.GetPointShadowMapsVariable().SetValue( l_index++, l_layer++ );
						break;

					case TextureType::eCubeArray:
						l_depthMap.get().SetIndex( l_index );
						p_pipeline.GetPointShadowMapsVariable().SetValue( l_index++ );
						break;
					}
				}
			}
		}

		return l_index;
	}

	inline void DoBindPass( PassRenderNodeUniforms & p_node
		, SceneNode & p_sceneNode
		, Pass & p_pass
		, Scene & p_scene
		, RenderPipeline & p_pipeline
		, DepthMapArray & p_depthMaps )
	{
		if ( CheckFlag( p_pipeline.GetFlags().m_programFlags, ProgramFlag::eLighting ) )
		{
			p_scene.GetLightCache().BindLights();
		}

		auto l_index = DoFillShaderDepthMaps( p_pipeline, p_depthMaps );

		p_pass.UpdateRenderNode( p_node );
		p_pass.BindTextures();

		for ( auto & l_depthMap : p_depthMaps )
		{
			l_depthMap.get().Bind();
		}

		if ( p_pass.HasEnvironmentMapping() )
		{
			auto & l_map = p_scene.GetEnvironmentMap( p_sceneNode );

			if ( CheckFlag( p_pipeline.GetFlags().m_programFlags, ProgramFlag::eLighting ) )
			{
				p_pipeline.GetEnvironmentMapVariable().SetValue( l_index );
				l_map.GetTexture().SetIndex( l_index );
				l_map.GetTexture().Bind();
			}
			else
			{
				p_node.m_environmentIndex.SetValue( float( l_map.GetIndex() ) );
			}
		}
		else
		{
			p_node.m_environmentIndex.SetValue( 0 );
		}

		p_node.m_refractionRatio.SetValue( p_pass.GetRefractionRatio() );

		if ( p_pass.NeedsGammaCorrection() )
		{
			p_node.m_gamma.SetValue( p_scene.GetHdrConfig().GetGamma() );
		}
		else
		{
			p_node.m_gamma.SetValue( 1.0f );
		}

		p_node.m_exposure.SetValue( p_scene.GetHdrConfig().GetExposure() );
		p_node.m_passUbo.Update();
	}

	inline void DoUnbindPass( PassRenderNodeUniforms & p_node
		, SceneNode & p_sceneNode
		, Pass & p_pass
		, Scene & p_scene
		, RenderPipeline & p_pipeline
		, DepthMapArray const & p_depthMaps )
	{
		if ( p_pass.HasEnvironmentMapping()
			&& CheckFlag( p_pipeline.GetFlags().m_programFlags, ProgramFlag::eLighting ) )
		{
			auto & l_map = p_scene.GetEnvironmentMap( p_sceneNode );
			l_map.GetTexture().Unbind();
		}

		for ( auto & l_depthMap : p_depthMaps )
		{
			l_depthMap.get().Unbind();
		}

		p_pass.UnbindTextures();

		if ( CheckFlag( p_pipeline.GetFlags().m_programFlags, ProgramFlag::eLighting ) )
		{
			p_scene.GetLightCache().UnbindLights();
		}
	}

	inline void DoBindPassOpacityMap( PassRenderNodeUniforms & p_node
		, Pass & p_pass )
	{
		auto l_unit = p_pass.GetTextureUnit( TextureChannel::eOpacity );

		if ( l_unit )
		{
			p_node.m_textures.find( l_unit->GetIndex() )->second.get().SetValue( 0 );
			l_unit->GetTexture()->Bind( 0u );
			l_unit->GetSampler()->Bind( 0u );
		}
	}

	inline void DoUnbindPassOpacityMap( PassRenderNodeUniforms & p_node
		, Pass & p_pass )
	{
		auto l_unit = p_pass.GetTextureUnit( TextureChannel::eOpacity );

		if ( l_unit )
		{
			l_unit->GetSampler()->Unbind( 0u );
			l_unit->GetTexture()->Unbind( 0u );
		}
	}

	template< typename DataType, typename InstanceType >
	inline void DoRenderObjectNode( ObjectRenderNode< DataType, InstanceType > & p_node )
	{
		auto & l_model = p_node.m_sceneNode.GetDerivedTransformationMatrix();
		auto & l_view = p_node.m_pipeline.GetViewMatrix();
		p_node.m_modelMatrix.SetValue( l_model );
		p_node.m_normalMatrix.SetValue( Matrix4x4r{ ( l_model * l_view ).get_minor( 3, 3 ).invert().get_transposed() } );
		p_node.m_modelMatrixUbo.Update();
		p_node.m_data.Draw( p_node.m_buffers );
	}

	inline void DoRenderNodeNoPass( StaticRenderNode & p_node )
	{
		DoRenderObjectNode( p_node );
	}

	inline void DoRenderNodeNoPass( BillboardRenderNode & p_node )
	{
		auto const & l_dimensions = p_node.m_data.GetDimensions();
		p_node.m_dimensions.SetValue( Point2i( l_dimensions.width(), l_dimensions.height() ) );
		p_node.m_billboardUbo.Update();
		DoRenderObjectNode( p_node );
	}

	inline void DoRenderNodeNoPass( MorphingRenderNode & p_node )
	{
		if ( p_node.m_mesh.IsPlayingAnimation() )
		{
			auto l_submesh = p_node.m_mesh.GetPlayingAnimation().GetAnimationSubmesh( p_node.m_data.GetId() );

			if ( l_submesh )
			{
				l_submesh->FillShader( p_node.m_time );
			}
		}
		else
		{
			p_node.m_time.SetValue( 1.0f );
		}

		p_node.m_morphingUbo.Update();
		DoRenderObjectNode( p_node );
	}

	inline void DoRenderNodeNoPass( SkinningRenderNode & p_node )
	{
		p_node.m_skeleton.FillShader( p_node.m_bonesMatrix );
		p_node.m_skinningUbo.Update();
		DoRenderObjectNode( p_node );
	}

	template< typename NodeType >
	inline void DoRenderNode( NodeType & p_node )
	{
		p_node.m_shadowReceiver.SetValue( p_node.m_instance.IsShadowReceiver() ? 1 : 0 );
		p_node.m_modelUbo.Update();
		DoRenderNodeNoPass( p_node );
	}
}
