template<> inline void ApplyVariable< int >( CGparameter p_cgParameter, int const * p_pValue, uint32_t p_uiOcc )
{
	if( p_uiOcc <= 1 )
	{
		CheckCgError( cgSetParameter1iv( p_cgParameter, p_pValue ), cuT( "ApplyVariable - cgSetParameter1iv" ) );
	}
	else
	{
		CheckCgError( cgSetParameterValueir( p_cgParameter, p_uiOcc, p_pValue ), cuT( "ApplyVariable - cgSetParameterValueir" ) );
	}
}

template<> inline void ApplyVariable< int, 1 >( CGparameter p_cgParameter, int const * p_pValue, uint32_t p_uiOcc )
{
	if( p_uiOcc <= 1 )
	{
		CheckCgError( cgSetParameter1iv( p_cgParameter, p_pValue ), cuT( "ApplyVariable - cgSetParameter1iv" ) );
	}
	else
	{
		CheckCgError( cgSetParameterValueir( p_cgParameter, p_uiOcc, p_pValue ), cuT( "ApplyVariable - cgSetParameterValueir" ) );
	}
}

template<> inline void ApplyVariable< int, 2 >( CGparameter p_cgParameter, int const * p_pValue, uint32_t p_uiOcc )
{
	if( p_uiOcc <= 1 )
	{
		CheckCgError( cgSetParameter2iv( p_cgParameter, p_pValue ), cuT( "ApplyVariable - cgSetParameter2iv" ) );
	}
	else
	{
		CheckCgError( cgSetParameterValueir( p_cgParameter, p_uiOcc * 2, p_pValue ), cuT( "ApplyVariable - cgSetParameterValueir" ) );
	}
}

template<> inline void ApplyVariable< int, 3 >( CGparameter p_cgParameter, int const * p_pValue, uint32_t p_uiOcc )
{
	if( p_uiOcc <= 1 )
	{
		CheckCgError( cgSetParameter3iv( p_cgParameter, p_pValue ), cuT( "ApplyVariable - cgSetParameter3iv" ) );
	}
	else
	{
		CheckCgError( cgSetParameterValueir( p_cgParameter, p_uiOcc * 3, p_pValue ), cuT( "ApplyVariable - cgSetParameterValueir" ) );
	}
}

template<> inline void ApplyVariable< int, 4 >( CGparameter p_cgParameter, int const * p_pValue, uint32_t p_uiOcc )
{
	if( p_uiOcc <= 1 )
	{
		CheckCgError( cgSetParameter4iv( p_cgParameter, p_pValue ), cuT( "ApplyVariable - cgSetParameter4iv" ) );
	}
	else
	{
		CheckCgError( cgSetParameterValueir( p_cgParameter, p_uiOcc * 4, p_pValue ), cuT( "ApplyVariable - cgSetParameterValueir" ) );
	}
}

template<> inline void ApplyVariable< float >( CGparameter p_cgParameter, float const * p_pValue, uint32_t p_uiOcc )
{
	if( p_uiOcc <= 1 )
	{
		CheckCgError( cgSetParameter1fv( p_cgParameter, p_pValue ), cuT( "ApplyVariable - cgGLSetParameter1fv" ) );
	}
	else
	{
		CheckCgError( cgSetParameterValuefr( p_cgParameter, p_uiOcc, p_pValue ), cuT( "ApplyVariable - cgSetParameterValuefr" ) );
	}
}

template<> inline void ApplyVariable< float, 1 >( CGparameter p_cgParameter, float const * p_pValue, uint32_t p_uiOcc )
{
	if( p_uiOcc <= 1 )
	{
		CheckCgError( cgSetParameter1fv( p_cgParameter, p_pValue ), cuT( "ApplyVariable - cgSetParameter1fv" ) );
	}
	else
	{
		CheckCgError( cgSetParameterValuefr( p_cgParameter, p_uiOcc, p_pValue ), cuT( "ApplyVariable - cgSetParameterValuefr" ) );
	}
}

