#include "Common/OpenGl.hpp"
#include "Shader/GlShaderProgram.hpp"

namespace GlRender
{
	template< typename T, uint32_t Rows, uint32_t Columns >
	GlMatrixUniform< T, Rows, Columns >::GlMatrixUniform( OpenGl & p_gl, uint32_t p_occurences, GlShaderProgram & p_program )
		: Castor3D::MatrixUniform< T, Rows, Columns >( p_program, p_occurences )
		, GlUniformBase( p_gl, p_program )
	{
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	GlMatrixUniform< T, Rows, Columns >::GlMatrixUniform( OpenGl & p_gl, Castor3D::MatrixUniform< T, Rows, Columns > & p_variable )
		: Castor3D::MatrixUniform< T, Rows, Columns >( p_variable )
		, GlUniformBase( p_gl, static_cast< GlShaderProgram & >( p_variable->GetProgram() ) )
	{
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	GlMatrixUniform< T, Rows, Columns >::~GlMatrixUniform()
	{
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	bool GlMatrixUniform< T, Rows, Columns >::Initialise()
	{
		if ( m_glName == GlInvalidIndex )
		{
			GetVariableLocation( Castor::string::string_cast< char >( Castor3D::Uniform::m_name ).c_str() );
		}

		return m_glName != GlInvalidIndex;
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	void GlMatrixUniform< T, Rows, Columns >::Cleanup()
	{
		m_glName = GlInvalidIndex;
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	void GlMatrixUniform< T, Rows, Columns >::Bind()const
	{
		GlUniformBase::DoBind< T, Rows, Columns >( Castor3D::MatrixUniform< T, Rows, Columns >::m_values, Castor3D::MatrixUniform< T, Rows, Columns >::m_occurences );
		Castor3D::Uniform::m_changed = false;
	}
}
