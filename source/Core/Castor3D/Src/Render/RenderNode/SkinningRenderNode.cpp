#include "SkinningRenderNode.hpp"

#include "Mesh/Submesh.hpp"
#include "Render/RenderPipeline.hpp"
#include "Scene/SceneNode.hpp"
#include "Scene/Geometry.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Shader/UniformBuffer.hpp"

using namespace Castor;

namespace Castor3D
{
	SkinningRenderNode::SkinningRenderNode( RenderPipeline & p_pipeline
		, PassRenderNode && p_pass
		, UniformBuffer & p_modelMatrixBuffer
		, UniformBuffer & p_modelBuffer
		, GeometryBuffers & p_buffers
		, SceneNode & p_sceneNode
		, Submesh & p_data
		, Geometry & p_instance
		, AnimatedSkeleton & p_skeleton
		, UniformBuffer & p_skinningUbo )
		: SubmeshRenderNode{ p_pipeline
			, std::move( p_pass )
			, p_modelMatrixBuffer
			, p_modelBuffer
			, p_buffers
			, p_sceneNode
			, p_data
			, p_instance }
		, m_skeleton{ p_skeleton }
		, m_skinningUbo{ p_skinningUbo }
		, m_bonesMatrix{ *p_skinningUbo.GetUniform< UniformType::eMat4x4f >( ShaderProgram::Bones ) }
	{
	}
}
