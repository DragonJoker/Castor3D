template <> inline void Castor3D::ApplyVariable<float>( CGparameter p_cgParameter, const float * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgGLSetParameter1fv( p_cgParameter, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetParameter1fv"));
	}
	else
	{
		cgGLSetParameterArray1f( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetParameterArray1f"));
	}

}

template <> inline void Castor3D::ApplyVariable<float, 1>( CGparameter p_cgParameter, const float * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgGLSetParameter1fv( p_cgParameter, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetParameter1fv"));
	}
	else
	{
		cgGLSetParameterArray1f( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetParameterArray1f"));
	}
}

template <> inline void Castor3D::ApplyVariable<float, 2>( CGparameter p_cgParameter, const float * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgGLSetParameter2fv( p_cgParameter, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetParameter2fv"));
	}
	else
	{
		cgGLSetParameterArray2f( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetParameterArray2f"));
	}
}

template <> inline void Castor3D::ApplyVariable<float, 3>( CGparameter p_cgParameter, const float * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgGLSetParameter3fv( p_cgParameter, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetParameter3fv"));
	}
	else
	{
		cgGLSetParameterArray3f( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetParameterArray3f"));
	}
}

template <> inline void Castor3D::ApplyVariable<float, 4>( CGparameter p_cgParameter, const float * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgGLSetParameter4fv( p_cgParameter, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetParameter4fv"));
	}
	else
	{
		cgGLSetParameterArray4f( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetParameterArray4f"));
	}
}

template <> inline void Castor3D::ApplyVariable<float, 2, 2>( CGparameter p_cgParameter, const float * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgGLSetMatrixParameterfr( p_cgParameter, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterfr"));
	}
	else
	{
		cgGLSetMatrixParameterArrayfr( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterArrayfr"));
	}
}

template <> inline void Castor3D::ApplyVariable<float, 2, 3>( CGparameter p_cgParameter, const float * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgGLSetMatrixParameterfr( p_cgParameter, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterfr"));
	}
	else
	{
		cgGLSetMatrixParameterArrayfr( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterArrayfr"));
	}
}

template <> inline void Castor3D::ApplyVariable<float, 2, 4>( CGparameter p_cgParameter, const float * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgGLSetMatrixParameterfr( p_cgParameter, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterfr"));
	}
	else
	{
		cgGLSetMatrixParameterArrayfr( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterArrayfr"));
	}
}

template <> inline void Castor3D::ApplyVariable<float, 3, 2>( CGparameter p_cgParameter, const float * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgGLSetMatrixParameterfr( p_cgParameter, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterfr"));
	}
	else
	{
		cgGLSetMatrixParameterArrayfr( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterArrayfr"));
	}
}

template <> inline void Castor3D::ApplyVariable<float, 3, 3>( CGparameter p_cgParameter, const float * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgGLSetMatrixParameterfr( p_cgParameter, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterfr"));
	}
	else
	{
		cgGLSetMatrixParameterArrayfr( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterArrayfr"));
	}
}

template <> inline void Castor3D::ApplyVariable<float, 3, 4>( CGparameter p_cgParameter, const float * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgGLSetMatrixParameterfr( p_cgParameter, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterfr"));
	}
	else
	{
		cgGLSetMatrixParameterArrayfr( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterArrayfr"));
	}
}

template <> inline void Castor3D::ApplyVariable<float, 4, 2>( CGparameter p_cgParameter, const float * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgGLSetMatrixParameterfr( p_cgParameter, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterfr"));
	}
	else
	{
		cgGLSetMatrixParameterArrayfr( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterArrayfr"));
	}
}

template <> inline void Castor3D::ApplyVariable<float, 4, 3>( CGparameter p_cgParameter, const float * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgGLSetMatrixParameterfr( p_cgParameter, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterfr"));
	}
	else
	{
		cgGLSetMatrixParameterArrayfr( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterArrayfr"));
	}
}

template <> inline void Castor3D::ApplyVariable<float, 4, 4>( CGparameter p_cgParameter, const float * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgGLSetMatrixParameterfr( p_cgParameter, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterfr"));
	}
	else
	{
		cgGLSetMatrixParameterArrayfr( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterArrayfr"));
	}
}

template <> inline void Castor3D::ApplyVariable<double>( CGparameter p_cgParameter, const double * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgGLSetParameter1dv( p_cgParameter, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetParameter1dv"));
	}
	else
	{
		cgGLSetParameterArray1d( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetParameterArray1d"));
	}
}

template <> inline void Castor3D::ApplyVariable<double, 1>( CGparameter p_cgParameter, const double * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgGLSetParameter1dv( p_cgParameter, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetParameter1dv"));
	}
	else
	{
		cgGLSetParameterArray1d( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetParameterArray1d"));
	}
}

template <> inline void Castor3D::ApplyVariable<double, 2>( CGparameter p_cgParameter, const double * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgGLSetParameter2dv( p_cgParameter, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetParameter2dv"));
	}
	else
	{
		cgGLSetParameterArray2d( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetParameterArray2d"));
	}
}

template <> inline void Castor3D::ApplyVariable<double, 3>( CGparameter p_cgParameter, const double * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgGLSetParameter3dv( p_cgParameter, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetParameter3dv"));
	}
	else
	{
		cgGLSetParameterArray3d( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetParameterArray3d"));
	}
}

template <> inline void Castor3D::ApplyVariable<double, 4>( CGparameter p_cgParameter, const double * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgGLSetParameter4dv( p_cgParameter, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetParameter4dv"));
	}
	else
	{
		cgGLSetParameterArray4d( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetParameterArray4d"));
	}
}

template <> inline void Castor3D::ApplyVariable<double, 2, 2>( CGparameter p_cgParameter, const double * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgGLSetMatrixParameterdr( p_cgParameter, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterdr"));
	}
	else
	{
		cgGLSetMatrixParameterArraydr( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterArraydr"));
	}
}

template <> inline void Castor3D::ApplyVariable<double, 2, 3>( CGparameter p_cgParameter, const double * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgGLSetMatrixParameterdr( p_cgParameter, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterdr"));
	}
	else
	{
		cgGLSetMatrixParameterArraydr( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterArraydr"));
	}
}

template <> inline void Castor3D::ApplyVariable<double, 2, 4>( CGparameter p_cgParameter, const double * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgGLSetMatrixParameterdr( p_cgParameter, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterdr"));
	}
	else
	{
		cgGLSetMatrixParameterArraydr( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterArraydr"));
	}
}

template <> inline void Castor3D::ApplyVariable<double, 3, 2>( CGparameter p_cgParameter, const double * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgGLSetMatrixParameterdr( p_cgParameter, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterdr"));
	}
	else
	{
		cgGLSetMatrixParameterArraydr( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterArraydr"));
	}
}

template <> inline void Castor3D::ApplyVariable<double, 3, 3>( CGparameter p_cgParameter, const double * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgGLSetMatrixParameterdr( p_cgParameter, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterdr"));
	}
	else
	{
		cgGLSetMatrixParameterArraydr( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterArraydr"));
	}
}

template <> inline void Castor3D::ApplyVariable<double, 3, 4>( CGparameter p_cgParameter, const double * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgGLSetMatrixParameterdr( p_cgParameter, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterdr"));
	}
	else
	{
		cgGLSetMatrixParameterArraydr( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterArraydr"));
	}
}

template <> inline void Castor3D::ApplyVariable<double, 4, 2>( CGparameter p_cgParameter, const double * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgGLSetMatrixParameterdr( p_cgParameter, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterdr"));
	}
	else
	{
		cgGLSetMatrixParameterArraydr( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterArraydr"));
	}
}

template <> inline void Castor3D::ApplyVariable<double, 4, 3>( CGparameter p_cgParameter, const double * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgGLSetMatrixParameterdr( p_cgParameter, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterdr"));
	}
	else
	{
		cgGLSetMatrixParameterArraydr( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterArraydr"));
	}
}

template <> inline void Castor3D::ApplyVariable<double, 4, 4>( CGparameter p_cgParameter, const double * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		cgGLSetMatrixParameterdr( p_cgParameter, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterdr"));
	}
	else
	{
		cgGLSetMatrixParameterArraydr( p_cgParameter, 0, p_uiOcc, p_pValue);
		CheckCgError( CU_T( "ApplyVariable - cgGLSetMatrixParameterArraydr"));
	}
}

//***********************************************************************************************************************

template <typename Type> 
inline void Castor3D::CgGLFrameVariableBase :: _apply( const String & p_strName, const Type * p_pValue, size_t p_uiOcc)
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
inline void Castor3D::CgGLFrameVariableBase :: _apply( const String & p_strName, const Type * p_pValue, size_t p_uiOcc)
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
inline void Castor3D::CgGLFrameVariableBase :: _apply( const String & p_strName, const Type * p_pValue, size_t p_uiOcc)
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