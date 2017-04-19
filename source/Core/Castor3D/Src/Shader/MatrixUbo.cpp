#include "MatrixUbo.hpp"

#include "Engine.hpp"
#include "Render/RenderPipeline.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Texture/TextureLayout.hpp"

using namespace Castor;

namespace Castor3D
{
	MatrixUbo::MatrixUbo( Engine & p_engine )
		: m_ubo{ ShaderProgram::BufferMatrix, *p_engine.GetRenderSystem() }
		, m_view{ *m_ubo.CreateUniform< UniformType::eMat4x4r >( RenderPipeline::MtxView ) }
		, m_projection{ *m_ubo.CreateUniform< UniformType::eMat4x4r >( RenderPipeline::MtxProjection ) }
	{
	}

	MatrixUbo::~MatrixUbo()
	{
	}

	void MatrixUbo::Update( Matrix4x4r const & p_view
		, Matrix4x4r const & p_projection )const
	{
		m_view.SetValue( p_view );
		m_projection.SetValue( p_projection );
		m_ubo.Update();
	}

	void MatrixUbo::Update( Matrix4x4r const & p_projection )const
	{
		m_projection.SetValue( p_projection );
		m_ubo.Update();
	}
}
