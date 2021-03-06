#include "Castor3D/Render/Node/BillboardRenderNode.hpp"

CU_ImplementCUSmartPtr( castor3d, BillboardRenderNode )

namespace castor3d
{
	BillboardRenderNode::BillboardRenderNode( PassRenderNode passNode
		, UniformBufferOffsetT< ModelUboConfiguration > modelBuffer
		, UniformBufferOffsetT< ModelInstancesUboConfiguration > modelInstancesBuffer
		, GeometryBuffers const & buffers
		, SceneNode & sceneNode
		, BillboardBase & data
		, UniformBufferOffsetT< BillboardUboConfiguration > billboardBuffer )
		: passNode{ std::move( passNode ) }
		, modelUbo{ std::move( modelBuffer ) }
		, modelInstancesUbo{ std::move( modelInstancesBuffer ) }
		, buffers{ buffers }
		, sceneNode{ sceneNode }
		, data{ data }
		, instance{ data }
		, billboardUbo{ std::move( billboardBuffer ) }
	{
	}
}
