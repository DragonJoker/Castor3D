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
	template< typename DataType, typename InstanceType >
	inline void doUpdateObjectNode( ObjectRenderNode< DataType, InstanceType > & node )
	{
		auto & model = node.m_sceneNode.getDerivedTransformationMatrix();
		node.m_modelMatrixUbo.getData().model = model;
	}

	inline void doUpdateNodeModelMatrix( StaticRenderNode & node )
	{
		doUpdateObjectNode( node );
	}

	inline void doUpdateNodeModelMatrix( BillboardRenderNode & node )
	{
		doUpdateObjectNode( node );
	}

	inline void doUpdateNodeModelMatrix( MorphingRenderNode & node )
	{
		doUpdateObjectNode( node );
	}

	inline void doUpdateNodeModelMatrix( SkinningRenderNode & node )
	{
		doUpdateObjectNode( node );
	}

	template< typename NodeType >
	inline void doUpdateNode( NodeType & node )
	{
		node.m_modelUbo.getData().shadowReceiver = node.m_instance.isShadowReceiver();
		node.m_modelUbo.getData().materialIndex = node.m_passNode.m_pass.getId();
		doUpdateNodeModelMatrix( node );
	}
}
