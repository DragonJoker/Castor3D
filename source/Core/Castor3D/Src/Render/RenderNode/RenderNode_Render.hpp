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
#include "Shader/Ubos/BillboardUbo.hpp"
#include "Shader/Ubos/MatrixUbo.hpp"
#include "Shader/Ubos/ModelMatrixUbo.hpp"
#include "Shader/Ubos/ModelUbo.hpp"
#include "Shader/Ubos/SceneUbo.hpp"
#include "Shader/Uniform/PushUniform.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Shader/UniformBuffer.hpp"
#include "ShadowMap/ShadowMap.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"
#include "Texture/TextureUnit.hpp"

using namespace castor;

namespace castor3d
{
	inline uint32_t doFillShaderShadowMaps( RenderPipeline & p_pipeline
		, ShadowMapLightTypeArray & shadowMaps )
	{
		uint32_t index = p_pipeline.getTexturesCount() + MinTextureIndex;

		if ( getShadowType( p_pipeline.getFlags().m_sceneFlags ) != ShadowType::eNone )
		{
			for ( auto i = 0u; i < shadowMaps.size(); ++i )
			{
				auto lightType = LightType( i );
				auto layer = 0u;

				for ( auto shadowMap : shadowMaps[i] )
				{
					auto & unit = shadowMap.get().getTexture();
					unit.getTexture()->bind( index );
					unit.getSampler()->bind( index );

					switch ( lightType )
					{
					case LightType::eDirectional:
						p_pipeline.getDirectionalShadowMapsVariable().setValue( index++, layer++ );
						break;

					case LightType::eSpot:
						p_pipeline.getSpotShadowMapsVariable().setValue( index++, layer++ );
						break;

					case LightType::ePoint:
						p_pipeline.getPointShadowMapsVariable().setValue( index++, layer++ );
						++layer;
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
		, ShadowMapLightTypeArray & shadowMaps
		, ModelUbo & p_model
		, EnvironmentMap *& p_envMap )
	{
		p_node.m_pass.bindTextures();
		auto index = doFillShaderShadowMaps( p_pipeline, shadowMaps );

		if ( ( checkFlag( p_pipeline.getFlags().m_passFlags, PassFlag::ePbrMetallicRoughness )
				|| checkFlag( p_pipeline.getFlags().m_passFlags, PassFlag::ePbrSpecularGlossiness ) )
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

		for ( auto & array : shadowMaps )
		{
			for ( auto & shadowMap : array )
			{
				shadowMap.get().getTexture().bind();
			}
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
