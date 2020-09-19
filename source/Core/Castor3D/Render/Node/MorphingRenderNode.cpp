#include "Castor3D/Render/Node/MorphingRenderNode.hpp"

#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Shader/Program.hpp"

using namespace castor;

namespace castor3d
{
	MorphingRenderNode::MorphingRenderNode( RenderPipeline & pipeline
		, PassRenderNode && passNode
		, UniformBufferOffsetT< ModelMatrixUboConfiguration > modelMatrixBuffer
		, UniformBufferOffsetT< ModelUboConfiguration > modelBuffer
		, UniformBufferOffsetT< PickingUboConfiguration > pickingBuffer
		, UniformBufferOffsetT< TexturesUboConfiguration > texturesBuffer
		, GeometryBuffers const & buffers
		, SceneNode & sceneNode
		, DataType & data
		, InstanceType & instance
		, AnimatedMesh & mesh
		, UniformBufferOffsetT< MorphingUboConfiguration > morphingUbo )
		: SubmeshRenderNode{ pipeline
			, std::move( passNode )
			, modelMatrixBuffer
			, modelBuffer
			, pickingBuffer
			, texturesBuffer
			, buffers
			, sceneNode
			, data
			, instance }
		, mesh{ mesh }
		, morphingUbo{ morphingUbo }
	{
	}
}
