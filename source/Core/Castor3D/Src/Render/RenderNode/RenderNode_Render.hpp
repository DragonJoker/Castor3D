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

using namespace castor;

namespace castor3d
{
	inline uint32_t doFillShaderDepthMaps( RenderPipeline & p_pipeline
		, DepthMapArray & p_depthMaps )
	{
		uint32_t index = p_pipeline.getTexturesCount() + Pass::MinTextureIndex;

		if ( !p_depthMaps.empty() )
		{
			auto layer = 0u;

			if ( getShadowType( p_pipeline.getFlags().m_sceneFlags ) != GLSL::ShadowType::eNone )
			{
				for ( auto & depthMap : p_depthMaps )
				{
					switch ( depthMap.get().getType() )
					{
					case TextureType::eTwoDimensions:
						depthMap.get().setIndex( index );
						p_pipeline.getDirectionalShadowMapsVariable().setValue( index++ );
						break;

					case TextureType::eTwoDimensionsArray:
						depthMap.get().setIndex( index );
						p_pipeline.getSpotShadowMapsVariable().setValue( index++ );
						break;

					case TextureType::eCube:
						depthMap.get().setIndex( index );
						p_pipeline.getPointShadowMapsVariable().setValue( index++, layer++ );
						break;

					case TextureType::eCubeArray:
						depthMap.get().setIndex( index );
						p_pipeline.getPointShadowMapsVariable().setValue( index++ );
						break;
					}
				}
			}
		}

		return index;
	}

	inline uint32_t doFillShaderPbrMaps( RenderPipeline & p_pipeline
		, Scene & p_scene
		, SceneNode & p_sceneNode
		, uint32_t p_index )
	{
		p_scene.getIbl( p_sceneNode ).getIrradiance().getTexture()->bind( p_index );
		p_scene.getIbl( p_sceneNode ).getIrradiance().getSampler()->bind( p_index );
		p_pipeline.getIrradianceMapVariable().setValue( p_index++ );
		p_scene.getIbl( p_sceneNode ).getPrefilteredEnvironment().getTexture()->bind( p_index );
		p_scene.getIbl( p_sceneNode ).getPrefilteredEnvironment().getSampler()->bind( p_index );
		p_pipeline.getPrefilteredMapVariable().setValue( p_index++ );
		p_scene.getSkybox().getIbl().getPrefilteredBrdf().getTexture()->bind( p_index );
		p_scene.getSkybox().getIbl().getPrefilteredBrdf().getSampler()->bind( p_index );
		p_pipeline.getBrdfMapVariable().setValue( p_index++ );
		return p_index;
	}

	inline void doBindPass( SceneNode & p_sceneNode
		, PassRenderNode & p_node
		, Scene & p_scene
		, RenderPipeline & p_pipeline
		, DepthMapArray & p_depthMaps
		, ModelUbo & p_model
		, EnvironmentMap *& p_envMap )
	{
		auto index = doFillShaderDepthMaps( p_pipeline, p_depthMaps );

		p_node.m_pass.bindTextures();

		if ( ( checkFlag( p_pipeline.getFlags().m_programFlags, ProgramFlag::ePbrMetallicRoughness )
				|| checkFlag( p_pipeline.getFlags().m_programFlags, ProgramFlag::ePbrSpecularGlossiness ) )
			&& checkFlag( p_pipeline.getFlags().m_programFlags, ProgramFlag::eLighting ) )
		{
			index = doFillShaderPbrMaps( p_pipeline
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
				variable.get().setValue( texture );
			}
		}

		for ( auto & depthMap : p_depthMaps )
		{
			depthMap.get().bind();
		}

		if ( p_node.m_pass.hasEnvironmentMapping() )
		{
			p_envMap = &p_scene.getEnvironmentMap( p_sceneNode );

			if ( checkFlag( p_pipeline.getFlags().m_programFlags, ProgramFlag::eLighting ) )
			{
				p_pipeline.getEnvironmentMapVariable().setValue( index );
				p_envMap->getTexture().setIndex( index );
				p_envMap->getTexture().bind();
			}
			else
			{
				p_model.setEnvMapIndex( p_envMap->getIndex() );
			}
		}
		else
		{
			p_model.setEnvMapIndex( 0 );
			p_envMap = nullptr;
		}
	}

	inline void doUnbindPass( SceneNode & p_sceneNode
		, PassRenderNode & p_node
		, Scene & p_scene
		, RenderPipeline & p_pipeline
		, DepthMapArray const & p_depthMaps
		, EnvironmentMap * p_envMap )
	{
		if ( p_envMap )
		{
			p_envMap->getTexture().unbind();
		}

		for ( auto & depthMap : p_depthMaps )
		{
			depthMap.get().unbind();
		}

		p_node.m_pass.unbindTextures();
	}

	inline void doBindPassOpacityMap( PassRenderNode & p_node
		, Pass & p_pass )
	{
		auto unit = p_pass.getTextureUnit( TextureChannel::eOpacity );

		if ( unit )
		{
			p_node.m_textures.find( unit->getIndex() )->second.get().setValue( 0 );
			unit->getTexture()->bind( 0u );
			unit->getSampler()->bind( 0u );
		}
	}

	inline void doUnbindPassOpacityMap( PassRenderNode & p_node
		, Pass & p_pass )
	{
		auto unit = p_pass.getTextureUnit( TextureChannel::eOpacity );

		if ( unit )
		{
			unit->getSampler()->unbind( 0u );
			unit->getTexture()->unbind( 0u );
		}
	}

	template< typename DataType, typename InstanceType >
	inline void doRenderObjectNode( ObjectRenderNode< DataType, InstanceType > & p_node )
	{
		auto & model = p_node.m_sceneNode.getDerivedTransformationMatrix();
		p_node.m_modelMatrixUbo.update( model );
		p_node.m_data.draw( p_node.m_buffers );
	}

	inline void doRenderNodeNoPass( StaticRenderNode & p_node )
	{
		doRenderObjectNode( p_node );
	}

	inline void doRenderNodeNoPass( BillboardRenderNode & p_node )
	{
		p_node.m_billboardUbo.update( p_node.m_instance.getDimensions() );
		doRenderObjectNode( p_node );
	}

	inline void doRenderNodeNoPass( MorphingRenderNode & p_node )
	{
		if ( p_node.m_mesh.isPlayingAnimation() )
		{
			auto submesh = p_node.m_mesh.getPlayingAnimation().getAnimationSubmesh( p_node.m_data.getId() );

			if ( submesh )
			{
				p_node.m_morphingUbo.update( submesh->getCurrentFactor() );
			}
			else
			{
				p_node.m_morphingUbo.update( 1.0f );
			}
		}
		else
		{
			p_node.m_morphingUbo.update( 1.0f );
		}

		doRenderObjectNode( p_node );
	}

	inline void doRenderNodeNoPass( SkinningRenderNode & p_node )
	{
		if ( !checkFlag( p_node.m_pipeline.getFlags().m_programFlags, ProgramFlag::eInstantiation ) )
		{
			p_node.m_skinningUbo.update( p_node.m_skeleton );
		}

		doRenderObjectNode( p_node );
	}

	template< typename NodeType >
	inline void doRenderNode( NodeType & p_node )
	{
		p_node.m_modelUbo.update( p_node.m_instance.isShadowReceiver()
			, p_node.m_passNode.m_pass.getId() );
		doRenderNodeNoPass( p_node );
	}
}
