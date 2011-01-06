#include "OpenGLCommon/PrecompiledHeader.h"

#include "OpenGLCommon/CgGLFrameVariable.h"

using namespace Castor3D;

void CgGLFrameVariableBase :: GetVariableLocation( const char * p_pVarName)
{
	m_cgParameter = cgGetNamedParameter( m_cgProgram, p_pVarName);
	CheckCgError( CU_T( "CgGLFrameVariableBase :: GetVariableLocation - cgGetNamedParameter - ") + String( p_pVarName));
}