template <> inline void Castor3D::ApplyVariable<int>( int p_iGLIndex, const int * p_pValue, size_t p_uiOcc)
{
	glUniform1iv( p_iGLIndex, p_uiOcc, p_pValue);
	CheckGLError( CU_T( "ApplyVariable - glUniform1i"));
}

template <> inline void Castor3D::ApplyVariable<int, 1>( int p_iGLIndex, const int * p_pValue, size_t p_uiOcc)
{
	glUniform1iv( p_iGLIndex, p_uiOcc, p_pValue);
	CheckGLError( CU_T( "ApplyVariable - glUniform1i"));
}

template <> inline void Castor3D::ApplyVariable<int, 2>( int p_iGLIndex, const int * p_pValue, size_t p_uiOcc)
{
	glUniform2iv( p_iGLIndex, p_uiOcc, p_pValue);
	CheckGLError( CU_T( "ApplyVariable - glUniform2iv"));
}

template <> inline void Castor3D::ApplyVariable<int, 3>( int p_iGLIndex, const int * p_pValue, size_t p_uiOcc)
{
	glUniform3iv( p_iGLIndex, p_uiOcc, p_pValue);
	CheckGLError( CU_T( "ApplyVariable - glUniform3iv"));
}

template <> inline void Castor3D::ApplyVariable<int, 4>( int p_iGLIndex, const int * p_pValue, size_t p_uiOcc)
{
	glUniform4iv( p_iGLIndex, p_uiOcc, p_pValue);
	CheckGLError( CU_T( "ApplyVariable - glUniform4iv"));
}

template <> inline void Castor3D::ApplyVariable<unsigned int>( int p_iGLIndex, const unsigned int * p_pValue, size_t p_uiOcc)
{
	glUniform1uiv( p_iGLIndex, p_uiOcc, p_pValue);
	CheckGLError( CU_T( "ApplyVariable - glUniform1i"));
}

template <> inline void Castor3D::ApplyVariable<unsigned int, 1>( int p_iGLIndex, const unsigned int * p_pValue, size_t p_uiOcc)
{
	glUniform1uiv( p_iGLIndex, p_uiOcc, p_pValue);
	CheckGLError( CU_T( "ApplyVariable - glUniform1i"));
}

template <> inline void Castor3D::ApplyVariable<unsigned int, 2>( int p_iGLIndex, const unsigned int * p_pValue, size_t p_uiOcc)
{
	glUniform2uiv( p_iGLIndex, p_uiOcc, p_pValue);
	CheckGLError( CU_T( "ApplyVariable - glUniform2iv"));
}

template <> inline void Castor3D::ApplyVariable<unsigned int, 3>( int p_iGLIndex, const unsigned int * p_pValue, size_t p_uiOcc)
{
	glUniform3uiv( p_iGLIndex, p_uiOcc, p_pValue);
	CheckGLError( CU_T( "ApplyVariable - glUniform3iv"));
}

template <> inline void Castor3D::ApplyVariable<unsigned int, 4>( int p_iGLIndex, const unsigned int * p_pValue, size_t p_uiOcc)
{
	glUniform4uiv( p_iGLIndex, p_uiOcc, p_pValue);
	CheckGLError( CU_T( "ApplyVariable - glUniform4iv"));
}

template <> inline void Castor3D::ApplyVariable<float>( int p_iGLIndex, const float * p_pValue, size_t p_uiOcc)
{
	glUniform1fv( p_iGLIndex, p_uiOcc, p_pValue);
	CheckGLError( CU_T( "ApplyVariable - glUniform1r"));
}

template <> inline void Castor3D::ApplyVariable<float, 1>( int p_iGLIndex, const float * p_pValue, size_t p_uiOcc)
{
	glUniform1fv( p_iGLIndex, p_uiOcc, p_pValue);
	CheckGLError( CU_T( "ApplyVariable - glUniform1r"));
}

template <> inline void Castor3D::ApplyVariable<float, 2>( int p_iGLIndex, const float * p_pValue, size_t p_uiOcc)
{
	glUniform2fv( p_iGLIndex, p_uiOcc, p_pValue);
	CheckGLError( CU_T( "ApplyVariable - glUniform2fv"));
}

template <> inline void Castor3D::ApplyVariable<float, 3>( int p_iGLIndex, const float * p_pValue, size_t p_uiOcc)
{
	glUniform3fv( p_iGLIndex, p_uiOcc, p_pValue);
	CheckGLError( CU_T( "ApplyVariable - glUniform3fv"));
}

template <> inline void Castor3D::ApplyVariable<float, 4>( int p_iGLIndex, const float * p_pValue, size_t p_uiOcc)
{
	glUniform4fv( p_iGLIndex, p_uiOcc, p_pValue);
	CheckGLError( CU_T( "ApplyVariable - glUniform4fv"));
}

