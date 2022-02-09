#include "Castor3D/Render/Node/SubmeshRenderNode.hpp"

CU_ImplementCUSmartPtr( castor3d, SubmeshRenderNode )

namespace castor3d
{
	SubmeshRenderNode::SubmeshRenderNode( PassRenderNode passNode
		, UniformBufferOffsetT< ModelIndexUboConfiguration > modelIndexBuffer
		, GpuDataBufferOffset * modelDataBuffer
		, UniformBufferOffsetT< ModelInstancesUboConfiguration > modelInstancesBuffer
		, GeometryBuffers const & buffers
		, SceneNode & sceneNode
		, Submesh & data
		, Geometry & instance )
		: passNode{ std::move( passNode ) }
		, modelIndexUbo{ std::move( modelIndexBuffer ) }
		, modelDataUbo{ std::move( modelDataBuffer ) }
		, modelInstancesUbo{ std::move( modelInstancesBuffer ) }
		, buffers{ buffers }
		, sceneNode{ sceneNode }
		, data{ data }
		, instance{ instance }
	{
	}
}
