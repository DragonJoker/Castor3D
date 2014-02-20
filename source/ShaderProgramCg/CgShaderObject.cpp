#include "ShaderProgramCg/PrecompiledHeader.hpp"

#include "ShaderProgramCg/CgFrameVariable.hpp"

using namespace CgShader;
using namespace Castor3D;

CgShaderObject :: CgShaderObject( CgShaderProgram * p_pParent, eSHADER_TYPE p_eType )
	:	ShaderObjectBase	( p_pParent, p_eType	)
	,	m_cgProfile			( CG_PROFILE_UNKNOWN	)
	,	m_cgProgram			( NULL					)
{
}

CgShaderObject :: ~CgShaderObject()
{
}

bool CgShaderObject :: HasParameter( Castor::String const & p_strName )
{
	return GetParameter( p_strName ) != NULL;
}

CGparameter CgShaderObject :: GetParameter( Castor::String const & p_strName )
{
	CGparameter l_pReturn = NULL;

	if( m_eStatus == eSHADER_STATUS_COMPILED )
	{
		ParamStrMap::iterator l_it = m_mapParamsByName.find( p_strName );

		if( l_it == m_mapParamsByName.end() )
		{
			CheckCgError( m_mapParamsByName.insert( std::make_pair( p_strName, cgGetNamedParameter( m_cgProgram, Castor::str_utils::to_str( p_strName ).c_str() ) ) ), cuT( "CgGlShaderObject :: GetParameter - cgGetNamedParameter" ) );
			l_it = m_mapParamsByName.find( p_strName );
		}

		l_pReturn = l_it->second;
	}

	return l_pReturn;
}

void CgShaderObject :: SetParameter( Castor::String const & p_strName, Castor::Matrix4x4r const & p_mtxValue )
{
	CGparameter l_pParam = GetParameter( p_strName );

	if( l_pParam )
	{
#if CASTOR_USE_DOUBLE
		CheckCgError( cgSetMatrixParameterfr( l_pParam, Matrix4x4f( p_mtxValue ).const_ptr() ), cuT( "CgGlShaderObject :: SetParameter - cgSetMatrixParameterfr" ) );
#else
		CheckCgError( cgSetMatrixParameterfr( l_pParam, p_mtxValue.const_ptr() ), cuT( "CgGlShaderObject :: SetParameter - cgSetMatrixParameterfr" ) );
#endif
	}
}

void CgShaderObject :: SetParameter( Castor::String const & p_strName, Castor::Matrix3x3r const & p_mtxValue )
{
	CGparameter l_pParam = GetParameter( p_strName );

	if( l_pParam )
	{
#if CASTOR_USE_DOUBLE
		CheckCgError( cgSetMatrixParameterfr( l_pParam, Matrix3x3f( p_mtxValue ).const_ptr() ), cuT( "CgGlShaderObject :: SetParameter - cgSetMatrixParameterfr" ) );
#else
		CheckCgError( cgSetMatrixParameterfr( l_pParam, p_mtxValue.const_ptr() ), cuT( "CgGlShaderObject :: SetParameter - cgSetMatrixParameterfr" ) );
#endif
	}
}