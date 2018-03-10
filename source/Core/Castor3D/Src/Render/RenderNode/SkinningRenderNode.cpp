#include "SkinningRenderNode.hpp"

#include "Mesh/Submesh.hpp"
#include "Render/RenderPipeline.hpp"
#include "Scene/SceneNode.hpp"
#include "Scene/Geometry.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Castor3DPrerequisites.hpp"

using namespace castor;

namespace castor3d
{
	SkinningRenderNode::SkinningRenderNode( RenderPipeline & pipeline
		, PassRenderNode && passNode
		, UniformBufferOffset< ModelMatrixUbo::Configuration > modelMatrixBuffer
		, UniformBufferOffset< ModelUbo::Configuration > modelBuffer
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
			, buffers
			, sceneNode
			, data
			, instance }
		, m_skeleton{ skeleton }
		, m_skinningUbo{ skinningUbo }
	{
	}
}
