#include "GlRenderSystem/PrecompiledHeader.hpp"

#include "GlRenderSystem/CgGlFrameVariable.hpp"

using namespace Castor3D;

void CgGlFrameVariableBase :: GetVariableLocation( char const * p_pVarName, CGprogram p_cgProgram)
{
	cgCheckError( m_cgParameter = cgGetNamedParameter( p_cgProgram, p_pVarName), cuT( "CgGlFrameVariableBase :: GetVariableLocation - cgGetNamedParameter - ") + String( p_pVarName));
}
