#include "ModelMatrixUbo.hpp"

#include "Engine.hpp"
#include "Render/RenderPipeline.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Texture/TextureLayout.hpp"

using namespace Castor;

namespace Castor3D
{
	ModelMatrixUbo::ModelMatrixUbo( Engine & engine )
		: m_ubo{ ShaderProgram::BufferModelMatrix
			, *engine.GetRenderSystem()
			, ModelMatrixUbo::BindingPoint }
		, m_model{ *m_ubo.CreateUniform< UniformType::eMat4x4r >( RenderPipeline::MtxModel ) }
		, m_normal{ *m_ubo.CreateUniform< UniformType::eMat4x4r >( RenderPipeline::MtxNormal ) }
	{
	}

	ModelMatrixUbo::~ModelMatrixUbo()
	{
	}

	void ModelMatrixUbo::Update( Castor::Matrix4x4r const & p_model  )const
	{
		m_model.SetValue( p_model );
		m_ubo.Update();
		m_ubo.BindTo( ModelMatrixUbo::BindingPoint );
	}

	void ModelMatrixUbo::Update( Castor::Matrix4x4r const & p_model
		, Castor::Matrix4x4r const & p_normal )const
	{
		m_normal.SetValue( p_normal );
		Update( p_model );
	}
}
