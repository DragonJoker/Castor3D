#include "Common/OpenGl.hpp"
#include "Shader/GlShaderProgram.hpp"

namespace GlRender
{
	template< typename T, uint32_t Rows, uint32_t Columns >
	GlMatrixFrameVariable< T, Rows, Columns >::GlMatrixFrameVariable( OpenGl & p_gl, uint32_t p_occurences, GlShaderProgram & p_program )
		: Castor3D::MatrixFrameVariable< T, Rows, Columns >( p_program, p_occurences )
		, GlFrameVariableBase( p_gl, p_program )
	{
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	GlMatrixFrameVariable< T, Rows, Columns >::GlMatrixFrameVariable( OpenGl & p_gl, Castor3D::MatrixFrameVariable< T, Rows, Columns > & p_variable )
		: Castor3D::MatrixFrameVariable< T, Rows, Columns >( p_variable )
		, GlFrameVariableBase( p_gl, static_cast< GlShaderProgram & >( p_variable->GetProgram() ) )
	{
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	GlMatrixFrameVariable< T, Rows, Columns >::~GlMatrixFrameVariable()
	{
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	bool GlMatrixFrameVariable< T, Rows, Columns >::Initialise()
	{
		if ( m_glName == GlInvalidIndex )
		{
			GetVariableLocation( Castor::string::string_cast< char >( Castor3D::FrameVariable::m_name ).c_str() );
		}

		return m_glName != GlInvalidIndex;
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	void GlMatrixFrameVariable< T, Rows, Columns >::Cleanup()
	{
		m_glName = GlInvalidIndex;
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	void GlMatrixFrameVariable< T, Rows, Columns >::Bind()const
	{
		if ( this->IsChanged() )
		{
			GlFrameVariableBase::DoBind< T, Rows, Columns >( Castor3D::MatrixFrameVariable< T, Rows, Columns >::m_values, Castor3D::FrameVariable::m_occurences );
			Castor3D::FrameVariable::m_changed = false;
		}
	}
}
