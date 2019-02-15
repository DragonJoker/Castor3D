#include "Castor3D/Render/RenderNode/SkinningRenderNode.hpp"

#include "Castor3D/Mesh/Submesh.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Castor3DPrerequisites.hpp"

using namespace castor;

namespace castor3d
{
	SkinningRenderNode::SkinningRenderNode( RenderPipeline & pipeline
		, PassRenderNode && passNode
		, UniformBufferOffset< ModelMatrixUbo::Configuration > modelMatrixBuffer
		, UniformBufferOffset< ModelUbo::Configuration > modelBuffer
		, UniformBufferOffset< PickingUbo::Configuration > pickingBuffer
		, GeometryBuffers const & buffers
		, SceneNode & sceneNode
		, Submesh & data
		, Geometry & instance
		, AnimatedSkeleton & skeleton
		, UniformBufferOffset< SkinningUbo::Configuration > skinningUbo )
		: SubmeshRenderNode{ pipeline
			, std::move( passNode )
			, modelMatrixBuffer
			, modelBuffer
			, pickingBuffer
			, buffers
			, sceneNode
			, data
			, instance }
		, skeleton{ skeleton }
		, skinningUbo{ skinningUbo }
	{
	}
}
