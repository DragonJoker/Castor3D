/*
See LICENSE file in root folder
*/
#ifndef ___C3D_StaticRenderNode_H___
#define ___C3D_StaticRenderNode_H___

#include "Castor3D/Render/Node/ObjectRenderNode.hpp"

namespace castor3d
{
	struct StaticRenderNode
		: public SubmeshRenderNode
	{
		C3D_API StaticRenderNode( PassRenderNode passNode
			, UniformBufferOffsetT< ModelUboConfiguration > modelBuffer
			, UniformBufferOffsetT< ModelInstancesUboConfiguration > modelInstancesBuffer
			, GeometryBuffers const & buffers
			, SceneNode & sceneNode
			, Submesh & data
			, Geometry & instance );
	};
}

#endif