template<> inline void ApplyVariable< float, 2 >( CGparameter p_cgParameter, float const * p_pValue, uint32_t p_uiOcc )
{
	if( p_uiOcc <= 1 )
	{
		CheckCgError( cgSetParameter2fv( p_cgParameter, p_pValue ), cuT( "ApplyVariable - cgSetParameter2fv" ) );
	}
	else
	{
		CheckCgError( cgSetParameterValuefr( p_cgParameter, p_uiOcc * 2, p_pValue ), cuT( "ApplyVariable - cgSetParameterValuefr" ) );
	}
}

template<> inline void ApplyVariable< float, 3 >( CGparameter p_cgParameter, float const * p_pValue, uint32_t p_uiOcc )
{
	if( p_uiOcc <= 1 )
	{
		CheckCgError( cgSetParameter3fv( p_cgParameter, p_pValue ), cuT( "ApplyVariable - cgSetParameter3fv" ) );
	}
	else
	{
		CheckCgError( cgSetParameterValuefr( p_cgParameter, p_uiOcc * 3, p_pValue ), cuT( "ApplyVariable - cgSetParameterValuefr" ) );
	}
}

template<> inline void ApplyVariable< float, 4 >( CGparameter p_cgParameter, float const * p_pValue, uint32_t p_uiOcc )
{
	if( p_uiOcc <= 1 )
	{
		CheckCgError( cgSetParameter4fv( p_cgParameter, p_pValue ), cuT( "ApplyVariable - cgSetParameter4fv" ) );
	}
	else
	{
		CheckCgError( cgSetParameterValuefr( p_cgParameter, p_uiOcc * 4, p_pValue ), cuT( "ApplyVariable - cgSetParameterValuefr" ) );
	}
}

template<> inline void ApplyVariable< double >( CGparameter p_cgParameter, double const * p_pValue, uint32_t p_uiOcc )
{
	if( p_uiOcc <= 1 )
	{
		CheckCgError( cgSetParameter1dv( p_cgParameter, p_pValue ), cuT( "ApplyVariable - cgSetParameter1dv" ) );
	}
	else
	{
		CheckCgError( cgSetParameterValuedr( p_cgParameter, p_uiOcc, p_pValue ), cuT( "ApplyVariable - cgSetParameterValuedr" ) );
	}
}

template<> inline void ApplyVariable< double, 1 >( CGparameter p_cgParameter, double const * p_pValue, uint32_t p_uiOcc )
{
	if( p_uiOcc <= 1 )
	{
		CheckCgError( cgSetParameter1dv( p_cgParameter, p_pValue ), cuT( "ApplyVariable - cgSetParameter1dv" ) );
	}
	else
	{
		CheckCgError( cgSetParameterValuedr( p_cgParameter, p_uiOcc, p_pValue ), cuT( "ApplyVariable - cgSetParameterValuedr" ) );
	}
}

template<> inline void ApplyVariable< double, 2 >( CGparameter p_cgParameter, double const * p_pValue, uint32_t p_uiOcc )
{
	if( p_uiOcc <= 1 )
	{
		CheckCgError( cgSetParameter2dv( p_cgParameter, p_pValue ), cuT( "ApplyVariable - cgSetParameter2dv" ) );
	}
	else
	{
		CheckCgError( cgSetParameterValuedr( p_cgParameter, p_uiOcc * 2, p_pValue ), cuT( "ApplyVariable - cgSetParameterValuedr" ) );
	}
}

template<> inline void ApplyVariable< double, 3 >( CGparameter p_cgParameter, double const * p_pValue, uint32_t p_uiOcc )
{
	if( p_uiOcc <= 1 )
	{
		CheckCgError( cgSetParameter3dv( p_cgParameter, p_pValue ), cuT( "ApplyVariable - cgSetParameter3dv" ) );
	}
	else
	{
		CheckCgError( cgSetParameterValuedr( p_cgParameter,  p_uiOcc * 3, p_pValue ), cuT( "ApplyVariable - cgSetParameterValuedr" ) );
	}
}

