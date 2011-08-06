template <> inline void ApplyVariable<int>( CGparameter p_cgParameter, int const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgCheckError( cgSetParameter1iv( p_cgParameter, p_pValue), cuT( "ApplyVariable - cgGLSetParameter1fv"));
	}
	else
	{
		cgCheckError( cgSetParameterValueir( p_cgParameter, p_uiOcc, p_pValue), cuT( "ApplyVariable - cgGLSetParameterArray1f"));
	}

}

template <> inline void ApplyVariable<int, 1>( CGparameter p_cgParameter, int const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgCheckError( cgSetParameter1iv( p_cgParameter, p_pValue), cuT( "ApplyVariable - cgGLSetParameter1fv"));
	}
	else
	{
		cgCheckError( cgSetParameterValueir( p_cgParameter, p_uiOcc, p_pValue), cuT( "ApplyVariable - cgGLSetParameterArray1f"));
	}
}

template <> inline void ApplyVariable<int, 2>( CGparameter p_cgParameter, int const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgCheckError( cgSetParameter2iv( p_cgParameter, p_pValue), cuT( "ApplyVariable - cgGLSetParameter2fv"));
	}
	else
	{
		cgCheckError( cgSetParameterValueir( p_cgParameter, p_uiOcc * 2, p_pValue), cuT( "ApplyVariable - cgGLSetParameterArray2f"));
	}
}

template <> inline void ApplyVariable<int, 3>( CGparameter p_cgParameter, int const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgCheckError( cgSetParameter3iv( p_cgParameter, p_pValue), cuT( "ApplyVariable - cgGLSetParameter3fv"));
	}
	else
	{
		cgCheckError( cgSetParameterValueir( p_cgParameter, p_uiOcc * 3, p_pValue), cuT( "ApplyVariable - cgGLSetParameterArray3f"));
	}
}

template <> inline void ApplyVariable<int, 4>( CGparameter p_cgParameter, int const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgCheckError( cgSetParameter4iv( p_cgParameter, p_pValue), cuT( "ApplyVariable - cgGLSetParameter4fv"));
	}
	else
	{
		cgCheckError( cgSetParameterValueir( p_cgParameter, p_uiOcc * 4, p_pValue), cuT( "ApplyVariable - cgGLSetParameterArray4f"));
	}
}

template <> inline void ApplyVariable<int, 2, 2>( CGparameter p_cgParameter, int const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgCheckError( cgSetMatrixParameterir( p_cgParameter, p_pValue), cuT( "ApplyVariable - cgGLSetMatrixParameterfr"));
	}
	else
	{
		cgCheckError( cgSetParameterValueir( p_cgParameter, p_uiOcc * 4, p_pValue), cuT( "ApplyVariable - cgGLSetMatrixParameterArrayfr"));
	}
}

template <> inline void ApplyVariable<int, 2, 3>( CGparameter p_cgParameter, int const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgCheckError( cgSetMatrixParameterir( p_cgParameter, p_pValue), cuT( "ApplyVariable - cgGLSetMatrixParameterfr"));
	}
	else
	{
		cgCheckError( cgSetParameterValueir( p_cgParameter, p_uiOcc * 6, p_pValue), cuT( "ApplyVariable - cgGLSetMatrixParameterArrayfr"));
	}
}

template <> inline void ApplyVariable<int, 2, 4>( CGparameter p_cgParameter, int const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgCheckError( cgSetMatrixParameterir( p_cgParameter, p_pValue), cuT( "ApplyVariable - cgGLSetMatrixParameterfr"));
	}
	else
	{
		cgCheckError( cgSetParameterValueir( p_cgParameter, p_uiOcc * 8, p_pValue), cuT( "ApplyVariable - cgGLSetMatrixParameterArrayfr"));
	}
}

template <> inline void ApplyVariable<int, 3, 2>( CGparameter p_cgParameter, int const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgCheckError( cgSetMatrixParameterir( p_cgParameter, p_pValue), cuT( "ApplyVariable - cgGLSetMatrixParameterfr"));
	}
	else
	{
		cgCheckError( cgSetParameterValueir( p_cgParameter, p_uiOcc * 6, p_pValue), cuT( "ApplyVariable - cgGLSetMatrixParameterArrayfr"));
	}
}

