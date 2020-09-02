/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MorphingRenderNode_H___
#define ___C3D_MorphingRenderNode_H___

#include "Castor3D/Render/Node/ObjectRenderNode.hpp"
#include "Castor3D/Scene/Animation/AnimationModule.hpp"
#include "Castor3D/Shader/Ubos/MorphingUbo.hpp"

namespace castor3d
{
	struct MorphingRenderNode
		: public SubmeshRenderNode
	{
		C3D_API MorphingRenderNode( RenderPipeline & pipeline
			, PassRenderNode && passNode
			, UniformBufferOffsetT< ModelMatrixUbo::Configuration > modelMatrixBuffer
			, UniformBufferOffsetT< ModelUbo::Configuration > modelBuffer
			, UniformBufferOffsetT< PickingUbo::Configuration > pickingBuffer
			, UniformBufferOffsetT< TexturesUbo::Configuration > texturesBuffer
			, GeometryBuffers const & buffers
			, SceneNode & sceneNode
			, DataType & data
			, InstanceType & instance
			, AnimatedMesh & mesh
			, UniformBufferOffsetT< MorphingUbo::Configuration > morphingUbo );

		//!\~english	The animated mesh.
		//!\~french		Le maillage animé.
		AnimatedMesh & mesh;
		//!\~english	The morphing UBO.
		//!\~french		L'UBO de morphing.
		UniformBufferOffsetT< MorphingUbo::Configuration > morphingUbo;
	};
}

#endif
