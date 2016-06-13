#include "Common/OpenGl.hpp"
#include "Shader/GlShaderProgram.hpp"

namespace GlRender
{
	template< typename T, uint32_t Count >
	GlPointFrameVariable<T, Count>::GlPointFrameVariable( OpenGl & p_gl, uint32_t p_occurences, GlShaderProgram * p_program )
		: Castor3D::PointFrameVariable< T, Count >( p_program, p_occurences )
		, GlFrameVariableBase( p_gl, &p_program->GetGlName() )
	{
	}

	template< typename T, uint32_t Count >
	GlPointFrameVariable<T, Count>::GlPointFrameVariable( OpenGl & p_gl, Castor3D::PointFrameVariable<T, Count> * p_variable )
		: Castor3D::PointFrameVariable< T, Count >( *p_variable )
		, GlFrameVariableBase( p_gl, &static_cast< GlShaderProgram * >( &p_variable->GetProgram() )->GetGlName() )
	{
	}

	template< typename T, uint32_t Count >
	GlPointFrameVariable<T, Count>::~GlPointFrameVariable()
	{
	}

	template< typename T, uint32_t Count >
	bool GlPointFrameVariable<T, Count>::Initialise()
	{
		if ( m_glName == eGL_INVALID_INDEX )
		{
			GetVariableLocation( Castor::string::string_cast< char >( Castor3D::FrameVariable::m_name ).c_str() );
		}

		return m_glName != eGL_INVALID_INDEX;
	}

	template< typename T, uint32_t Count >
	void GlPointFrameVariable<T, Count>::Cleanup()
	{
		m_glName = eGL_INVALID_INDEX;
	}

	template< typename T, uint32_t Count >
	void GlPointFrameVariable<T, Count>::Bind()
	{
		if ( Castor3D::FrameVariable::m_changed )
		{
			GlFrameVariableBase::DoBind< T, Count >( Castor3D::PointFrameVariable<T, Count>::m_values, Castor3D::FrameVariable::m_occurences );
			Castor3D::FrameVariable::m_changed = false;
		}
	}
}
