/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SkinningRenderNode_H___
#define ___C3D_SkinningRenderNode_H___

#include "Castor3D/Render/RenderNode/ObjectRenderNode.hpp"
#include "Castor3D/Shader/Ubos/SkinningUbo.hpp"

namespace castor3d
{
	struct SkinningRenderNode
		: public SubmeshRenderNode
	{
		C3D_API SkinningRenderNode( RenderPipeline & pipeline
			, PassRenderNode && passNode
			, UniformBufferOffset< ModelMatrixUbo::Configuration > modelMatrixBuffer
			, UniformBufferOffset< ModelUbo::Configuration > modelBuffer
			, UniformBufferOffset< PickingUbo::Configuration > pickingBuffer
			, UniformBufferOffset< TexturesUbo::Configuration > texturesBuffer
			, GeometryBuffers const & buffers
			, SceneNode & sceneNode
			, DataType & data
			, InstanceType & instance
			, AnimatedSkeleton & skeleton
			, UniformBufferOffset < SkinningUbo::Configuration > skinningUbo );

		//!\~english	The animated skeleton.
		//!\~french		Le squelette animé.
		AnimatedSkeleton & skeleton;
		//!\~english	The animation UBO.
		//!\~french		L'UBO d'animation.
		UniformBufferOffset< SkinningUbo::Configuration > skinningUbo;
	};
}

#endif
