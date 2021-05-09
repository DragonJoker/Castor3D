#include "Castor3D/Render/Node/SkinningRenderNode.hpp"

#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Shader/Program.hpp"

using namespace castor;

namespace castor3d
{
	SkinningRenderNode::SkinningRenderNode( PassRenderNode passNode
		, UniformBufferOffsetT< ModelUboConfiguration > modelBuffer
		, UniformBufferOffsetT< ModelInstancesUboConfiguration > modelInstancesBuffer
		, GeometryBuffers const & buffers
		, SceneNode & sceneNode
		, Submesh & data
		, Geometry & instance
		, AnimatedSkeleton & skeleton
		, UniformBufferOffsetT< SkinningUboConfiguration > skinningUbo )
		: SubmeshRenderNode{ std::move( passNode )
			, modelBuffer
			, modelInstancesBuffer
			, buffers
			, sceneNode
			, data
			, instance }
		, skeleton{ skeleton }
		, skinningUbo{ skinningUbo }
	{
	}
}