template<> inline void ApplyVariable< double, 4 >( CGparameter p_cgParameter, double const * p_pValue, uint32_t p_uiOcc )
{
	if( p_uiOcc <= 1 )
	{
		CheckCgError( cgSetParameter4dv( p_cgParameter, p_pValue ), cuT( "ApplyVariable - cgSetParameter4dv" ) );
	}
	else
	{
		CheckCgError( cgSetParameterValuedr( p_cgParameter, p_uiOcc * 4, p_pValue ), cuT( "ApplyVariable - cgSetParameterValuedr" ) );
	}
}

template<> inline void ApplyVariable< int, 2, 2 >( CGparameter p_cgParameter, int const * p_pValue, uint32_t p_uiOcc )
{
	if( p_uiOcc <= 1 )
	{
		CheckCgError( cgSetMatrixParameterir( p_cgParameter, p_pValue ), cuT( "ApplyVariable - cgGLSetMatrixParameterfr" ) );
	}
	else
	{
		CheckCgError( cgSetParameterValueir( p_cgParameter, p_uiOcc * 4, p_pValue ), cuT( "ApplyVariable - cgGLSetMatrixParameterArrayfr" ) );
	}
}

template<> inline void ApplyVariable< int, 2, 3 >( CGparameter p_cgParameter, int const * p_pValue, uint32_t p_uiOcc )
{
	if( p_uiOcc <= 1 )
	{
		CheckCgError( cgSetMatrixParameterir( p_cgParameter, p_pValue ), cuT( "ApplyVariable - cgGLSetMatrixParameterfr" ) );
	}
	else
	{
		CheckCgError( cgSetParameterValueir( p_cgParameter, p_uiOcc * 6, p_pValue ), cuT( "ApplyVariable - cgGLSetMatrixParameterArrayfr" ) );
	}
}

template<> inline void ApplyVariable< int, 2, 4 >( CGparameter p_cgParameter, int const * p_pValue, uint32_t p_uiOcc )
{
	if( p_uiOcc <= 1 )
	{
		CheckCgError( cgSetMatrixParameterir( p_cgParameter, p_pValue ), cuT( "ApplyVariable - cgGLSetMatrixParameterfr" ) );
	}
	else
	{
		CheckCgError( cgSetParameterValueir( p_cgParameter, p_uiOcc * 8, p_pValue ), cuT( "ApplyVariable - cgGLSetMatrixParameterArrayfr" ) );
	}
}

template<> inline void ApplyVariable< int, 3, 2 >( CGparameter p_cgParameter, int const * p_pValue, uint32_t p_uiOcc )
{
	if( p_uiOcc <= 1 )
	{
		CheckCgError( cgSetMatrixParameterir( p_cgParameter, p_pValue ), cuT( "ApplyVariable - cgGLSetMatrixParameterfr" ) );
	}
	else
	{
		CheckCgError( cgSetParameterValueir( p_cgParameter, p_uiOcc * 6, p_pValue ), cuT( "ApplyVariable - cgGLSetMatrixParameterArrayfr" ) );
	}
}

template<> inline void ApplyVariable< int, 3, 3 >( CGparameter p_cgParameter, int const * p_pValue, uint32_t p_uiOcc )
{
	if( p_uiOcc <= 1 )
	{
		CheckCgError( cgSetMatrixParameterir( p_cgParameter, p_pValue ), cuT( "ApplyVariable - cgGLSetMatrixParameterfr" ) );
	}
	else
	{
		CheckCgError( cgSetParameterValueir( p_cgParameter, p_uiOcc * 9, p_pValue ), cuT( "ApplyVariable - cgGLSetMatrixParameterArrayfr" ) );
	}
}

