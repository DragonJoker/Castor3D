#include "StaticRenderNode.hpp"

#include "Mesh/Submesh.hpp"
#include "Render/RenderPipeline.hpp"
#include "Scene/SceneNode.hpp"
#include "Scene/Geometry.hpp"
#include "Castor3DPrerequisites.hpp"

using namespace castor;

namespace castor3d
{
	StaticRenderNode::StaticRenderNode( RenderPipeline & pipeline
		, PassRenderNode && passNode
		, UniformBufferOffset< ModelMatrixUbo::Configuration > modelMatrixBuffer
		, UniformBufferOffset< ModelUbo::Configuration > modelBuffer
		, GeometryBuffers const & buffers
		, SceneNode & sceneNode
		, Submesh & data
		, Geometry & instance )
		: SubmeshRenderNode{ pipeline
			, std::move( passNode )
			, modelMatrixBuffer
			, modelBuffer
			, buffers
			, sceneNode
			, data
			, instance }
	{
	}
}
