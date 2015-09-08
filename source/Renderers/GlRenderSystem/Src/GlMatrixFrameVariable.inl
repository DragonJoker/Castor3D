#include "OpenGl.hpp"
#include "GlShaderProgram.hpp"

namespace GlRender
{
	template< typename T, uint32_t Rows, uint32_t Columns >
	GlMatrixFrameVariable<T, Rows, Columns>::GlMatrixFrameVariable( OpenGl & p_gl, uint32_t p_uiOcc, GlShaderProgram * p_pProgram )
		: Castor3D::MatrixFrameVariable< T, Rows, Columns >( p_pProgram, p_uiOcc )
		, GlFrameVariableBase( p_gl, &p_pProgram->GetGlProgram() )
	{
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	GlMatrixFrameVariable<T, Rows, Columns>::GlMatrixFrameVariable( OpenGl & p_gl, Castor3D::MatrixFrameVariable<T, Rows, Columns> * p_pVariable )
		: Castor3D::MatrixFrameVariable<T, Rows, Columns>( *p_pVariable )
		, GlFrameVariableBase( p_gl, &static_cast< GlShaderProgram * >( p_pVariable->GetProgram() )->GetGlProgram() )
	{
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	GlMatrixFrameVariable<T, Rows, Columns>::~GlMatrixFrameVariable()
	{
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	bool GlMatrixFrameVariable<T, Rows, Columns>::Initialise()
	{
		if ( m_iGlIndex == eGL_INVALID_INDEX )
		{
			GetVariableLocation( Castor::string::to_str( Castor3D::FrameVariable::m_strName ).c_str() );
		}

		return m_iGlIndex != eGL_INVALID_INDEX;
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	void GlMatrixFrameVariable<T, Rows, Columns>::Cleanup()
	{
		m_iGlIndex = eGL_INVALID_INDEX;
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	void GlMatrixFrameVariable<T, Rows, Columns>::Bind()
	{
		if ( Castor3D::FrameVariable::m_bChanged )
		{
			GlFrameVariableBase::DoBind< T, Rows, Columns >( Castor3D::MatrixFrameVariable<T, Rows, Columns>::m_pValues, Castor3D::FrameVariable::m_uiOcc );
			Castor3D::FrameVariable::m_bChanged = false;
		}
	}
}