template<> inline void ApplyVariable< int, 3, 4 >( CGparameter p_cgParameter, int const * p_pValue, uint32_t p_uiOcc )
{
	if( p_uiOcc <= 1 )
	{
		CheckCgError( cgSetMatrixParameterir( p_cgParameter, p_pValue ), cuT( "ApplyVariable - cgGLSetMatrixParameterfr" ) );
	}
	else
	{
		CheckCgError( cgSetParameterValueir( p_cgParameter, p_uiOcc * 12, p_pValue ), cuT( "ApplyVariable - cgGLSetMatrixParameterArrayfr" ) );
	}
}

template<> inline void ApplyVariable< int, 4, 2 >( CGparameter p_cgParameter, int const * p_pValue, uint32_t p_uiOcc )
{
	if( p_uiOcc <= 1 )
	{
		CheckCgError( cgSetMatrixParameterir( p_cgParameter, p_pValue ), cuT( "ApplyVariable - cgGLSetMatrixParameterfr" ) );
	}
	else
	{
		CheckCgError( cgSetParameterValueir( p_cgParameter, p_uiOcc * 8, p_pValue ), cuT( "ApplyVariable - cgGLSetMatrixParameterArrayfr" ) );
	}
}

template<> inline void ApplyVariable< int, 4, 3 >( CGparameter p_cgParameter, int const * p_pValue, uint32_t p_uiOcc )
{
	if( p_uiOcc <= 1 )
	{
		CheckCgError( cgSetMatrixParameterir( p_cgParameter, p_pValue ), cuT( "ApplyVariable - cgGLSetMatrixParameterfr" ) );
	}
	else
	{
		CheckCgError( cgSetParameterValueir( p_cgParameter, p_uiOcc * 12, p_pValue ), cuT( "ApplyVariable - cgGLSetMatrixParameterArrayfr" ) );
	}
}

template<> inline void ApplyVariable< int, 4, 4 >( CGparameter p_cgParameter, int const * p_pValue, uint32_t p_uiOcc )
{
	if( p_uiOcc <= 1 )
	{
		CheckCgError( cgSetMatrixParameterir( p_cgParameter, p_pValue ), cuT( "ApplyVariable - cgGLSetMatrixParameterfr" ) );
	}
	else
	{
		CheckCgError( cgSetParameterValueir( p_cgParameter, p_uiOcc * 16, p_pValue ), cuT( "ApplyVariable - cgGLSetMatrixParameterArrayfr" ) );
	}
}

template<> inline void ApplyVariable< float, 2, 2 >( CGparameter p_cgParameter, float const * p_pValue, uint32_t p_uiOcc )
{
	if( p_uiOcc <= 1 )
	{
		CheckCgError( cgSetMatrixParameterfr( p_cgParameter, p_pValue ), cuT( "ApplyVariable - cgGLSetMatrixParameterfr" ) );
	}
	else
	{
		CheckCgError( cgSetParameterValuefr( p_cgParameter, p_uiOcc * 4, p_pValue ), cuT( "ApplyVariable - cgGLSetMatrixParameterArrayfr" ) );
	}
}

template<> inline void ApplyVariable< float, 2, 3 >( CGparameter p_cgParameter, float const * p_pValue, uint32_t p_uiOcc )
{
	if( p_uiOcc <= 1 )
	{
		CheckCgError( cgSetMatrixParameterfr( p_cgParameter, p_pValue ), cuT( "ApplyVariable - cgGLSetMatrixParameterfr" ) );
	}
	else
	{
		CheckCgError( cgSetParameterValuefr( p_cgParameter, p_uiOcc * 6, p_pValue ), cuT( "ApplyVariable - cgGLSetMatrixParameterArrayfr" ) );
	}
}

