/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MorphingRenderNode_H___
#define ___C3D_MorphingRenderNode_H___

#include "Castor3D/Render/Node/ObjectRenderNode.hpp"
#include "Castor3D/Scene/Animation/AnimationModule.hpp"

namespace castor3d
{
	struct MorphingRenderNode
		: public SubmeshRenderNode
	{
		C3D_API MorphingRenderNode( PassRenderNode passNode
			, UniformBufferOffsetT< ModelUboConfiguration > modelBuffer
			, UniformBufferOffsetT< PickingUboConfiguration > pickingBuffer
			, UniformBufferOffsetT< TexturesUboConfiguration > texturesBuffer
			, UniformBufferOffsetT< ModelInstancesUboConfiguration > modelInstancesBuffer
			, GeometryBuffers const & buffers
			, SceneNode & sceneNode
			, DataType & data
			, InstanceType & instance
			, AnimatedMesh & mesh
			, UniformBufferOffsetT< MorphingUboConfiguration > morphingUbo );

		//!\~english	The animated mesh.
		//!\~french		Le maillage animé.
		AnimatedMesh & mesh;
		//!\~english	The morphing UBO.
		//!\~french		L'UBO de morphing.
		UniformBufferOffsetT< MorphingUboConfiguration > morphingUbo;
	};
}

#endif
