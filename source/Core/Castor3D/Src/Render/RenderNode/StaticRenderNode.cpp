#include "StaticRenderNode.hpp"

#include "Mesh/Submesh.hpp"
#include "Render/RenderPipeline.hpp"
#include "Scene/SceneNode.hpp"
#include "Scene/Geometry.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Shader/UniformBuffer.hpp"

using namespace Castor;

namespace Castor3D
{
	StaticRenderNode::StaticRenderNode( RenderPipeline & p_pipeline
		, PassRenderNode && p_pass
		, UniformBuffer & p_modelMatrixBuffer
		, UniformBuffer & p_modelBuffer
		, GeometryBuffers & p_buffers
		, SceneNode & p_sceneNode
		, Submesh & p_data
		, Geometry & p_instance )
		: SubmeshRenderNode{ p_pipeline
			, std::move( p_pass )
			, p_modelMatrixBuffer
			, p_modelBuffer
			, p_buffers
			, p_sceneNode
			, p_data
			, p_instance }
	{
	}

	void StaticRenderNode::Render()
	{
		auto & l_model = m_sceneNode.GetDerivedTransformationMatrix();
		auto & l_view = m_pipeline.GetViewMatrix();
		m_shadowReceiver.SetValue( m_instance.IsShadowReceiver() ? 1 : 0 );
		m_modelMatrix.SetValue( l_model );
		m_normalMatrix.SetValue( Matrix4x4r{ ( l_model * l_view ).get_minor( 3, 3 ).invert().get_transposed() } );
		m_modelUbo.Update();
		m_modelMatrixUbo.Update();
		m_data.Draw( m_buffers );
	}
}
