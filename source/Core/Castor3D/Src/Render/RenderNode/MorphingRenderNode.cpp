#include "MorphingRenderNode.hpp"

#include "Mesh/Submesh.hpp"
#include "Render/RenderPipeline.hpp"
#include "Scene/SceneNode.hpp"
#include "Scene/Geometry.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Castor3DPrerequisites.hpp"

using namespace castor;

namespace castor3d
{
	MorphingRenderNode::MorphingRenderNode( RenderPipeline & pipeline
		, PassRenderNode && passNode
		, UniformBufferOffset< ModelMatrixUbo::Configuration > modelMatrixBuffer
		, UniformBufferOffset< ModelUbo::Configuration > modelBuffer
		, UniformBufferOffset< PickingUbo::Configuration > pickingBuffer
		, GeometryBuffers const & buffers
		, SceneNode & sceneNode
		, DataType & data
		, InstanceType & instance
		, AnimatedMesh & mesh
		, UniformBufferOffset< MorphingUbo::Configuration > morphingUbo )
		: SubmeshRenderNode{ pipeline
			, std::move( passNode )
			, modelMatrixBuffer
			, modelBuffer
			, pickingBuffer
			, buffers
			, sceneNode
			, data
			, instance }
		, mesh{ mesh }
		, morphingUbo{ morphingUbo }
	{
	}
}