template <> inline void ApplyVariable<int, 3, 3>( CGparameter p_cgParameter, int const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgCheckError( cgSetMatrixParameterir( p_cgParameter, p_pValue), cuT( "ApplyVariable - cgGLSetMatrixParameterfr"));
	}
	else
	{
		cgCheckError( cgSetParameterValueir( p_cgParameter, p_uiOcc * 9, p_pValue), cuT( "ApplyVariable - cgGLSetMatrixParameterArrayfr"));
	}
}

template <> inline void ApplyVariable<int, 3, 4>( CGparameter p_cgParameter, int const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgCheckError( cgSetMatrixParameterir( p_cgParameter, p_pValue), cuT( "ApplyVariable - cgGLSetMatrixParameterfr"));
	}
	else
	{
		cgCheckError( cgSetParameterValueir( p_cgParameter, p_uiOcc * 12, p_pValue), cuT( "ApplyVariable - cgGLSetMatrixParameterArrayfr"));
	}
}

template <> inline void ApplyVariable<int, 4, 2>( CGparameter p_cgParameter, int const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgCheckError( cgSetMatrixParameterir( p_cgParameter, p_pValue), cuT( "ApplyVariable - cgGLSetMatrixParameterfr"));
	}
	else
	{
		cgCheckError( cgSetParameterValueir( p_cgParameter, p_uiOcc * 8, p_pValue), cuT( "ApplyVariable - cgGLSetMatrixParameterArrayfr"));
	}
}

template <> inline void ApplyVariable<int, 4, 3>( CGparameter p_cgParameter, int const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgCheckError( cgSetMatrixParameterir( p_cgParameter, p_pValue), cuT( "ApplyVariable - cgGLSetMatrixParameterfr"));
	}
	else
	{
		cgCheckError( cgSetParameterValueir( p_cgParameter, p_uiOcc * 12, p_pValue), cuT( "ApplyVariable - cgGLSetMatrixParameterArrayfr"));
	}
}

template <> inline void ApplyVariable<int, 4, 4>( CGparameter p_cgParameter, int const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgCheckError( cgSetMatrixParameterir( p_cgParameter, p_pValue), cuT( "ApplyVariable - cgGLSetMatrixParameterfr"));
	}
	else
	{
		cgCheckError( cgSetParameterValueir( p_cgParameter, p_uiOcc * 16, p_pValue), cuT( "ApplyVariable - cgGLSetMatrixParameterArrayfr"));
	}
}

template <> inline void ApplyVariable<float>( CGparameter p_cgParameter, float const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgCheckError( cgSetParameter1fv( p_cgParameter, p_pValue), cuT( "ApplyVariable - cgGLSetParameter1fv"));
	}
	else
	{
		cgCheckError( cgSetParameterValuefr( p_cgParameter, p_uiOcc, p_pValue), cuT( "ApplyVariable - cgGLSetParameterArray1f"));
	}

}

template <> inline void ApplyVariable<float, 1>( CGparameter p_cgParameter, float const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgCheckError( cgSetParameter1fv( p_cgParameter, p_pValue), cuT( "ApplyVariable - cgGLSetParameter1fv"));
	}
	else
	{
		cgCheckError( cgSetParameterValuefr( p_cgParameter, p_uiOcc, p_pValue), cuT( "ApplyVariable - cgGLSetParameterArray1f"));
	}
}

template <> inline void ApplyVariable<float, 2>( CGparameter p_cgParameter, float const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgCheckError( cgSetParameter2fv( p_cgParameter, p_pValue), cuT( "ApplyVariable - cgGLSetParameter2fv"));
	}
	else
	{
		cgCheckError( cgSetParameterValuefr( p_cgParameter, p_uiOcc * 2, p_pValue), cuT( "ApplyVariable - cgGLSetParameterArray2f"));
	}
}

