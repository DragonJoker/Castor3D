#include "Dx9RenderSystem/PrecompiledHeader.h"

#include "Dx9RenderSystem/CgDx9FrameVariable.h"

using namespace Castor3D;

void CgDx9FrameVariableBase :: GetVariableLocation( const char * p_pVarName)
{
	m_cgParameter = cgGetNamedParameter( m_cgProgram, p_pVarName);
	CheckCgError( CU_T( "CgDx9FrameVariableBase :: GetVariableLocation - cgGetNamedParameter - ") + String( p_pVarName));
}