template<> inline void ApplyVariable< float, 2, 4 >( CGparameter p_cgParameter, float const * p_pValue, uint32_t p_uiOcc )
{
	if( p_uiOcc <= 1 )
	{
		CheckCgError( cgSetMatrixParameterfr( p_cgParameter, p_pValue ), cuT( "ApplyVariable - cgGLSetMatrixParameterfr" ) );
	}
	else
	{
		CheckCgError( cgSetParameterValuefr( p_cgParameter, p_uiOcc * 8, p_pValue ), cuT( "ApplyVariable - cgGLSetMatrixParameterArrayfr" ) );
	}
}

template<> inline void ApplyVariable< float, 3, 2 >( CGparameter p_cgParameter, float const * p_pValue, uint32_t p_uiOcc )
{
	if( p_uiOcc <= 1 )
	{
		CheckCgError( cgSetMatrixParameterfr( p_cgParameter, p_pValue ), cuT( "ApplyVariable - cgGLSetMatrixParameterfr" ) );
	}
	else
	{
		CheckCgError( cgSetParameterValuefr( p_cgParameter, p_uiOcc * 6, p_pValue ), cuT( "ApplyVariable - cgGLSetMatrixParameterArrayfr" ) );
	}
}

template<> inline void ApplyVariable< float, 3, 3 >( CGparameter p_cgParameter, float const * p_pValue, uint32_t p_uiOcc )
{
	if( p_uiOcc <= 1 )
	{
		CheckCgError( cgSetMatrixParameterfr( p_cgParameter, p_pValue ), cuT( "ApplyVariable - cgGLSetMatrixParameterfr" ) );
	}
	else
	{
		CheckCgError( cgSetParameterValuefr( p_cgParameter, p_uiOcc * 9, p_pValue ), cuT( "ApplyVariable - cgGLSetMatrixParameterArrayfr" ) );
	}
}

template<> inline void ApplyVariable< float, 3, 4 >( CGparameter p_cgParameter, float const * p_pValue, uint32_t p_uiOcc )
{
	if( p_uiOcc <= 1 )
	{
		CheckCgError( cgSetMatrixParameterfr( p_cgParameter, p_pValue ), cuT( "ApplyVariable - cgGLSetMatrixParameterfr" ) );
	}
	else
	{
		CheckCgError( cgSetParameterValuefr( p_cgParameter, p_uiOcc * 12, p_pValue ), cuT( "ApplyVariable - cgGLSetMatrixParameterArrayfr" ) );
	}
}

template<> inline void ApplyVariable< float, 4, 2 >( CGparameter p_cgParameter, float const * p_pValue, uint32_t p_uiOcc )
{
	if( p_uiOcc <= 1 )
	{
		CheckCgError( cgSetMatrixParameterfr( p_cgParameter, p_pValue ), cuT( "ApplyVariable - cgGLSetMatrixParameterfr" ) );
	}
	else
	{
		CheckCgError( cgSetParameterValuefr( p_cgParameter, p_uiOcc * 8, p_pValue ), cuT( "ApplyVariable - cgGLSetMatrixParameterArrayfr" ) );
	}
}

template<> inline void ApplyVariable< float, 4, 3 >( CGparameter p_cgParameter, float const * p_pValue, uint32_t p_uiOcc )
{
	if( p_uiOcc <= 1 )
	{
		CheckCgError( cgSetMatrixParameterfr( p_cgParameter, p_pValue ), cuT( "ApplyVariable - cgGLSetMatrixParameterfr" ) );
	}
	else
	{
		CheckCgError( cgSetParameterValuefr( p_cgParameter, p_uiOcc * 12, p_pValue ), cuT( "ApplyVariable - cgGLSetMatrixParameterArrayfr" ) );
	}
}

template<> inline void ApplyVariable< float, 4, 4 >( CGparameter p_cgParameter, float const * p_pValue, uint32_t p_uiOcc )
{
	if( p_uiOcc <= 1 )
	{
		CheckCgError( cgSetMatrixParameterfr( p_cgParameter, p_pValue ), cuT( "ApplyVariable - cgGLSetMatrixParameterfr" ) );
	}
	else
	{
		CheckCgError( cgSetParameterValuefr( p_cgParameter, p_uiOcc * 16, p_pValue ), cuT( "ApplyVariable - cgGLSetMatrixParameterArrayfr" ) );
	}
}

