template <> inline void ApplyVariable<float>( CGparameter p_cgParameter, float const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgSetParameter1fv( p_cgParameter, p_pValue);
		CheckCgError( cuT( "ApplyVariable - cgGLSetParameter1fv"));
	}
	else
	{
//		cgSetParameterArray1f( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( cuT( "ApplyVariable - cgGLSetParameterArray1f"));
	}

}

template <> inline void ApplyVariable<float, 1>( CGparameter p_cgParameter, float const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgSetParameter1fv( p_cgParameter, p_pValue);
		CheckCgError( cuT( "ApplyVariable - cgGLSetParameter1fv"));
	}
	else
	{
//		cgSetParameterArray1f( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( cuT( "ApplyVariable - cgGLSetParameterArray1f"));
	}
}

template <> inline void ApplyVariable<float, 2>( CGparameter p_cgParameter, float const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgSetParameter2fv( p_cgParameter, p_pValue);
		CheckCgError( cuT( "ApplyVariable - cgGLSetParameter2fv"));
	}
	else
	{
//		cgSetParameterArray2f( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( cuT( "ApplyVariable - cgGLSetParameterArray2f"));
	}
}

template <> inline void ApplyVariable<float, 3>( CGparameter p_cgParameter, float const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgSetParameter3fv( p_cgParameter, p_pValue);
		CheckCgError( cuT( "ApplyVariable - cgGLSetParameter3fv"));
	}
	else
	{
//		cgSetParameterArray3f( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( cuT( "ApplyVariable - cgGLSetParameterArray3f"));
	}
}

template <> inline void ApplyVariable<float, 4>( CGparameter p_cgParameter, float const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgSetParameter4fv( p_cgParameter, p_pValue);
		CheckCgError( cuT( "ApplyVariable - cgGLSetParameter4fv"));
	}
	else
	{
//		cgSetParameterArray4f( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( cuT( "ApplyVariable - cgGLSetParameterArray4f"));
	}
}

template <> inline void ApplyVariable<float, 2, 2>( CGparameter p_cgParameter, float const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgSetMatrixParameterfr( p_cgParameter, p_pValue);
		CheckCgError( cuT( "ApplyVariable - cgGLSetMatrixParameterfr"));
	}
	else
	{
//		cgSetMatrixParameterArrayfr( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( cuT( "ApplyVariable - cgGLSetMatrixParameterArrayfr"));
	}
}

template <> inline void ApplyVariable<float, 2, 3>( CGparameter p_cgParameter, float const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgSetMatrixParameterfr( p_cgParameter, p_pValue);
		CheckCgError( cuT( "ApplyVariable - cgGLSetMatrixParameterfr"));
	}
	else
	{
//		cgSetMatrixParameterArrayfr( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( cuT( "ApplyVariable - cgGLSetMatrixParameterArrayfr"));
	}
}

template <> inline void ApplyVariable<float, 2, 4>( CGparameter p_cgParameter, float const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgSetMatrixParameterfr( p_cgParameter, p_pValue);
		CheckCgError( cuT( "ApplyVariable - cgGLSetMatrixParameterfr"));
	}
	else
	{
//		cgSetMatrixParameterArrayfr( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( cuT( "ApplyVariable - cgGLSetMatrixParameterArrayfr"));
	}
}

template <> inline void ApplyVariable<float, 3, 2>( CGparameter p_cgParameter, float const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgSetMatrixParameterfr( p_cgParameter, p_pValue);
		CheckCgError( cuT( "ApplyVariable - cgGLSetMatrixParameterfr"));
	}
	else
	{
//		cgSetMatrixParameterArrayfr( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( cuT( "ApplyVariable - cgGLSetMatrixParameterArrayfr"));
	}
}

template <> inline void ApplyVariable<float, 3, 3>( CGparameter p_cgParameter, float const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgSetMatrixParameterfr( p_cgParameter, p_pValue);
		CheckCgError( cuT( "ApplyVariable - cgGLSetMatrixParameterfr"));
	}
	else
	{
//		cgSetMatrixParameterArrayfr( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( cuT( "ApplyVariable - cgGLSetMatrixParameterArrayfr"));
	}
}

template <> inline void ApplyVariable<float, 3, 4>( CGparameter p_cgParameter, float const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgSetMatrixParameterfr( p_cgParameter, p_pValue);
		CheckCgError( cuT( "ApplyVariable - cgGLSetMatrixParameterfr"));
	}
	else
	{
//		cgSetMatrixParameterArrayfr( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( cuT( "ApplyVariable - cgGLSetMatrixParameterArrayfr"));
	}
}

