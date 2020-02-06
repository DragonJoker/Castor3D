#include "ObjectRenderNode.hpp"

#include "Render/RenderPipeline.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Shader/UniformBuffer.hpp"

namespace Castor3D
{
	template<>
	ObjectRenderNode< Submesh, Geometry >::ObjectRenderNode( RenderPipeline & p_pipeline
		, PassRenderNode && p_pass
		, UniformBuffer & p_modelMatrixBuffer
		, UniformBuffer & p_modelBuffer
		, GeometryBuffers & p_buffers
		, SceneNode & p_sceneNode
		, Submesh & p_data
		, Geometry & p_instance )
		: m_pipeline{ p_pipeline }
		, m_passNode{ std::move( p_pass ) }
		, m_modelMatrixUbo{ p_modelMatrixBuffer }
		, m_modelMatrix{ *p_modelMatrixBuffer.GetUniform< UniformType::eMat4x4r >( RenderPipeline::MtxModel ) }
		, m_normalMatrix{ *p_modelMatrixBuffer.GetUniform< UniformType::eMat4x4r >( RenderPipeline::MtxNormal ) }
		, m_modelUbo{ p_modelBuffer }
		, m_shadowReceiver{ *p_modelBuffer.GetUniform< UniformType::eInt >( ShaderProgram::ShadowReceiver ) }
		, m_buffers{ p_buffers }
		, m_sceneNode{ p_sceneNode }
		, m_data{ p_data }
		, m_instance{ p_instance }
	{
	}

	template<>
	ObjectRenderNode< BillboardBase, BillboardBase >::ObjectRenderNode( RenderPipeline & p_pipeline
		, PassRenderNode && p_pass
		, UniformBuffer & p_modelMatrixBuffer
		, UniformBuffer & p_modelBuffer
		, GeometryBuffers & p_buffers
		, SceneNode & p_sceneNode
		, BillboardBase & p_data
		, BillboardBase & p_instance )
		: m_pipeline{ p_pipeline }
		, m_passNode{ std::move( p_pass ) }
		, m_modelMatrixUbo{ p_modelMatrixBuffer }
		, m_modelMatrix{ *p_modelMatrixBuffer.GetUniform< UniformType::eMat4x4r >( RenderPipeline::MtxModel ) }
		, m_normalMatrix{ *p_modelMatrixBuffer.GetUniform< UniformType::eMat4x4r >( RenderPipeline::MtxNormal ) }
		, m_modelUbo{ p_modelBuffer }
		, m_shadowReceiver{ *p_modelBuffer.GetUniform< UniformType::eInt >( ShaderProgram::ShadowReceiver ) }
		, m_buffers{ p_buffers }
		, m_sceneNode{ p_sceneNode }
		, m_data{ p_data }
		, m_instance{ p_instance }
	{
	}
}
