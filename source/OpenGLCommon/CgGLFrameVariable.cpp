#include "OpenGlCommon/PrecompiledHeader.h"

#include "OpenGlCommon/CgGlFrameVariable.h"

using namespace Castor3D;

void CgGlFrameVariableBase :: GetVariableLocation( const char * p_pVarName)
{
	CheckCgError( m_cgParameter = cgGetNamedParameter( m_cgProgram, p_pVarName), CU_T( "CgGlFrameVariableBase :: GetVariableLocation - cgGetNamedParameter - ") + String( p_pVarName));
}
