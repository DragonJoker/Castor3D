template <> inline void ApplyVariable<int>( int p_iGlIndex, const int * p_pValue, size_t p_uiOcc)
{
	CheckGlError( glUniform1iv( p_iGlIndex, p_uiOcc, p_pValue), CU_T( "ApplyVariable - glUniform1i"));
}

template <> inline void ApplyVariable<int, 1>( int p_iGlIndex, const int * p_pValue, size_t p_uiOcc)
{
	CheckGlError( glUniform1iv( p_iGlIndex, p_uiOcc, p_pValue), CU_T( "ApplyVariable - glUniform1i"));
}

template <> inline void ApplyVariable<int, 2>( int p_iGlIndex, const int * p_pValue, size_t p_uiOcc)
{
	CheckGlError( glUniform2iv( p_iGlIndex, p_uiOcc, p_pValue), CU_T( "ApplyVariable - glUniform2iv"));
}

template <> inline void ApplyVariable<int, 3>( int p_iGlIndex, const int * p_pValue, size_t p_uiOcc)
{
	CheckGlError( glUniform3iv( p_iGlIndex, p_uiOcc, p_pValue), CU_T( "ApplyVariable - glUniform3iv"));
}

template <> inline void ApplyVariable<int, 4>( int p_iGlIndex, const int * p_pValue, size_t p_uiOcc)
{
	CheckGlError( glUniform4iv( p_iGlIndex, p_uiOcc, p_pValue), CU_T( "ApplyVariable - glUniform4iv"));
}

template <> inline void ApplyVariable<unsigned int>( int p_iGlIndex, const unsigned int * p_pValue, size_t p_uiOcc)
{
	CheckGlError( glUniform1uiv( p_iGlIndex, p_uiOcc, p_pValue), CU_T( "ApplyVariable - glUniform1i"));
}

template <> inline void ApplyVariable<unsigned int, 1>( int p_iGlIndex, const unsigned int * p_pValue, size_t p_uiOcc)
{
	CheckGlError( glUniform1uiv( p_iGlIndex, p_uiOcc, p_pValue), CU_T( "ApplyVariable - glUniform1i"));
}

template <> inline void ApplyVariable<unsigned int, 2>( int p_iGlIndex, const unsigned int * p_pValue, size_t p_uiOcc)
{
	CheckGlError( glUniform2uiv( p_iGlIndex, p_uiOcc, p_pValue), CU_T( "ApplyVariable - glUniform2iv"));
}

template <> inline void ApplyVariable<unsigned int, 3>( int p_iGlIndex, const unsigned int * p_pValue, size_t p_uiOcc)
{
	CheckGlError( glUniform3uiv( p_iGlIndex, p_uiOcc, p_pValue), CU_T( "ApplyVariable - glUniform3iv"));
}

template <> inline void ApplyVariable<unsigned int, 4>( int p_iGlIndex, const unsigned int * p_pValue, size_t p_uiOcc)
{
	CheckGlError( glUniform4uiv( p_iGlIndex, p_uiOcc, p_pValue), CU_T( "ApplyVariable - glUniform4iv"));
}

template <> inline void ApplyVariable<float>( int p_iGlIndex, const float * p_pValue, size_t p_uiOcc)
{
	CheckGlError( glUniform1fv( p_iGlIndex, p_uiOcc, p_pValue), CU_T( "ApplyVariable - glUniform1r"));
}

template <> inline void ApplyVariable<float, 1>( int p_iGlIndex, const float * p_pValue, size_t p_uiOcc)
{
	CheckGlError( glUniform1fv( p_iGlIndex, p_uiOcc, p_pValue), CU_T( "ApplyVariable - glUniform1r"));
}

template <> inline void ApplyVariable<float, 2>( int p_iGlIndex, const float * p_pValue, size_t p_uiOcc)
{
	CheckGlError( glUniform2fv( p_iGlIndex, p_uiOcc, p_pValue), CU_T( "ApplyVariable - glUniform2fv"));
}