template <> inline void Castor3D::ApplyVariable<float, 2, 2>( int p_iGLIndex, const float * p_pValue, size_t p_uiOcc)
{
	glUniformMatrix2fv( p_iGLIndex, 4 * p_uiOcc, false, p_pValue);
	CheckGLError( CU_T( "ApplyVariable - glUniformMatrix2fv"));
}

template <> inline void Castor3D::ApplyVariable<float, 2, 3>( int p_iGLIndex, const float * p_pValue, size_t p_uiOcc)
{
	glUniformMatrix2x3fv( p_iGLIndex, 9 * p_uiOcc, false, p_pValue);
	CheckGLError( CU_T( "ApplyVariable - glUniformMatrix2x3fv"));
}

template <> inline void Castor3D::ApplyVariable<float, 2, 4>( int p_iGLIndex, const float * p_pValue, size_t p_uiOcc)
{
	glUniformMatrix2x4fv( p_iGLIndex, 16 * p_uiOcc, false, p_pValue);
	CheckGLError( CU_T( "ApplyVariable - glUniformMatrix2x4fv"));
}

template <> inline void Castor3D::ApplyVariable<float, 3, 2>( int p_iGLIndex, const float * p_pValue, size_t p_uiOcc)
{
	glUniformMatrix3x2fv( p_iGLIndex, 4 * p_uiOcc, false, p_pValue);
	CheckGLError( CU_T( "ApplyVariable - glUniformMatrix3x2fv"));
}

template <> inline void Castor3D::ApplyVariable<float, 3, 3>( int p_iGLIndex, const float * p_pValue, size_t p_uiOcc)
{
	glUniformMatrix3fv( p_iGLIndex, 9 * p_uiOcc, false, p_pValue);
	CheckGLError( CU_T( "ApplyVariable - glUniformMatrix3fv"));
}

template <> inline void Castor3D::ApplyVariable<float, 3, 4>( int p_iGLIndex, const float * p_pValue, size_t p_uiOcc)
{
	glUniformMatrix3x4fv( p_iGLIndex, 16 * p_uiOcc, false, p_pValue);
	CheckGLError( CU_T( "ApplyVariable - glUniformMatrix3x4fv"));
}

template <> inline void Castor3D::ApplyVariable<float, 4, 2>( int p_iGLIndex, const float * p_pValue, size_t p_uiOcc)
{
	glUniformMatrix4x2fv( p_iGLIndex, 4 * p_uiOcc, false, p_pValue);
	CheckGLError( CU_T( "ApplyVariable - glUniformMatrix4x2fv"));
}

template <> inline void Castor3D::ApplyVariable<float, 4, 3>( int p_iGLIndex, const float * p_pValue, size_t p_uiOcc)
{
	glUniformMatrix4x3fv( p_iGLIndex, 9 * p_uiOcc, false, p_pValue);
	CheckGLError( CU_T( "ApplyVariable - glUniformMatrix4x3fv"));
}

template <> inline void Castor3D::ApplyVariable<float, 4, 4>( int p_iGLIndex, const float * p_pValue, size_t p_uiOcc)
{
	glUniformMatrix4fv( p_iGLIndex, 16 * p_uiOcc, false, p_pValue);
	CheckGLError( CU_T( "ApplyVariable - glUniformMatrix4fv"));
}

//***********************************************************************************************************************

template <typename Type> 
inline void Castor3D::GLFrameVariableBase :: _apply( const String & p_strName, const Type * p_pValue, size_t p_uiOcc)
{
	if (m_bPresentInProgram)
	{
		if (m_glIndex == GL_INVALID_INDEX)
		{
			GetVariableLocation( p_strName.char_str());
		}
		if (m_glIndex != GL_INVALID_INDEX) 
		{
			ApplyVariable<Type>( m_glIndex, p_pValue, p_uiOcc);
		}
		else
		{
			m_bPresentInProgram = false;
		}
	}
}
template <typename Type, int Count>
inline void Castor3D::GLFrameVariableBase :: _apply( const String & p_strName, const Type * p_pValue, size_t p_uiOcc)
{
	if (m_bPresentInProgram)
	{
		if (m_glIndex == GL_INVALID_INDEX)
		{
			GetVariableLocation( p_strName.char_str());
		}
		if (m_glIndex != GL_INVALID_INDEX) 
		{
			ApplyVariable<Type, Count>( m_glIndex, p_pValue, p_uiOcc);
		}
		else
		{
			m_bPresentInProgram = false;
		}
	}
}
template <typename Type, int Rows, int Columns>
inline void Castor3D::GLFrameVariableBase :: _apply( const String & p_strName, const Type * p_pValue, size_t p_uiOcc)
{
	if (m_bPresentInProgram)
	{
		if (m_glIndex == GL_INVALID_INDEX)
		{
			GetVariableLocation( p_strName.char_str());
		}
		if (m_glIndex != GL_INVALID_INDEX) 
		{
			ApplyVariable<Type, Rows, Columns>( m_glIndex, p_pValue, p_uiOcc);
		}
		else
		{
			m_bPresentInProgram = false;
		}
	}
}

//***********************************************************************************************************************