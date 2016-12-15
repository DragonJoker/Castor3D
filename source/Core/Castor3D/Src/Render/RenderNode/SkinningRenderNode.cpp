#include "SkinningRenderNode.hpp"

#include "Mesh/Submesh.hpp"
#include "Render/RenderPipeline.hpp"
#include "Scene/SceneNode.hpp"
#include "Scene/Geometry.hpp"
#include "Scene/Animation/AnimatedSkeleton.hpp"
#include "Scene/Animation/Skeleton/SkeletonAnimationInstance.hpp"
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

	void SkinningRenderNode::Render()
	{
		auto & l_model = m_sceneNode.GetDerivedTransformationMatrix();
		auto & l_view = m_pipeline.GetViewMatrix();
		m_shadowReceiver.SetValue( m_instance.IsShadowReceiver() ? 1 : 0 );
		m_modelMatrix.SetValue( l_model );
		m_normalMatrix.SetValue( Matrix4x4r{ ( l_model * l_view ).get_minor( 3, 3 ).invert().get_transposed() } );
		m_skeleton.FillShader( m_bonesMatrix );
		m_skinningUbo.Update();
		m_modelUbo.Update();
		m_modelMatrixUbo.Update();
		m_skinningUbo.Update();
		m_data.Draw( m_buffers );
	}
}
