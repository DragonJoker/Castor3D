/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SkinningRenderNode_H___
#define ___C3D_SkinningRenderNode_H___

#include "Castor3D/Scene/Animation/AnimationModule.hpp"

#include "Castor3D/Render/Node/ObjectRenderNode.hpp"

namespace castor3d
{
	struct SkinningRenderNode
		: public SubmeshRenderNode
	{
		C3D_API SkinningRenderNode( PassRenderNode passNode
			, UniformBufferOffsetT< ModelUboConfiguration > modelBuffer
			, UniformBufferOffsetT< ModelInstancesUboConfiguration > modelInstancesBuffer
			, GeometryBuffers const & buffers
			, SceneNode & sceneNode
			, DataType & data
			, InstanceType & instance
			, AnimatedSkeleton & skeleton
			, UniformBufferOffsetT < SkinningUboConfiguration > skinningUbo );

		//!\~english	The animated skeleton.
		//!\~french		Le squelette animé.
		AnimatedSkeleton & skeleton;
		//!\~english	The animation UBO.
		//!\~french		L'UBO d'animation.
		UniformBufferOffsetT< SkinningUboConfiguration > skinningUbo;
	};
}

#endif
