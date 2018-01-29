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
#include "ShadowMap/ShadowMap.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"
#include "Texture/TextureUnit.hpp"

#include <Shader/ShaderProgram.hpp>

using namespace castor;

namespace castor3d
{
	inline void doBindPass( SceneNode & sceneNode
		, PassRenderNode & node
		, Scene & scene
		, RenderPipeline & pipeline
		, ShadowMapLightTypeArray & shadowMaps
		, ModelUbo & model
		, EnvironmentMap *& envMap )
	{
		uint32_t index = pipeline.getTexturesCount() + MinTextureIndex;

		for ( auto pair : node.m_textures )
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

		if ( node.m_pass.hasEnvironmentMapping() )
		{
			envMap = &scene.getEnvironmentMap( sceneNode );

			if ( checkFlag( pipeline.getFlags().m_programFlags, ProgramFlag::eLighting ) )
			{
				pipeline.getEnvironmentMapVariable().setValue( index );
				envMap->getTexture().setIndex( index );
				envMap->getTexture().bind();
			}
			else
			{
				model.setEnvMapIndex( envMap->getIndex() );
			}
		}
		else
		{
			model.setEnvMapIndex( 0 );
			envMap = nullptr;
		}
	}

	inline void doBindPassOpacityMap( PassRenderNode & node
		, Pass & pass )
	{
		auto unit = pass.getTextureUnit( TextureChannel::eOpacity );

		if ( unit )
		{
			node.m_textures.find( unit->getIndex() )->second.get().setValue( 0 );
			unit->getTexture()->bind( 0u );
			unit->getSampler()->bind( 0u );
		}
	}

	inline void doUnbindPassOpacityMap( PassRenderNode & node
		, Pass & pass )
	{
		auto unit = pass.getTextureUnit( TextureChannel::eOpacity );

		if ( unit )
		{
			unit->getSampler()->unbind( 0u );
			unit->getTexture()->unbind( 0u );
		}
	}

	template< typename DataType, typename InstanceType >
	inline void doRenderObjectNode( ObjectRenderNode< DataType, InstanceType > & node )
	{
		auto & model = node.m_sceneNode.getDerivedTransformationMatrix();
		node.m_modelMatrixUbo.update( model );
		node.m_data.draw( node.m_buffers );
	}

	inline void doRenderNodeNoPass( StaticRenderNode & node )
	{
		doRenderObjectNode( node );
	}

	inline void doRenderNodeNoPass( BillboardRenderNode & node )
	{
		node.m_billboardUbo.update( node.m_instance.getDimensions() );
		doRenderObjectNode( node );
	}

	inline void doRenderNodeNoPass( MorphingRenderNode & node )
	{
		if ( node.m_mesh.isPlayingAnimation() )
		{
			auto submesh = node.m_mesh.getPlayingAnimation().getAnimationSubmesh( node.m_data.getId() );

			if ( submesh )
			{
				node.m_morphingUbo.update( submesh->getCurrentFactor() );
			}
			else
			{
				node.m_morphingUbo.update( 1.0f );
			}
		}
		else
		{
			node.m_morphingUbo.update( 1.0f );
		}

		doRenderObjectNode( node );
	}

	inline void doRenderNodeNoPass( SkinningRenderNode & node )
	{
		if ( !checkFlag( node.m_pipeline.getFlags().m_programFlags, ProgramFlag::eInstantiation ) )
		{
			node.m_skinningUbo.update( node.m_skeleton );
		}

		doRenderObjectNode( node );
	}

	template< typename NodeType >
	inline void doRenderNode( NodeType & node )
	{
		node.m_modelUbo.update( node.m_instance.isShadowReceiver()
			, node.m_passNode.m_pass.getId() );
		doRenderNodeNoPass( node );
	}
}
