#include "Dx9RenderSystem/PrecompiledHeader.hpp"

#include "Dx9RenderSystem/CgDx9FrameVariable.hpp"

using namespace Castor3D;

void CgDx9FrameVariableBase :: GetVariableLocation( char const * p_pVarName, CGprogram p_cgProgram)
{
	m_cgParameter = cgGetNamedParameter( p_cgProgram, p_pVarName);
	CheckCgError( cuT( "CgDx9FrameVariableBase :: GetVariableLocation - cgGetNamedParameter - ") + String( p_pVarName));
}