template <> inline void ApplyVariable<float, 4, 2>( CGparameter p_cgParameter, float const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgSetMatrixParameterfr( p_cgParameter, p_pValue);
		CheckCgError( cuT( "ApplyVariable - cgGLSetMatrixParameterfr"));
	}
	else
	{
//		cgSetMatrixParameterArrayfr( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( cuT( "ApplyVariable - cgGLSetMatrixParameterArrayfr"));
	}
}

template <> inline void ApplyVariable<float, 4, 3>( CGparameter p_cgParameter, float const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgSetMatrixParameterfr( p_cgParameter, p_pValue);
		CheckCgError( cuT( "ApplyVariable - cgGLSetMatrixParameterfr"));
	}
	else
	{
//		cgSetMatrixParameterArrayfr( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( cuT( "ApplyVariable - cgGLSetMatrixParameterArrayfr"));
	}
}

template <> inline void ApplyVariable<float, 4, 4>( CGparameter p_cgParameter, float const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgSetMatrixParameterfr( p_cgParameter, p_pValue);
		CheckCgError( cuT( "ApplyVariable - cgGLSetMatrixParameterfr"));
	}
	else
	{
//		cgSetMatrixParameterArrayfr( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( cuT( "ApplyVariable - cgGLSetMatrixParameterArrayfr"));
	}
}

template <> inline void ApplyVariable<double>( CGparameter p_cgParameter, double const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgSetParameter1dv( p_cgParameter, p_pValue);
		CheckCgError( cuT( "ApplyVariable - cgGLSetParameter1dv"));
	}
	else
	{
//		cgSetParameterArray1d( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( cuT( "ApplyVariable - cgGLSetParameterArray1d"));
	}
}

template <> inline void ApplyVariable<double, 1>( CGparameter p_cgParameter, double const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgSetParameter1dv( p_cgParameter, p_pValue);
		CheckCgError( cuT( "ApplyVariable - cgGLSetParameter1dv"));
	}
	else
	{
//		cgSetParameterArray1d( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( cuT( "ApplyVariable - cgGLSetParameterArray1d"));
	}
}

template <> inline void ApplyVariable<double, 2>( CGparameter p_cgParameter, double const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgSetParameter2dv( p_cgParameter, p_pValue);
		CheckCgError( cuT( "ApplyVariable - cgGLSetParameter2dv"));
	}
	else
	{
//		cgSetParameterArray2d( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( cuT( "ApplyVariable - cgGLSetParameterArray2d"));
	}
}

template <> inline void ApplyVariable<double, 3>( CGparameter p_cgParameter, double const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgSetParameter3dv( p_cgParameter, p_pValue);
		CheckCgError( cuT( "ApplyVariable - cgGLSetParameter3dv"));
	}
	else
	{
//		cgSetParameterArray3d( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( cuT( "ApplyVariable - cgGLSetParameterArray3d"));
	}
}

template <> inline void ApplyVariable<double, 4>( CGparameter p_cgParameter, double const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgSetParameter4dv( p_cgParameter, p_pValue);
		CheckCgError( cuT( "ApplyVariable - cgGLSetParameter4dv"));
	}
	else
	{
//		cgSetParameterArray4d( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( cuT( "ApplyVariable - cgGLSetParameterArray4d"));
	}
}

template <> inline void ApplyVariable<double, 2, 2>( CGparameter p_cgParameter, double const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgSetMatrixParameterdr( p_cgParameter, p_pValue);
		CheckCgError( cuT( "ApplyVariable - cgGLSetMatrixParameterdr"));
	}
	else
	{
//		cgSetMatrixParameterArraydr( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( cuT( "ApplyVariable - cgGLSetMatrixParameterArraydr"));
	}
}

template <> inline void ApplyVariable<double, 2, 3>( CGparameter p_cgParameter, double const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgSetMatrixParameterdr( p_cgParameter, p_pValue);
		CheckCgError( cuT( "ApplyVariable - cgGLSetMatrixParameterdr"));
	}
	else
	{
//		cgSetMatrixParameterArraydr( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( cuT( "ApplyVariable - cgGLSetMatrixParameterArraydr"));
	}
}

template <> inline void ApplyVariable<double, 2, 4>( CGparameter p_cgParameter, double const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgSetMatrixParameterdr( p_cgParameter, p_pValue);
		CheckCgError( cuT( "ApplyVariable - cgGLSetMatrixParameterdr"));
	}
	else
	{
//		cgSetMatrixParameterArraydr( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( cuT( "ApplyVariable - cgGLSetMatrixParameterArraydr"));
	}
}

