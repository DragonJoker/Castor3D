#include "MorphingRenderNode.hpp"

#include "Mesh/Submesh.hpp"
#include "Render/RenderPipeline.hpp"
#include "Scene/SceneNode.hpp"
#include "Scene/Geometry.hpp"
#include "Scene/Animation/AnimatedMesh.hpp"
#include "Scene/Animation/Mesh/MeshAnimationInstance.hpp"
#include "Scene/Animation/Mesh/MeshAnimationInstanceSubmesh.hpp"
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

	void MorphingRenderNode::Render()
	{
		auto & l_model = m_sceneNode.GetDerivedTransformationMatrix();
		auto & l_view = m_pipeline.GetViewMatrix();
		m_shadowReceiver.SetValue( m_instance.IsShadowReceiver() ? 1 : 0 );
		m_modelMatrix.SetValue( l_model );
		m_normalMatrix.SetValue( Matrix4x4r{ ( l_model * l_view ).get_minor( 3, 3 ).invert().get_transposed() } );

		if ( m_mesh.IsPlayingAnimation() )
		{
			auto l_submesh = m_mesh.GetPlayingAnimation().GetAnimationSubmesh( m_data.GetId() );

			if ( l_submesh )
			{
				l_submesh->FillShader( m_time );
			}
		}
		else
		{
			m_time.SetValue( 1.0f );
		}

		m_modelUbo.Update();
		m_modelMatrixUbo.Update();
		m_morphingUbo.Update();
		m_data.Draw( m_buffers );
	}
}