template <> inline void ApplyVariable<float, 3>( CGparameter p_cgParameter, float const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgCheckError( cgSetParameter3fv( p_cgParameter, p_pValue), cuT( "ApplyVariable - cgGLSetParameter3fv"));
	}
	else
	{
		cgCheckError( cgSetParameterValuefr( p_cgParameter, p_uiOcc * 3, p_pValue), cuT( "ApplyVariable - cgGLSetParameterArray3f"));
	}
}

template <> inline void ApplyVariable<float, 4>( CGparameter p_cgParameter, float const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgCheckError( cgSetParameter4fv( p_cgParameter, p_pValue), cuT( "ApplyVariable - cgGLSetParameter4fv"));
	}
	else
	{
		cgCheckError( cgSetParameterValuefr( p_cgParameter, p_uiOcc * 4, p_pValue), cuT( "ApplyVariable - cgGLSetParameterArray4f"));
	}
}

template <> inline void ApplyVariable<float, 2, 2>( CGparameter p_cgParameter, float const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgCheckError( cgSetMatrixParameterfr( p_cgParameter, p_pValue), cuT( "ApplyVariable - cgGLSetMatrixParameterfr"));
	}
	else
	{
		cgCheckError( cgSetParameterValuefr( p_cgParameter, p_uiOcc * 4, p_pValue), cuT( "ApplyVariable - cgGLSetMatrixParameterArrayfr"));
	}
}

template <> inline void ApplyVariable<float, 2, 3>( CGparameter p_cgParameter, float const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgCheckError( cgSetMatrixParameterfr( p_cgParameter, p_pValue), cuT( "ApplyVariable - cgGLSetMatrixParameterfr"));
	}
	else
	{
		cgCheckError( cgSetParameterValuefr( p_cgParameter, p_uiOcc * 6, p_pValue), cuT( "ApplyVariable - cgGLSetMatrixParameterArrayfr"));
	}
}

template <> inline void ApplyVariable<float, 2, 4>( CGparameter p_cgParameter, float const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgCheckError( cgSetMatrixParameterfr( p_cgParameter, p_pValue), cuT( "ApplyVariable - cgGLSetMatrixParameterfr"));
	}
	else
	{
		cgCheckError( cgSetParameterValuefr( p_cgParameter, p_uiOcc * 8, p_pValue), cuT( "ApplyVariable - cgGLSetMatrixParameterArrayfr"));
	}
}

template <> inline void ApplyVariable<float, 3, 2>( CGparameter p_cgParameter, float const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgCheckError( cgSetMatrixParameterfr( p_cgParameter, p_pValue), cuT( "ApplyVariable - cgGLSetMatrixParameterfr"));
	}
	else
	{
		cgCheckError( cgSetParameterValuefr( p_cgParameter, p_uiOcc * 6, p_pValue), cuT( "ApplyVariable - cgGLSetMatrixParameterArrayfr"));
	}
}

template <> inline void ApplyVariable<float, 3, 3>( CGparameter p_cgParameter, float const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgCheckError( cgSetMatrixParameterfr( p_cgParameter, p_pValue), cuT( "ApplyVariable - cgGLSetMatrixParameterfr"));
	}
	else
	{
		cgCheckError( cgSetParameterValuefr( p_cgParameter, p_uiOcc * 9, p_pValue), cuT( "ApplyVariable - cgGLSetMatrixParameterArrayfr"));
	}
}

template <> inline void ApplyVariable<float, 3, 4>( CGparameter p_cgParameter, float const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgCheckError( cgSetMatrixParameterfr( p_cgParameter, p_pValue), cuT( "ApplyVariable - cgGLSetMatrixParameterfr"));
	}
	else
	{
		cgCheckError( cgSetParameterValuefr( p_cgParameter, p_uiOcc * 12, p_pValue), cuT( "ApplyVariable - cgGLSetMatrixParameterArrayfr"));
	}
}

template <> inline void ApplyVariable<float, 4, 2>( CGparameter p_cgParameter, float const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgCheckError( cgSetMatrixParameterfr( p_cgParameter, p_pValue), cuT( "ApplyVariable - cgGLSetMatrixParameterfr"));
	}
	else
	{
		cgCheckError( cgSetParameterValuefr( p_cgParameter, p_uiOcc * 8, p_pValue), cuT( "ApplyVariable - cgGLSetMatrixParameterArrayfr"));
	}
}

