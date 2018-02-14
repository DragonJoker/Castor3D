#include "BillboardRenderNode.hpp"

#include "Render/RenderPipeline.hpp"
#include "Scene/SceneNode.hpp"
#include "Scene/BillboardList.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Castor3DPrerequisites.hpp"

using namespace castor;

namespace castor3d
{
	BillboardRenderNode::BillboardRenderNode( RenderPipeline & pipeline
		, PassRenderNode && passNode
		, UniformBufferOffset< ModelMatrixUbo::Configuration > modelMatrixBuffer
		, UniformBufferOffset< ModelUbo::Configuration > modelBuffer
		, renderer::GeometryBuffers const & buffers
		, SceneNode & sceneNode
		, BillboardBase & data
		, UniformBufferOffset< BillboardUbo::Configuration > billboardUbo )
		: BillboardListRenderNode{ pipeline
			, std::move( passNode )
			, modelMatrixBuffer
			, modelBuffer
			, buffers
			, sceneNode
			, data
			, data }
		, m_billboardUbo{ billboardUbo }
	{
	}
}
