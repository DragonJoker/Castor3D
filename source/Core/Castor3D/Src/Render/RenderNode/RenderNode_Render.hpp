#include "RenderNode.hpp"

#include "Engine.hpp"
#include "Material/Pass.hpp"
#include "Render/RenderPipeline.hpp"
#include "Scene/BillboardList.hpp"
#include "Scene/Geometry.hpp"
#include "Scene/Scene.hpp"
#include "Shader/PushUniform.hpp"
#include "Shader/UniformBuffer.hpp"

using namespace Castor;

namespace Castor3D
{
	inline void DoFillShaderDepthMaps( RenderPipeline & p_pipeline
		, DepthMapArray & p_depthMaps )
	{
		if ( !p_depthMaps.empty() )
		{
			auto l_index = p_pipeline.GetTexturesCount() + 1;

			if ( CheckFlag( p_pipeline.GetFlags().m_programFlags, ProgramFlag::eShadows ) )
			{
				for ( auto & l_depthMap : p_depthMaps )
				{
					if ( l_depthMap.get().GetType() == TextureType::eTwoDimensions )
					{
						l_depthMap.get().SetIndex( l_index );
						p_pipeline.GetDirectionalShadowMapsVariable().SetValue( l_index++ );
					}
					else if ( l_depthMap.get().GetType() == TextureType::eTwoDimensionsArray )
					{
						l_depthMap.get().SetIndex( l_index );
						p_pipeline.GetSpotShadowMapsVariable().SetValue( l_index++ );
					}
					else if ( l_depthMap.get().GetType() == TextureType::eCube )
					{
						l_depthMap.get().SetIndex( l_index );
						p_pipeline.GetPointShadowMapsVariable().SetValue( l_index++ );
					}
					else if ( l_depthMap.get().GetType() == TextureType::eCubeArray )
					{
						l_depthMap.get().SetIndex( l_index );
						p_pipeline.GetPointShadowMapsVariable().SetValue( l_index++ );
					}
				}
			}
		}
	}

	inline void DoBindPass( PassRenderNodeUniforms & p_node
		, Pass & p_pass
		, Scene const & p_scene
		, RenderPipeline & p_pipeline
		, DepthMapArray & p_depthMaps )
	{
		if ( CheckFlag( p_pipeline.GetFlags().m_programFlags, ProgramFlag::eLighting ) )
		{
			p_scene.GetLightCache().BindLights();
		}

		DoFillShaderDepthMaps( p_pipeline, p_depthMaps );
		p_pass.UpdateRenderNode( p_node );
		p_node.m_passUbo.Update();
		p_pass.BindTextures();

		for ( auto & l_depthMap : p_depthMaps )
		{
			l_depthMap.get().Bind();
		}
	}

	inline void DoUnbindPass( PassRenderNodeUniforms & p_node
		, Pass & p_pass
		, Scene const & p_scene
		, RenderPipeline & p_pipeline
		, DepthMapArray const & p_depthMaps )
	{
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
}