template<> inline void ApplyVariable< double, 2, 2 >( CGparameter p_cgParameter, double const * p_pValue, uint32_t p_uiOcc )
{
	if( p_uiOcc <= 1 )
	{
		CheckCgError( cgSetMatrixParameterdr( p_cgParameter, p_pValue ), cuT( "ApplyVariable - cgGLSetMatrixParameterdr" ) );
	}
	else
	{
		CheckCgError( cgSetParameterValuedr( p_cgParameter, p_uiOcc * 4, p_pValue ), cuT( "ApplyVariable - cgGLSetMatrixParameterArraydr" ) );
	}
}

template<> inline void ApplyVariable< double, 2, 3 >( CGparameter p_cgParameter, double const * p_pValue, uint32_t p_uiOcc )
{
	if( p_uiOcc <= 1 )
	{
		CheckCgError( cgSetMatrixParameterdr( p_cgParameter, p_pValue ), cuT( "ApplyVariable - cgGLSetMatrixParameterdr" ) );
	}
	else
	{
		CheckCgError( cgSetParameterValuedr( p_cgParameter, p_uiOcc * 6, p_pValue ), cuT( "ApplyVariable - cgGLSetMatrixParameterArraydr" ) );
	}
}

template<> inline void ApplyVariable< double, 2, 4 >( CGparameter p_cgParameter, double const * p_pValue, uint32_t p_uiOcc )
{
	if( p_uiOcc <= 1 )
	{
		CheckCgError( cgSetMatrixParameterdr( p_cgParameter, p_pValue ), cuT( "ApplyVariable - cgGLSetMatrixParameterdr" ) );
	}
	else
	{
		CheckCgError( cgSetParameterValuedr( p_cgParameter, p_uiOcc * 8, p_pValue ), cuT( "ApplyVariable - cgGLSetMatrixParameterArraydr" ) );
	}
}

template<> inline void ApplyVariable< double, 3, 2 >( CGparameter p_cgParameter, double const * p_pValue, uint32_t p_uiOcc )
{
	if( p_uiOcc <= 1 )
	{
		CheckCgError( cgSetMatrixParameterdr( p_cgParameter, p_pValue ), cuT( "ApplyVariable - cgGLSetMatrixParameterdr" ) );
	}
	else
	{
		CheckCgError( cgSetParameterValuedr( p_cgParameter, p_uiOcc * 6, p_pValue ), cuT( "ApplyVariable - cgGLSetMatrixParameterArraydr" ) );
	}
}

template<> inline void ApplyVariable< double, 3, 3 >( CGparameter p_cgParameter, double const * p_pValue, uint32_t p_uiOcc )
{
	if( p_uiOcc <= 1 )
	{
		CheckCgError( cgSetMatrixParameterdr( p_cgParameter, p_pValue ), cuT( "ApplyVariable - cgGLSetMatrixParameterdr" ) );
	}
	else
	{
		CheckCgError( cgSetParameterValuedr( p_cgParameter, p_uiOcc * 9, p_pValue ), cuT( "ApplyVariable - cgGLSetMatrixParameterArraydr" ) );
	}
}

template<> inline void ApplyVariable< double, 3, 4 >( CGparameter p_cgParameter, double const * p_pValue, uint32_t p_uiOcc )
{
	if( p_uiOcc <= 1 )
	{
		CheckCgError( cgSetMatrixParameterdr( p_cgParameter, p_pValue ), cuT( "ApplyVariable - cgGLSetMatrixParameterdr" ) );
	}
	else
	{
		CheckCgError( cgSetParameterValuedr( p_cgParameter, p_uiOcc * 12, p_pValue ), cuT( "ApplyVariable - cgGLSetMatrixParameterArraydr" ) );
	}
}

