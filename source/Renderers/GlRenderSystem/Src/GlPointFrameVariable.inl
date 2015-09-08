#include "OpenGl.hpp"
#include "GlShaderProgram.hpp"

namespace GlRender
{
	template< typename T, uint32_t Count >
	GlPointFrameVariable<T, Count>::GlPointFrameVariable( OpenGl & p_gl, uint32_t p_uiOcc, GlShaderProgram * p_pProgram )
		: Castor3D::PointFrameVariable< T, Count >( p_pProgram, p_uiOcc )
		, GlFrameVariableBase( p_gl, &p_pProgram->GetGlProgram() )
	{
	}

	template< typename T, uint32_t Count >
	GlPointFrameVariable<T, Count>::GlPointFrameVariable( OpenGl & p_gl, Castor3D::PointFrameVariable<T, Count> * p_pVariable )
		: Castor3D::PointFrameVariable< T, Count >( *p_pVariable )
		, GlFrameVariableBase( p_gl, &static_cast< GlShaderProgram * >( &p_pVariable->GetProgram() )->GetGlProgram() )
	{
	}

	template< typename T, uint32_t Count >
	GlPointFrameVariable<T, Count>::~GlPointFrameVariable()
	{
	}

	template< typename T, uint32_t Count >
	bool GlPointFrameVariable<T, Count>::Initialise()
	{
		if ( m_iGlIndex == eGL_INVALID_INDEX )
		{
			GetVariableLocation( Castor::string::to_str( Castor3D::FrameVariable::m_strName ).c_str() );
		}

		return m_iGlIndex != eGL_INVALID_INDEX;
	}

	template< typename T, uint32_t Count >
	void GlPointFrameVariable<T, Count>::Cleanup()
	{
		m_iGlIndex = eGL_INVALID_INDEX;
	}

	template< typename T, uint32_t Count >
	void GlPointFrameVariable<T, Count>::Bind()
	{
		if ( Castor3D::FrameVariable::m_bChanged )
		{
			GlFrameVariableBase::DoBind< T, Count >( Castor3D::PointFrameVariable<T, Count>::m_pValues, Castor3D::FrameVariable::m_uiOcc );
			Castor3D::FrameVariable::m_bChanged = false;
		}
	}
}
