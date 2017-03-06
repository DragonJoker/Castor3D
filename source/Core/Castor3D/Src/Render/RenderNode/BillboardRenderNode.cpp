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
		, PassRenderNode && p_pass
		, UniformBuffer & p_modelMatrixBuffer
		, UniformBuffer & p_modelBuffer
		, GeometryBuffers & p_buffers
		, SceneNode & p_sceneNode
		, BillboardBase & p_data
		, UniformBuffer & p_billboardUbo )
		: BillboardListRenderNode{ p_pipeline
			, std::move( p_pass )
			, p_modelMatrixBuffer
			, p_modelBuffer
			, p_buffers
			, p_sceneNode
			, p_data
			, p_data }
		, m_billboardUbo{ p_billboardUbo }
		, m_dimensions{ *p_billboardUbo.GetUniform< UniformType::eVec2i >( ShaderProgram::Dimensions ) }
		, m_windowSize{ *p_billboardUbo.GetUniform< UniformType::eVec2i >( ShaderProgram::WindowSize ) }
	{
	}
}