template<> inline void ApplyVariable< double, 4, 2 >( CGparameter p_cgParameter, double const * p_pValue, uint32_t p_uiOcc )
{
	if( p_uiOcc <= 1 )
	{
		CheckCgError( cgSetMatrixParameterdr( p_cgParameter, p_pValue ), cuT( "ApplyVariable - cgGLSetMatrixParameterdr" ) );
	}
	else
	{
		CheckCgError( cgSetParameterValuedr( p_cgParameter, p_uiOcc * 8, p_pValue ), cuT( "ApplyVariable - cgGLSetMatrixParameterArraydr" ) );
	}
}

template<> inline void ApplyVariable< double, 4, 3 >( CGparameter p_cgParameter, double const * p_pValue, uint32_t p_uiOcc )
{
	if( p_uiOcc <= 1 )
	{
		CheckCgError( cgSetMatrixParameterdr( p_cgParameter, p_pValue ), cuT( "ApplyVariable - cgGLSetMatrixParameterdr" ) );
	}
	else
	{
		CheckCgError( cgSetParameterValuedr( p_cgParameter, p_uiOcc * 12, p_pValue ), cuT( "ApplyVariable - cgGLSetMatrixParameterArraydr" ) );
	}
}

template<> inline void ApplyVariable< double, 4, 4 >( CGparameter p_cgParameter, double const * p_pValue, uint32_t p_uiOcc )
{
	if( p_uiOcc <= 1 )
	{
		CheckCgError( cgSetMatrixParameterdr( p_cgParameter, p_pValue ), cuT( "ApplyVariable - cgGLSetMatrixParameterdr" ) );
	}
	else
	{
		CheckCgError( cgSetParameterValuedr( p_cgParameter, p_uiOcc * 16, p_pValue ), cuT( "ApplyVariable - cgGLSetMatrixParameterArraydr" ) );
	}
}

template< typename T > inline void ApplyVariable( CGparameter CU_PARAM_UNUSED( p_cgParameter ), T const * CU_PARAM_UNUSED( p_pValue ), uint32_t CU_PARAM_UNUSED( p_uiOcc ) )
{
	CASTOR_EXCEPTION( "ApplyVariable - Unsupported arguments" );
}

template< typename T, uint32_t Count > inline void ApplyVariable( CGparameter CU_PARAM_UNUSED( p_cgParameter ), T const * CU_PARAM_UNUSED( p_pValue ), uint32_t CU_PARAM_UNUSED( p_uiOcc ) )
{
	CASTOR_EXCEPTION( "ApplyVariable - Unsupported arguments" );
}

template< typename T, uint32_t Rows, uint32_t Columns > inline void ApplyVariable( CGparameter CU_PARAM_UNUSED( p_cgParameter ), T const * CU_PARAM_UNUSED( p_pValue ), uint32_t CU_PARAM_UNUSED( p_uiOcc ) )
{
	CASTOR_EXCEPTION( "ApplyVariable - Unsupported arguments" );
}

//***********************************************************************************************************************

template< typename T >
CgOneFrameVariable< T > :: CgOneFrameVariable( CgShaderProgram * p_pProgram, uint32_t p_uiOcc )
	:	CgFrameVariable					( 						)
	,	Castor3D::OneFrameVariable< T >	( p_pProgram, p_uiOcc	)
{
}

template< typename T >
CgOneFrameVariable< T > :: CgOneFrameVariable( CgOneFrameVariable< T > const & p_variable )
	:	CgFrameVariable					( p_variable	)
	,	Castor3D::OneFrameVariable< T >	( p_variable	)
{
}

template< typename T >
CgOneFrameVariable< T > :: CgOneFrameVariable( CgOneFrameVariable< T > && p_variable )
	:	CgFrameVariable					( p_variable	)
	,	Castor3D::OneFrameVariable< T >	( p_variable	)
{
}

