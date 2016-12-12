#include "Common/OpenGl.hpp"
#include "Shader/GlShaderProgram.hpp"

namespace GlRender
{
	template< typename T >
	GlOneUniform< T >::GlOneUniform( OpenGl & p_gl, uint32_t p_occurences, GlShaderProgram & p_program )
		: Castor3D::OneUniform< T >( p_program, p_occurences )
		, GlUniformBase( p_gl, p_program )
	{
	}

	template< typename T >
	GlOneUniform< T >::GlOneUniform( OpenGl & p_gl, Castor3D::OneUniform< T > & p_variable )
		: Castor3D::OneUniform< T >( p_variable )
		, GlUniformBase( p_gl, static_cast< GlShaderProgram & >( p_variable->GetProgram() ) )
	{
	}

	template< typename T >
	GlOneUniform< T >::~GlOneUniform()
	{
	}

	template< typename T >
	bool GlOneUniform< T >::Initialise()
	{
		if ( m_glName == GlInvalidIndex )
		{
			GetVariableLocation( Castor::string::string_cast< char >( Castor3D::Uniform::m_name ).c_str() );
		}

		return m_glName != GlInvalidIndex;
	}

	template< typename T >
	void GlOneUniform< T >::Cleanup()
	{
		m_glName = GlInvalidIndex;
	}

	template< typename T >
	void GlOneUniform< T >::Bind()const
	{
		if ( this->IsChanged() )
		{
			GlUniformBase::DoBind< T >( Castor3D::OneUniform< T >::m_values, Castor3D::Uniform::m_occurences );
			Castor3D::Uniform::m_changed = false;
		}
	}
}
