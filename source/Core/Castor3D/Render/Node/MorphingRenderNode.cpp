#include "Castor3D/Render/Node/MorphingRenderNode.hpp"

#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Shader/Program.hpp"

using namespace castor;

namespace castor3d
{
	MorphingRenderNode::MorphingRenderNode( PassRenderNode passNode
		, UniformBufferOffsetT< ModelUboConfiguration > modelBuffer
		, UniformBufferOffsetT< PickingUboConfiguration > pickingBuffer
		, UniformBufferOffsetT< TexturesUboConfiguration > texturesBuffer
		, UniformBufferOffsetT< ModelInstancesUboConfiguration > modelInstancesBuffer
		, GeometryBuffers const & buffers
		, SceneNode & sceneNode
		, DataType & data
		, InstanceType & instance
		, AnimatedMesh & mesh
		, UniformBufferOffsetT< MorphingUboConfiguration > morphingUbo )
		: SubmeshRenderNode{ std::move( passNode )
			, modelBuffer
			, pickingBuffer
			, texturesBuffer
			, modelInstancesBuffer
			, buffers
			, sceneNode
			, data
			, instance }
		, mesh{ mesh }
		, morphingUbo{ morphingUbo }
	{
	}
}