template <> inline void ApplyVariable<double, 3, 2>( CGparameter p_cgParameter, double const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgSetMatrixParameterdr( p_cgParameter, p_pValue);
		CheckCgError( cuT( "ApplyVariable - cgGLSetMatrixParameterdr"));
	}
	else
	{
//		cgSetMatrixParameterArraydr( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( cuT( "ApplyVariable - cgGLSetMatrixParameterArraydr"));
	}
}

template <> inline void ApplyVariable<double, 3, 3>( CGparameter p_cgParameter, double const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgSetMatrixParameterdr( p_cgParameter, p_pValue);
		CheckCgError( cuT( "ApplyVariable - cgGLSetMatrixParameterdr"));
	}
	else
	{
//		cgSetMatrixParameterArraydr( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( cuT( "ApplyVariable - cgGLSetMatrixParameterArraydr"));
	}
}

template <> inline void ApplyVariable<double, 3, 4>( CGparameter p_cgParameter, double const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgSetMatrixParameterdr( p_cgParameter, p_pValue);
		CheckCgError( cuT( "ApplyVariable - cgGLSetMatrixParameterdr"));
	}
	else
	{
//		cgSetMatrixParameterArraydr( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( cuT( "ApplyVariable - cgGLSetMatrixParameterArraydr"));
	}
}

template <> inline void ApplyVariable<double, 4, 2>( CGparameter p_cgParameter, double const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgSetMatrixParameterdr( p_cgParameter, p_pValue);
		CheckCgError( cuT( "ApplyVariable - cgGLSetMatrixParameterdr"));
	}
	else
	{
//		cgSetMatrixParameterArraydr( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( cuT( "ApplyVariable - cgGLSetMatrixParameterArraydr"));
	}
}

template <> inline void ApplyVariable<double, 4, 3>( CGparameter p_cgParameter, double const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgSetMatrixParameterdr( p_cgParameter, p_pValue);
		CheckCgError( cuT( "ApplyVariable - cgGLSetMatrixParameterdr"));
	}
	else
	{
//		cgSetMatrixParameterArraydr( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( cuT( "ApplyVariable - cgGLSetMatrixParameterArraydr"));
	}
}

template <> inline void ApplyVariable<double, 4, 4>( CGparameter p_cgParameter, double const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgSetMatrixParameterdr( p_cgParameter, p_pValue);
		CheckCgError( cuT( "ApplyVariable - cgGLSetMatrixParameterdr"));
	}
	else
	{
//		cgSetMatrixParameterArraydr( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( cuT( "ApplyVariable - cgGLSetMatrixParameterArraydr"));
	}
}

template <typename T> inline void ApplyVariable( CGparameter p_cgParameter, T const * p_pValue, size_t p_uiOcc)
{
	CASTOR_EXCEPTION( "ApplyVariable - Unsupported arguments");
}

template <typename T, size_t Count> inline void ApplyVariable( CGparameter p_cgParameter, T const * p_pValue, size_t p_uiOcc)
{
	CASTOR_EXCEPTION( "ApplyVariable - Unsupported arguments");
}

template <typename T, size_t Rows, size_t Columns> inline void ApplyVariable( CGparameter p_cgParameter, T const * p_pValue, size_t p_uiOcc)
{
	CASTOR_EXCEPTION( "ApplyVariable - Unsupported arguments");
}

//***********************************************************************************************************************

template <typename Type>
inline void CgDx9FrameVariableBase :: _apply( String const & p_strName, Type const * p_pValue, size_t p_uiOcc, CGprogram p_cgProgram)
{
	if (m_bPresentInProgram)
	{
		if (m_cgParameter == NULL)
		{
			GetVariableLocation( p_strName.char_str(), p_cgProgram);
		}
		if (m_cgParameter)
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
inline void CgDx9FrameVariableBase :: _apply( String const & p_strName, Type const * p_pValue, size_t p_uiOcc, CGprogram p_cgProgram)
{
	if (m_bPresentInProgram)
	{
		if (m_cgParameter == NULL)
		{
			GetVariableLocation( p_strName.char_str(), p_cgProgram);
		}
		if (m_cgParameter)
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
inline void CgDx9FrameVariableBase :: _apply( String const & p_strName, Type const * p_pValue, size_t p_uiOcc, CGprogram p_cgProgram)
{
	if (m_bPresentInProgram)
	{
		if (m_cgParameter == NULL)
		{
			GetVariableLocation( p_strName.char_str(), p_cgProgram);
		}
		if (m_cgParameter)
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
