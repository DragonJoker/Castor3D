#include "OpenGl.hpp"
#include "GlShaderProgram.hpp"

namespace GlRender
{
	template< typename T >
	GlOneFrameVariable< T >::GlOneFrameVariable( OpenGl & p_gl, uint32_t p_uiOcc, GlShaderProgram * p_pProgram )
		: Castor3D::OneFrameVariable< T >( p_pProgram, p_uiOcc )
		, GlFrameVariableBase( p_gl, &p_pProgram->GetGlProgram() )
	{
	}

	template< typename T >
	GlOneFrameVariable< T >::GlOneFrameVariable( OpenGl & p_gl, Castor3D::OneFrameVariable< T > * p_pVariable )
		: Castor3D::OneFrameVariable< T >( *p_pVariable )
		, GlFrameVariableBase( p_gl, &static_cast< GlShaderProgram * >( &p_pVariable->GetProgram() )->GetGlProgram() )
	{
	}

	template< typename T >
	GlOneFrameVariable< T >::~GlOneFrameVariable()
	{
	}

	template< typename T >
	bool GlOneFrameVariable< T >::Initialise()
	{
		if ( m_iGlIndex == eGL_INVALID_INDEX )
		{
			GetVariableLocation( Castor::str_utils::to_str( Castor3D::FrameVariable::m_strName ).c_str() );
		}

		return m_iGlIndex != eGL_INVALID_INDEX;
	}

	template< typename T >
	void GlOneFrameVariable< T >::Cleanup()
	{
		m_iGlIndex = eGL_INVALID_INDEX;
	}

	template< typename T >
	void GlOneFrameVariable< T >::Bind()
	{
		if ( Castor3D::FrameVariable::m_bChanged )
		{
			GlFrameVariableBase::DoBind< T >( Castor3D::OneFrameVariable< T >::m_pValues, Castor3D::FrameVariable::m_uiOcc );
			Castor3D::FrameVariable::m_bChanged = false;
		}
	}
}
