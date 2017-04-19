#include "BillboardRenderNode.hpp"

#include "Render/RenderPipeline.hpp"
#include "Scene/SceneNode.hpp"
#include "Scene/BillboardList.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Shader/UniformBuffer.hpp"

using namespace Castor;

namespace Castor3D
{
	BillboardRenderNode::BillboardRenderNode( RenderPipeline & p_pipeline
		, PassRenderNode && p_passNode
		, ModelMatrixUbo & p_modelMatrixBuffer
		, ModelUbo & p_modelBuffer
		, GeometryBuffers & p_buffers
		, SceneNode & p_sceneNode
		, BillboardBase & p_data
		, BillboardUbo & p_billboardUbo )
		: BillboardListRenderNode{ p_pipeline
			, std::move( p_passNode )
			, p_modelMatrixBuffer
			, p_modelBuffer
			, p_buffers
			, p_sceneNode
			, p_data
			, p_data }
		, m_billboardUbo{ p_billboardUbo }
	{
	}
}
