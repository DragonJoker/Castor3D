#include "MatrixUbo.hpp"

#include "Engine.hpp"
#include "Render/RenderPipeline.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Texture/TextureLayout.hpp"

using namespace castor;

namespace castor3d
{
	MatrixUbo::MatrixUbo( Engine & engine )
		: m_ubo{ ShaderProgram::BufferMatrix
			, *engine.getRenderSystem()
			, MatrixUbo ::BindingPoint }
		, m_view{ *m_ubo.createUniform< UniformType::eMat4x4r >( RenderPipeline::MtxView ) }
		, m_projection{ *m_ubo.createUniform< UniformType::eMat4x4r >( RenderPipeline::MtxProjection ) }
		, m_invProjection{ *m_ubo.createUniform< UniformType::eMat4x4r >( RenderPipeline::MtxInvProjection ) }
	{
	}

	MatrixUbo::~MatrixUbo()
	{
	}

	void MatrixUbo::update( Matrix4x4r const & p_view
		, Matrix4x4r const & p_projection )const
	{
		m_view.setValue( p_view );
		m_projection.setValue( p_projection );
		m_invProjection.setValue( p_projection.getInverse() );
		m_ubo.update();
		m_ubo.bindTo( MatrixUbo::BindingPoint );
	}

	void MatrixUbo::update( Matrix4x4r const & p_projection )const
	{
		m_projection.setValue( p_projection );
		m_invProjection.setValue( p_projection.getInverse() );
		m_ubo.update();
		m_ubo.bindTo( MatrixUbo::BindingPoint );
	}
}
