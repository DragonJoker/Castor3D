#include "MorphingRenderNode.hpp"

#include "Mesh/Submesh.hpp"
#include "Render/RenderPipeline.hpp"
#include "Scene/SceneNode.hpp"
#include "Scene/Geometry.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Shader/UniformBuffer.hpp"

using namespace Castor;

namespace Castor3D
{
	MorphingRenderNode::MorphingRenderNode( RenderPipeline & p_pipeline
		, PassRenderNode && p_pass
		, UniformBuffer & p_modelMatrixBuffer
		, UniformBuffer & p_modelBuffer
		, GeometryBuffers & p_buffers
		, SceneNode & p_sceneNode
		, DataType & p_data
		, InstanceType & p_instance
		, AnimatedMesh & p_mesh
		, UniformBuffer & p_morphingUbo )
		: SubmeshRenderNode{ p_pipeline
			, std::move( p_pass )
			, p_modelMatrixBuffer
			, p_modelBuffer
			, p_buffers
			, p_sceneNode
			, p_data
			, p_instance }
		, m_mesh{ p_mesh }
		, m_morphingUbo{ p_morphingUbo }
		, m_time{ *p_morphingUbo.GetUniform< UniformType::eFloat >( ShaderProgram::Time ) }
	{
	}
}
