#include "OpenGl.hpp"
#include "GlShaderProgram.hpp"

namespace GlRender
{
	template< typename T >
	GlOneFrameVariable< T >::GlOneFrameVariable( OpenGl & p_gl, uint32_t p_occurences, GlShaderProgram * p_program )
		: Castor3D::OneFrameVariable< T >( p_program, p_occurences )
		, GlFrameVariableBase( p_gl, &p_program->GetGlName() )
	{
	}

	template< typename T >
	GlOneFrameVariable< T >::GlOneFrameVariable( OpenGl & p_gl, Castor3D::OneFrameVariable< T > * p_variable )
		: Castor3D::OneFrameVariable< T >( *p_variable )
		, GlFrameVariableBase( p_gl, &static_cast< GlShaderProgram * >( &p_variable->GetProgram() )->GetGlName() )
	{
	}

	template< typename T >
	GlOneFrameVariable< T >::~GlOneFrameVariable()
	{
	}

	template< typename T >
	bool GlOneFrameVariable< T >::Initialise()
	{
		if ( m_glName == eGL_INVALID_INDEX )
		{
			GetVariableLocation( Castor::string::string_cast< char >( Castor3D::FrameVariable::m_name ).c_str() );
		}

		return m_glName != eGL_INVALID_INDEX;
	}

	template< typename T >
	void GlOneFrameVariable< T >::Cleanup()
	{
		m_glName = eGL_INVALID_INDEX;
	}

	template< typename T >
	void GlOneFrameVariable< T >::Bind()
	{
		if ( Castor3D::FrameVariable::m_changed )
		{
			GlFrameVariableBase::DoBind< T >( Castor3D::OneFrameVariable< T >::m_values, Castor3D::FrameVariable::m_occurences );
			Castor3D::FrameVariable::m_changed = false;
		}
	}
}
