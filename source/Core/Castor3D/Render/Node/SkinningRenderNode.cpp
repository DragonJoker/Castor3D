#include "Castor3D/Render/Node/SkinningRenderNode.hpp"

#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Shader/Program.hpp"

using namespace castor;

namespace castor3d
{
	SkinningRenderNode::SkinningRenderNode( RenderPipeline & pipeline
		, PassRenderNode && passNode
		, UniformBufferOffsetT< ModelMatrixUbo::Configuration > modelMatrixBuffer
		, UniformBufferOffsetT< ModelUbo::Configuration > modelBuffer
		, UniformBufferOffsetT< PickingUbo::Configuration > pickingBuffer
		, UniformBufferOffsetT< TexturesUbo::Configuration > texturesBuffer
		, GeometryBuffers const & buffers
		, SceneNode & sceneNode
		, Submesh & data
		, Geometry & instance
		, AnimatedSkeleton & skeleton
		, UniformBufferOffsetT< SkinningUbo::Configuration > skinningUbo )
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
		, skeleton{ skeleton }
		, skinningUbo{ skinningUbo }
	{
	}
}
