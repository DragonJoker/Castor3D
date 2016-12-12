#include "Common/OpenGl.hpp"
#include "Shader/GlShaderProgram.hpp"

namespace GlRender
{
	template< typename T, uint32_t Count >
	GlPointUniform< T, Count >::GlPointUniform( OpenGl & p_gl, uint32_t p_occurences, GlShaderProgram & p_program )
		: Castor3D::PointUniform< T, Count >( p_program, p_occurences )
		, GlUniformBase( p_gl, p_program )
	{
	}

	template< typename T, uint32_t Count >
	GlPointUniform< T, Count >::GlPointUniform( OpenGl & p_gl, Castor3D::PointUniform< T, Count > & p_variable )
		: Castor3D::PointUniform< T, Count >( p_variable )
		, GlUniformBase( p_gl, static_cast< GlShaderProgram & >( p_variable->GetProgram() ) )
	{
	}

	template< typename T, uint32_t Count >
	GlPointUniform< T, Count >::~GlPointUniform()
	{
	}

	template< typename T, uint32_t Count >
	bool GlPointUniform< T, Count >::Initialise()
	{
		if ( m_glName == GlInvalidIndex )
		{
			GetVariableLocation( Castor::string::string_cast< char >( Castor3D::Uniform::m_name ).c_str() );
		}

		return m_glName != GlInvalidIndex;
	}

	template< typename T, uint32_t Count >
	void GlPointUniform< T, Count >::Cleanup()
	{
		m_glName = GlInvalidIndex;
	}

	template< typename T, uint32_t Count >
	void GlPointUniform< T, Count >::Bind()const
	{
		GlUniformBase::DoBind< T, Count >( Castor3D::PointUniform< T, Count >::m_values, Castor3D::PointUniform< T, Count >::m_occurences );
		Castor3D::Uniform::m_changed = false;
	}
}
