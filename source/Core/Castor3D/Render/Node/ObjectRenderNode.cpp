#include "Castor3D/Render/Node/ObjectRenderNode.hpp"

#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Shader/Program.hpp"

namespace castor3d
{
	template<>
	ObjectRenderNode< Submesh, Geometry >::ObjectRenderNode( PassRenderNode passNode
		, UniformBufferOffsetT< ModelUboConfiguration > modelBuffer
		, UniformBufferOffsetT< PickingUboConfiguration > pickingBuffer
		, UniformBufferOffsetT< ModelInstancesUboConfiguration > modelInstancesBuffer
		, GeometryBuffers const & buffers
		, SceneNode & sceneNode
		, Submesh & data
		, Geometry & instance )
		: passNode{ std::move( passNode ) }
		, modelUbo{ modelBuffer }
		, pickingUbo{ pickingBuffer }
		, modelInstancesUbo{ modelInstancesBuffer }
		, buffers{ buffers }
		, sceneNode{ sceneNode }
		, data{ data }
		, instance{ instance }
	{
	}

	template<>
	ObjectRenderNode< BillboardBase, BillboardBase >::ObjectRenderNode( PassRenderNode passNode
		, UniformBufferOffsetT< ModelUboConfiguration > modelBuffer
		, UniformBufferOffsetT< PickingUboConfiguration > pickingBuffer
		, UniformBufferOffsetT< ModelInstancesUboConfiguration > modelInstancesBuffer
		, GeometryBuffers const & buffers
		, SceneNode & sceneNode
		, BillboardBase & data
		, BillboardBase & instance )
		: passNode{ std::move( passNode ) }
		, modelUbo{ modelBuffer }
		, pickingUbo{ pickingBuffer }
		, modelInstancesUbo{ modelInstancesBuffer }
		, buffers{ buffers }
		, sceneNode{ sceneNode }
		, data{ data }
		, instance{ instance }
	{
	}
}
