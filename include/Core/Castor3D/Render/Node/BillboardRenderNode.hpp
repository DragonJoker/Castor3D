/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BillboardRenderNode_H___
#define ___C3D_BillboardRenderNode_H___

#include "Castor3D/Render/Node/ObjectRenderNode.hpp"
#include "Castor3D/Shader/Ubos/BillboardUbo.hpp"

namespace castor3d
{
	struct BillboardRenderNode
		: public BillboardListRenderNode
	{
		C3D_API BillboardRenderNode( RenderPipeline & pipeline
			, PassRenderNode && passNode
			, UniformBufferOffsetT< ModelMatrixUbo::Configuration > modelMatrixBuffer
			, UniformBufferOffsetT< ModelUbo::Configuration > modelBuffer
			, UniformBufferOffsetT< PickingUbo::Configuration > pickingBuffer
			, UniformBufferOffsetT< BillboardUbo::Configuration > billboardBuffer
			, UniformBufferOffsetT< TexturesUbo::Configuration > texturesBuffer
			, GeometryBuffers const & buffers
			, SceneNode & sceneNode
			, BillboardBase & data );

		//!\~english	The billboard UBO.
		//!\~french		L'UBO de billboard.
		UniformBufferOffsetT< BillboardUbo::Configuration > billboardUbo;
	};
}

#endif
