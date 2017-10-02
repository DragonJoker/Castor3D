#include "MatrixUbo.hpp"

#include "Engine.hpp"
#include "Render/RenderPipeline.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Texture/TextureLayout.hpp"

using namespace castor;

namespace castor3d
{
	String const MatrixUbo::BufferMatrix = cuT( "Matrices" );
	String const MatrixUbo::Projection = cuT( "c3d_projection" );
	String const MatrixUbo::CurView = cuT( "c3d_curView" );
	String const MatrixUbo::PrvView = cuT( "c3d_prvView" );
	String const MatrixUbo::InvProjection = cuT( "c3d_invProjection" );
	String const MatrixUbo::CurJitter = cuT( "c3d_curJitter" );
	String const MatrixUbo::PrvJitter = cuT( "c3d_prvJitter" );

	MatrixUbo::MatrixUbo( Engine & engine )
		: m_ubo{ MatrixUbo::BufferMatrix
			, *engine.getRenderSystem()
			, MatrixUbo ::BindingPoint }
		, m_projection{ *m_ubo.createUniform< UniformType::eMat4x4r >( MatrixUbo::Projection ) }
		, m_invProjection{ *m_ubo.createUniform< UniformType::eMat4x4r >( MatrixUbo::InvProjection ) }
		, m_curView{ *m_ubo.createUniform< UniformType::eMat4x4r >( MatrixUbo::CurView ) }
		, m_prvView{ *m_ubo.createUniform< UniformType::eMat4x4r >( MatrixUbo::PrvView ) }
		, m_curJitter{ *m_ubo.createUniform< UniformType::eVec2f >( MatrixUbo::CurJitter ) }
		, m_prvJitter{ *m_ubo.createUniform< UniformType::eVec2f >( MatrixUbo::PrvJitter ) }
	{
	}

	MatrixUbo::~MatrixUbo()
	{
	}

	void MatrixUbo::update( Matrix4x4r const & view
		, Matrix4x4r const & projection
		, Point2r const & jitter )const
	{
		m_prvView.setValue( m_curView.getValue() );
		m_prvJitter.setValue( m_curJitter.getValue() );
		m_curView.setValue( view );
		m_projection.setValue( projection );
		m_invProjection.setValue( projection.getInverse() );
		m_curJitter.setValue( jitter );
		m_ubo.update();
		m_ubo.bindTo( MatrixUbo::BindingPoint );
	}

	void MatrixUbo::update( Matrix4x4r const & projection )const
	{
		m_projection.setValue( projection );
		m_invProjection.setValue( projection.getInverse() );
		m_ubo.update();
		m_ubo.bindTo( MatrixUbo::BindingPoint );
	}
}