template <> inline void ApplyVariable<float, 4, 3>( CGparameter p_cgParameter, float const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgCheckError( cgSetMatrixParameterfr( p_cgParameter, p_pValue), cuT( "ApplyVariable - cgGLSetMatrixParameterfr"));
	}
	else
	{
		cgCheckError( cgSetParameterValuefr( p_cgParameter, p_uiOcc * 12, p_pValue), cuT( "ApplyVariable - cgGLSetMatrixParameterArrayfr"));
	}
}

template <> inline void ApplyVariable<float, 4, 4>( CGparameter p_cgParameter, float const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgCheckError( cgSetMatrixParameterfr( p_cgParameter, p_pValue), cuT( "ApplyVariable - cgGLSetMatrixParameterfr"));
	}
	else
	{
		cgCheckError( cgSetParameterValuefr( p_cgParameter, p_uiOcc * 16, p_pValue), cuT( "ApplyVariable - cgGLSetMatrixParameterArrayfr"));
	}
}

template <> inline void ApplyVariable<double>( CGparameter p_cgParameter, double const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgCheckError( cgSetParameter1dv( p_cgParameter, p_pValue), cuT( "ApplyVariable - cgGLSetParameter1dv"));
	}
	else
	{
		cgCheckError( cgSetParameterValuedr( p_cgParameter, p_uiOcc, p_pValue), cuT( "ApplyVariable - cgGLSetParameterArray1d"));
	}
}

template <> inline void ApplyVariable<double, 1>( CGparameter p_cgParameter, double const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgCheckError( cgSetParameter1dv( p_cgParameter, p_pValue), cuT( "ApplyVariable - cgGLSetParameter1dv"));
	}
	else
	{
		cgCheckError( cgSetParameterValuedr( p_cgParameter, p_uiOcc, p_pValue), cuT( "ApplyVariable - cgGLSetParameterArray1d"));
	}
}

template <> inline void ApplyVariable<double, 2>( CGparameter p_cgParameter, double const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgCheckError( cgSetParameter2dv( p_cgParameter, p_pValue), cuT( "ApplyVariable - cgGLSetParameter2dv"));
	}
	else
	{
		cgCheckError( cgSetParameterValuedr( p_cgParameter, p_uiOcc * 2, p_pValue), cuT( "ApplyVariable - cgGLSetParameterArray2d"));
	}
}

template <> inline void ApplyVariable<double, 3>( CGparameter p_cgParameter, double const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgCheckError( cgSetParameter3dv( p_cgParameter, p_pValue), cuT( "ApplyVariable - cgGLSetParameter3dv"));
	}
	else
	{
		cgCheckError( cgSetParameterValuedr( p_cgParameter,  p_uiOcc * 3, p_pValue), cuT( "ApplyVariable - cgGLSetParameterArray3d"));
	}
}

template <> inline void ApplyVariable<double, 4>( CGparameter p_cgParameter, double const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgCheckError( cgSetParameter4dv( p_cgParameter, p_pValue), cuT( "ApplyVariable - cgGLSetParameter4dv"));
	}
	else
	{
		cgCheckError( cgSetParameterValuedr( p_cgParameter, p_uiOcc * 4, p_pValue), cuT( "ApplyVariable - cgGLSetParameterArray4d"));
	}
}

template <> inline void ApplyVariable<double, 2, 2>( CGparameter p_cgParameter, double const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgCheckError( cgSetMatrixParameterdr( p_cgParameter, p_pValue), cuT( "ApplyVariable - cgGLSetMatrixParameterdr"));
	}
	else
	{
		cgCheckError( cgSetParameterValuedr( p_cgParameter, p_uiOcc * 4, p_pValue), cuT( "ApplyVariable - cgGLSetMatrixParameterArraydr"));
	}
}

template <> inline void ApplyVariable<double, 2, 3>( CGparameter p_cgParameter, double const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgCheckError( cgSetMatrixParameterdr( p_cgParameter, p_pValue), cuT( "ApplyVariable - cgGLSetMatrixParameterdr"));
	}
	else
	{
		cgCheckError( cgSetParameterValuedr( p_cgParameter, p_uiOcc * 6, p_pValue), cuT( "ApplyVariable - cgGLSetMatrixParameterArraydr"));
	}
}

