/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SkinningRenderNode_H___
#define ___C3D_SkinningRenderNode_H___

#include "Castor3D/Render/Node/ObjectRenderNode.hpp"
#include "Castor3D/Shader/Ubos/SkinningUbo.hpp"

namespace castor3d
{
	struct SkinningRenderNode
		: public SubmeshRenderNode
	{
		C3D_API SkinningRenderNode( RenderPipeline & pipeline
			, PassRenderNode && passNode
			, UniformBufferOffsetT< ModelMatrixUbo::Configuration > modelMatrixBuffer
			, UniformBufferOffsetT< ModelUbo::Configuration > modelBuffer
			, UniformBufferOffsetT< PickingUbo::Configuration > pickingBuffer
			, UniformBufferOffsetT< TexturesUbo::Configuration > texturesBuffer
			, GeometryBuffers const & buffers
			, SceneNode & sceneNode
			, DataType & data
			, InstanceType & instance
			, AnimatedSkeleton & skeleton
			, UniformBufferOffsetT < SkinningUbo::Configuration > skinningUbo );

		//!\~english	The animated skeleton.
		//!\~french		Le squelette animé.
		AnimatedSkeleton & skeleton;
		//!\~english	The animation UBO.
		//!\~french		L'UBO d'animation.
		UniformBufferOffsetT< SkinningUbo::Configuration > skinningUbo;
	};
}

#endif
