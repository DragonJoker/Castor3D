template <> inline void ApplyVariable<float>( CGparameter p_cgParameter, const float * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgSetParameter1fv( p_cgParameter, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetParameter1fv"));
	}
	else
	{
//		cgSetParameterArray1f( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetParameterArray1f"));
	}

}

template <> inline void ApplyVariable<float, 1>( CGparameter p_cgParameter, const float * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgSetParameter1fv( p_cgParameter, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetParameter1fv"));
	}
	else
	{
//		cgSetParameterArray1f( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetParameterArray1f"));
	}
}

template <> inline void ApplyVariable<float, 2>( CGparameter p_cgParameter, const float * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgSetParameter2fv( p_cgParameter, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetParameter2fv"));
	}
	else
	{
//		cgSetParameterArray2f( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetParameterArray2f"));
	}
}

template <> inline void ApplyVariable<float, 3>( CGparameter p_cgParameter, const float * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgSetParameter3fv( p_cgParameter, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetParameter3fv"));
	}
	else
	{
//		cgSetParameterArray3f( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetParameterArray3f"));
	}
}

template <> inline void ApplyVariable<float, 4>( CGparameter p_cgParameter, const float * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgSetParameter4fv( p_cgParameter, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetParameter4fv"));
	}
	else
	{
//		cgSetParameterArray4f( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetParameterArray4f"));
	}
}

template <> inline void ApplyVariable<float, 2, 2>( CGparameter p_cgParameter, const float * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgSetMatrixParameterfr( p_cgParameter, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterfr"));
	}
	else
	{
//		cgSetMatrixParameterArrayfr( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterArrayfr"));
	}
}

template <> inline void ApplyVariable<float, 2, 3>( CGparameter p_cgParameter, const float * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgSetMatrixParameterfr( p_cgParameter, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterfr"));
	}
	else
	{
//		cgSetMatrixParameterArrayfr( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterArrayfr"));
	}
}

template <> inline void ApplyVariable<float, 2, 4>( CGparameter p_cgParameter, const float * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgSetMatrixParameterfr( p_cgParameter, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterfr"));
	}
	else
	{
//		cgSetMatrixParameterArrayfr( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterArrayfr"));
	}
}

template <> inline void ApplyVariable<float, 3, 2>( CGparameter p_cgParameter, const float * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgSetMatrixParameterfr( p_cgParameter, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterfr"));
	}
	else
	{
//		cgSetMatrixParameterArrayfr( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterArrayfr"));
	}
}

template <> inline void ApplyVariable<float, 3, 3>( CGparameter p_cgParameter, const float * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgSetMatrixParameterfr( p_cgParameter, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterfr"));
	}
	else
	{
//		cgSetMatrixParameterArrayfr( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterArrayfr"));
	}
}

template <> inline void ApplyVariable<float, 3, 4>( CGparameter p_cgParameter, const float * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgSetMatrixParameterfr( p_cgParameter, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterfr"));
	}
	else
	{
//		cgSetMatrixParameterArrayfr( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterArrayfr"));
	}
}

template <> inline void ApplyVariable<float, 4, 2>( CGparameter p_cgParameter, const float * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgSetMatrixParameterfr( p_cgParameter, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterfr"));
	}
	else
	{
//		cgSetMatrixParameterArrayfr( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterArrayfr"));
	}
}

template <> inline void ApplyVariable<float, 4, 3>( CGparameter p_cgParameter, const float * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgSetMatrixParameterfr( p_cgParameter, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterfr"));
	}
	else
	{
//		cgSetMatrixParameterArrayfr( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterArrayfr"));
	}
}

template <> inline void ApplyVariable<float, 4, 4>( CGparameter p_cgParameter, const float * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgSetMatrixParameterfr( p_cgParameter, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterfr"));
	}
	else
	{
//		cgSetMatrixParameterArrayfr( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterArrayfr"));
	}
}

template <> inline void ApplyVariable<double>( CGparameter p_cgParameter, const double * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgSetParameter1dv( p_cgParameter, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetParameter1dv"));
	}
	else
	{
//		cgSetParameterArray1d( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetParameterArray1d"));
	}
}

template <> inline void ApplyVariable<double, 1>( CGparameter p_cgParameter, const double * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgSetParameter1dv( p_cgParameter, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetParameter1dv"));
	}
	else
	{
//		cgSetParameterArray1d( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetParameterArray1d"));
	}
}

template <> inline void ApplyVariable<double, 2>( CGparameter p_cgParameter, const double * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgSetParameter2dv( p_cgParameter, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetParameter2dv"));
	}
	else
	{
//		cgSetParameterArray2d( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetParameterArray2d"));
	}
}