template <> inline void ApplyVariable<double, 2, 4>( CGparameter p_cgParameter, double const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgCheckError( cgSetMatrixParameterdr( p_cgParameter, p_pValue), cuT( "ApplyVariable - cgGLSetMatrixParameterdr"));
	}
	else
	{
		cgCheckError( cgSetParameterValuedr( p_cgParameter, p_uiOcc * 8, p_pValue), cuT( "ApplyVariable - cgGLSetMatrixParameterArraydr"));
	}
}

template <> inline void ApplyVariable<double, 3, 2>( CGparameter p_cgParameter, double const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgCheckError( cgSetMatrixParameterdr( p_cgParameter, p_pValue), cuT( "ApplyVariable - cgGLSetMatrixParameterdr"));
	}
	else
	{
		cgCheckError( cgSetParameterValuedr( p_cgParameter, p_uiOcc * 6, p_pValue), cuT( "ApplyVariable - cgGLSetMatrixParameterArraydr"));
	}
}

template <> inline void ApplyVariable<double, 3, 3>( CGparameter p_cgParameter, double const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgCheckError( cgSetMatrixParameterdr( p_cgParameter, p_pValue), cuT( "ApplyVariable - cgGLSetMatrixParameterdr"));
	}
	else
	{
		cgCheckError( cgSetParameterValuedr( p_cgParameter, p_uiOcc * 9, p_pValue), cuT( "ApplyVariable - cgGLSetMatrixParameterArraydr"));
	}
}

template <> inline void ApplyVariable<double, 3, 4>( CGparameter p_cgParameter, double const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgCheckError( cgSetMatrixParameterdr( p_cgParameter, p_pValue), cuT( "ApplyVariable - cgGLSetMatrixParameterdr"));
	}
	else
	{
		cgCheckError( cgSetParameterValuedr( p_cgParameter, p_uiOcc * 12, p_pValue), cuT( "ApplyVariable - cgGLSetMatrixParameterArraydr"));
	}
}

template <> inline void ApplyVariable<double, 4, 2>( CGparameter p_cgParameter, double const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgCheckError( cgSetMatrixParameterdr( p_cgParameter, p_pValue), cuT( "ApplyVariable - cgGLSetMatrixParameterdr"));
	}
	else
	{
		cgCheckError( cgSetParameterValuedr( p_cgParameter, p_uiOcc * 8, p_pValue), cuT( "ApplyVariable - cgGLSetMatrixParameterArraydr"));
	}
}

template <> inline void ApplyVariable<double, 4, 3>( CGparameter p_cgParameter, double const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgCheckError( cgSetMatrixParameterdr( p_cgParameter, p_pValue), cuT( "ApplyVariable - cgGLSetMatrixParameterdr"));
	}
	else
	{
		cgCheckError( cgSetParameterValuedr( p_cgParameter, p_uiOcc * 12, p_pValue), cuT( "ApplyVariable - cgGLSetMatrixParameterArraydr"));
	}
}

template <> inline void ApplyVariable<double, 4, 4>( CGparameter p_cgParameter, double const * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgCheckError( cgSetMatrixParameterdr( p_cgParameter, p_pValue), cuT( "ApplyVariable - cgGLSetMatrixParameterdr"));
	}
	else
	{
		cgCheckError( cgSetParameterValuedr( p_cgParameter, p_uiOcc * 16, p_pValue), cuT( "ApplyVariable - cgGLSetMatrixParameterArraydr"));
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
inline void CgGlFrameVariableBase :: _apply( String const & p_strName, Type const * p_pValue, size_t p_uiOcc, CGprogram p_cgProgram)
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
inline void CgGlFrameVariableBase :: _apply( String const & p_strName, Type const * p_pValue, size_t p_uiOcc, CGprogram p_cgProgram)
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
inline void CgGlFrameVariableBase :: _apply( String const & p_strName, Type const * p_pValue, size_t p_uiOcc, CGprogram p_cgProgram)
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
