/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BillboardRenderNode_H___
#define ___C3D_BillboardRenderNode_H___

#include "Castor3D/Render/Node/ObjectRenderNode.hpp"

#include "Castor3D/Shader/Ubos/UbosModule.hpp"

namespace castor3d
{
	struct BillboardRenderNode
		: public BillboardListRenderNode
	{
		C3D_API BillboardRenderNode( PassRenderNode passNode
			, UniformBufferOffsetT< ModelUboConfiguration > modelBuffer
			, UniformBufferOffsetT< BillboardUboConfiguration > billboardBuffer
			, UniformBufferOffsetT< ModelInstancesUboConfiguration > modelInstancesBuffer
			, GeometryBuffers const & buffers
			, SceneNode & sceneNode
			, BillboardBase & data );

		//!\~english	The billboard UBO.
		//!\~french		L'UBO de billboard.
		UniformBufferOffsetT< BillboardUboConfiguration > billboardUbo;
	};
}

#endif