template <> inline void ApplyVariable<double, 3>( CGparameter p_cgParameter, const double * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgSetParameter3dv( p_cgParameter, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetParameter3dv"));
	}
	else
	{
//		cgSetParameterArray3d( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetParameterArray3d"));
	}
}

template <> inline void ApplyVariable<double, 4>( CGparameter p_cgParameter, const double * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgSetParameter4dv( p_cgParameter, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetParameter4dv"));
	}
	else
	{
//		cgSetParameterArray4d( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetParameterArray4d"));
	}
}

template <> inline void ApplyVariable<double, 2, 2>( CGparameter p_cgParameter, const double * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgSetMatrixParameterdr( p_cgParameter, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterdr"));
	}
	else
	{
//		cgSetMatrixParameterArraydr( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterArraydr"));
	}
}

template <> inline void ApplyVariable<double, 2, 3>( CGparameter p_cgParameter, const double * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgSetMatrixParameterdr( p_cgParameter, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterdr"));
	}
	else
	{
//		cgSetMatrixParameterArraydr( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterArraydr"));
	}
}

template <> inline void ApplyVariable<double, 2, 4>( CGparameter p_cgParameter, const double * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgSetMatrixParameterdr( p_cgParameter, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterdr"));
	}
	else
	{
//		cgSetMatrixParameterArraydr( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterArraydr"));
	}
}

template <> inline void ApplyVariable<double, 3, 2>( CGparameter p_cgParameter, const double * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgSetMatrixParameterdr( p_cgParameter, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterdr"));
	}
	else
	{
//		cgSetMatrixParameterArraydr( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterArraydr"));
	}
}

template <> inline void ApplyVariable<double, 3, 3>( CGparameter p_cgParameter, const double * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgSetMatrixParameterdr( p_cgParameter, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterdr"));
	}
	else
	{
//		cgSetMatrixParameterArraydr( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterArraydr"));
	}
}

template <> inline void ApplyVariable<double, 3, 4>( CGparameter p_cgParameter, const double * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgSetMatrixParameterdr( p_cgParameter, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterdr"));
	}
	else
	{
//		cgSetMatrixParameterArraydr( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterArraydr"));
	}
}

template <> inline void ApplyVariable<double, 4, 2>( CGparameter p_cgParameter, const double * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgSetMatrixParameterdr( p_cgParameter, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterdr"));
	}
	else
	{
//		cgSetMatrixParameterArraydr( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterArraydr"));
	}
}

template <> inline void ApplyVariable<double, 4, 3>( CGparameter p_cgParameter, const double * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgSetMatrixParameterdr( p_cgParameter, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterdr"));
	}
	else
	{
//		cgSetMatrixParameterArraydr( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterArraydr"));
	}
}

template <> inline void ApplyVariable<double, 4, 4>( CGparameter p_cgParameter, const double * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgSetMatrixParameterdr( p_cgParameter, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterdr"));
	}
	else
	{
//		cgSetMatrixParameterArraydr( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterArraydr"));
	}
}

//***********************************************************************************************************************

template <typename Type> 
inline void CgDx9FrameVariableBase :: _apply( const String & p_strName, const Type * p_pValue, size_t p_uiOcc)
{
	if (m_bPresentInProgram)
	{
		if (m_cgParameter == NULL)
		{
			GetVariableLocation( p_strName.char_str());
		}
		if (m_cgParameter != NULL) 
		{
			ApplyVariable<Type>( m_cgParameter, p_pValue, p_uiOcc);
		}
		else
		{
			m_bPresentInProgram = false;
		}
	}
}
template <typename Type, int Count>
inline void CgDx9FrameVariableBase :: _apply( const String & p_strName, const Type * p_pValue, size_t p_uiOcc)
{
	if (m_bPresentInProgram)
	{
		if (m_cgParameter == NULL)
		{
			GetVariableLocation( p_strName.char_str());
		}
		if (m_cgParameter != NULL) 
		{
			ApplyVariable<Type, Count>( m_cgParameter, p_pValue, p_uiOcc);
		}
		else
		{
			m_bPresentInProgram = false;
		}
	}
}
template <typename Type, int Rows, int Columns>
inline void CgDx9FrameVariableBase :: _apply( const String & p_strName, const Type * p_pValue, size_t p_uiOcc)
{
	if (m_bPresentInProgram)
	{
		if (m_cgParameter == NULL)
		{
			GetVariableLocation( p_strName.char_str());
		}
		if (m_cgParameter != NULL) 
		{
			ApplyVariable<Type, Rows, Columns>( m_cgParameter, p_pValue, p_uiOcc);
		}
		else
		{
			m_bPresentInProgram = false;
		}
	}
}

//***********************************************************************************************************************
