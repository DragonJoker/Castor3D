#include "Castor3D/Render/Node/StaticRenderNode.hpp"

#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Geometry.hpp"

using namespace castor;

namespace castor3d
{
	StaticRenderNode::StaticRenderNode( PassRenderNode passNode
		, UniformBufferOffsetT< ModelMatrixUboConfiguration > modelMatrixBuffer
		, UniformBufferOffsetT< ModelUboConfiguration > modelBuffer
		, UniformBufferOffsetT< PickingUboConfiguration > pickingBuffer
		, UniformBufferOffsetT< TexturesUboConfiguration > texturesBuffer
		, UniformBufferOffsetT< ModelInstancesUboConfiguration > modelInstancesBuffer
		, GeometryBuffers const & buffers
		, SceneNode & sceneNode
		, Submesh & data
		, Geometry & instance )
		: SubmeshRenderNode{ std::move( passNode )
			, modelMatrixBuffer
			, modelBuffer
			, pickingBuffer
			, texturesBuffer
			, modelInstancesBuffer
			, buffers
			, sceneNode
			, data
			, instance }
	{
	}
}
