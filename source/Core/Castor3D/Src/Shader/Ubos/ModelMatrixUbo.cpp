#include "ModelMatrixUbo.hpp"

#include "Engine.hpp"
#include "Render/RenderPipeline.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Texture/TextureLayout.hpp"

using namespace castor;

namespace castor3d
{
	String const ModelMatrixUbo::BufferModelMatrix = cuT( "ModelMatrices" );
	String const ModelMatrixUbo::MtxModel = cuT( "c3d_mtxModel" );
	String const ModelMatrixUbo::MtxNormal = cuT( "c3d_mtxNormal" );

	ModelMatrixUbo::ModelMatrixUbo( Engine & engine )
		: m_ubo{ ModelMatrixUbo::BufferModelMatrix
			, *engine.getRenderSystem()
			, ModelMatrixUbo::BindingPoint }
		, m_model{ *m_ubo.createUniform< UniformType::eMat4x4r >( ModelMatrixUbo::MtxModel ) }
		, m_normal{ *m_ubo.createUniform< UniformType::eMat4x4r >( ModelMatrixUbo::MtxNormal ) }
	{
	}

	ModelMatrixUbo::~ModelMatrixUbo()
	{
	}

	void ModelMatrixUbo::update( castor::Matrix4x4r const & p_model  )const
	{
		m_model.setValue( p_model );
		m_ubo.update();
		m_ubo.bindTo( ModelMatrixUbo::BindingPoint );
	}

	void ModelMatrixUbo::update( castor::Matrix4x4r const & p_model
		, castor::Matrix4x4r const & p_normal )const
	{
		m_normal.setValue( p_normal );
		update( p_model );
	}
}