template <> inline void ApplyVariable<float, 3>( int p_iGlIndex, const float * p_pValue, size_t p_uiOcc)
{
	CheckGlError( glUniform3fv( p_iGlIndex, p_uiOcc, p_pValue), CU_T( "ApplyVariable - glUniform3fv"));
}

template <> inline void ApplyVariable<float, 4>( int p_iGlIndex, const float * p_pValue, size_t p_uiOcc)
{
	CheckGlError( glUniform4fv( p_iGlIndex, p_uiOcc, p_pValue), CU_T( "ApplyVariable - glUniform4fv"));
}

template <> inline void ApplyVariable<float, 2, 2>( int p_iGlIndex, const float * p_pValue, size_t p_uiOcc)
{
	CheckGlError( glUniformMatrix2fv( p_iGlIndex, 4 * p_uiOcc, false, p_pValue), CU_T( "ApplyVariable - glUniformMatrix2fv"));
}

template <> inline void ApplyVariable<float, 2, 3>( int p_iGlIndex, const float * p_pValue, size_t p_uiOcc)
{
	CheckGlError( glUniformMatrix2x3fv( p_iGlIndex, 9 * p_uiOcc, false, p_pValue), CU_T( "ApplyVariable - glUniformMatrix2x3fv"));
}

template <> inline void ApplyVariable<float, 2, 4>( int p_iGlIndex, const float * p_pValue, size_t p_uiOcc)
{
	CheckGlError( glUniformMatrix2x4fv( p_iGlIndex, 16 * p_uiOcc, false, p_pValue), CU_T( "ApplyVariable - glUniformMatrix2x4fv"));
}

template <> inline void ApplyVariable<float, 3, 2>( int p_iGlIndex, const float * p_pValue, size_t p_uiOcc)
{
	CheckGlError( glUniformMatrix3x2fv( p_iGlIndex, 4 * p_uiOcc, false, p_pValue), CU_T( "ApplyVariable - glUniformMatrix3x2fv"));
}

template <> inline void ApplyVariable<float, 3, 3>( int p_iGlIndex, const float * p_pValue, size_t p_uiOcc)
{
	CheckGlError( glUniformMatrix3fv( p_iGlIndex, 9 * p_uiOcc, false, p_pValue), CU_T( "ApplyVariable - glUniformMatrix3fv"));
}

template <> inline void ApplyVariable<float, 3, 4>( int p_iGlIndex, const float * p_pValue, size_t p_uiOcc)
{
	CheckGlError( glUniformMatrix3x4fv( p_iGlIndex, 16 * p_uiOcc, false, p_pValue), CU_T( "ApplyVariable - glUniformMatrix3x4fv"));
}

template <> inline void ApplyVariable<float, 4, 2>( int p_iGlIndex, const float * p_pValue, size_t p_uiOcc)
{
	CheckGlError( glUniformMatrix4x2fv( p_iGlIndex, 4 * p_uiOcc, false, p_pValue), CU_T( "ApplyVariable - glUniformMatrix4x2fv"));
}

template <> inline void ApplyVariable<float, 4, 3>( int p_iGlIndex, const float * p_pValue, size_t p_uiOcc)
{
	CheckGlError( glUniformMatrix4x3fv( p_iGlIndex, 9 * p_uiOcc, false, p_pValue), CU_T( "ApplyVariable - glUniformMatrix4x3fv"));
}

template <> inline void ApplyVariable<float, 4, 4>( int p_iGlIndex, const float * p_pValue, size_t p_uiOcc)
{
	CheckGlError( glUniformMatrix4fv( p_iGlIndex, 16 * p_uiOcc, false, p_pValue), CU_T( "ApplyVariable - glUniformMatrix4fv"));
}

//***********************************************************************************************************************

template <typename Type> 
inline void GlFrameVariableBase :: _apply( const String & p_strName, const Type * p_pValue, size_t p_uiOcc)
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
inline void GlFrameVariableBase :: _apply( const String & p_strName, const Type * p_pValue, size_t p_uiOcc)
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
inline void GlFrameVariableBase :: _apply( const String & p_strName, const Type * p_pValue, size_t p_uiOcc)
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
