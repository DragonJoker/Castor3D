/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MorphingRenderNode_H___
#define ___C3D_MorphingRenderNode_H___

#include "Castor3D/Render/RenderNode/ObjectRenderNode.hpp"
#include "Castor3D/Shader/Ubos/MorphingUbo.hpp"

namespace castor3d
{
	struct MorphingRenderNode
		: public SubmeshRenderNode
	{
		C3D_API MorphingRenderNode( RenderPipeline & pipeline
			, PassRenderNode && passNode
			, UniformBufferOffset< ModelMatrixUbo::Configuration > modelMatrixBuffer
			, UniformBufferOffset< ModelUbo::Configuration > modelBuffer
			, UniformBufferOffset< PickingUbo::Configuration > pickingBuffer
			, UniformBufferOffset< TexturesUbo::Configuration > texturesBuffer
			, GeometryBuffers const & buffers
			, SceneNode & sceneNode
			, DataType & data
			, InstanceType & instance
			, AnimatedMesh & mesh
			, UniformBufferOffset< MorphingUbo::Configuration > morphingUbo );

		//!\~english	The animated mesh.
		//!\~french		Le maillage animé.
		AnimatedMesh & mesh;
		//!\~english	The morphing UBO.
		//!\~french		L'UBO de morphing.
		UniformBufferOffset< MorphingUbo::Configuration > morphingUbo;
	};
}

#endif