template< typename T >
CgOneFrameVariable< T > :: ~CgOneFrameVariable()
{
}

template< typename T >
void CgOneFrameVariable< T > :: SetStage( Castor3D::ShaderObjectBaseSPtr p_pStage )
{
//	Castor3D::OneFrameVariable< T >::SetStage( p_pStage );
	SetProgram( std::static_pointer_cast< CgShaderObject >( p_pStage )->GetProgram() );
}

//***********************************************************************************************************************

template< typename T, uint32_t Count >
CgPointFrameVariable< T, Count > :: CgPointFrameVariable( CgShaderProgram * p_pProgram, uint32_t p_uiOcc )
	:	CgFrameVariable								(						)
	,	Castor3D::PointFrameVariable< T, Count >	( p_pProgram, p_uiOcc	)
{
}

template< typename T, uint32_t Count >
CgPointFrameVariable< T, Count > :: CgPointFrameVariable( CgPointFrameVariable< T, Count > const & p_variable )
	:	CgFrameVariable								( p_variable	)
	,	Castor3D::PointFrameVariable< T, Count >	( p_variable	)
{
}

template< typename T, uint32_t Count >
CgPointFrameVariable< T, Count > :: CgPointFrameVariable( CgPointFrameVariable< T, Count > && p_variable )
	:	CgFrameVariable								( p_variable	)
	,	Castor3D::PointFrameVariable< T, Count >	( p_variable	)
{
}

template< typename T, uint32_t Count >
CgPointFrameVariable< T, Count > :: ~CgPointFrameVariable()
{
}

template< typename T, uint32_t Count >
void CgPointFrameVariable< T, Count > :: SetStage( Castor3D::ShaderObjectBaseSPtr p_pStage )
{
//	Castor3D::PointFrameVariable< T, Count >::SetStage( p_pStage );
	SetProgram( std::static_pointer_cast< CgShaderObject >( p_pStage )->GetProgram() );
}

//***********************************************************************************************************************

template< typename T, uint32_t Rows, uint32_t Columns >
CgMatrixFrameVariable< T, Rows, Columns > :: CgMatrixFrameVariable( CgShaderProgram * p_pProgram, uint32_t p_uiOcc )
	:	CgFrameVariable										(						)
	,	Castor3D::MatrixFrameVariable< T, Rows, Columns >	( p_pProgram, p_uiOcc	)
{
}

template< typename T, uint32_t Rows, uint32_t Columns >
CgMatrixFrameVariable< T, Rows, Columns > :: CgMatrixFrameVariable( CgMatrixFrameVariable< T, Rows, Columns > const & p_variable )
	:	CgFrameVariable										( p_variable	)
	,	Castor3D::MatrixFrameVariable< T, Rows, Columns >	( p_variable	)
{
}

template< typename T, uint32_t Rows, uint32_t Columns >
CgMatrixFrameVariable< T, Rows, Columns > :: CgMatrixFrameVariable( CgMatrixFrameVariable< T, Rows, Columns > && p_variable )
	:	CgFrameVariable										( p_variable	)
	,	Castor3D::MatrixFrameVariable< T, Rows, Columns >	( p_variable	)
{
}

template< typename T, uint32_t Rows, uint32_t Columns >
CgMatrixFrameVariable< T, Rows, Columns > :: ~CgMatrixFrameVariable()
{
}

template< typename T, uint32_t Rows, uint32_t Columns >
void CgMatrixFrameVariable< T, Rows, Columns > :: SetStage( Castor3D::ShaderObjectBaseSPtr p_pStage )
{
//	Castor3D::MatrixFrameVariable< T, Rows, Columns >::SetStage( p_pStage );
	SetProgram( std::static_pointer_cast< CgShaderObject >( p_pStage )->GetProgram() );
}

//***********************************************************************************************************************
