#include "ShaderProgramCg/PrecompiledHeader.hpp"

#if defined( Cg_OpenGL )
#include "ShaderProgramCg/CgGlFrameVariable.hpp"

using namespace CgShader;
using namespace Castor3D;
using namespace Castor;

CgGlFrameVariableBase :: CgGlFrameVariableBase()
	:	m_bPresentInProgram	( true	)
	,	m_cgParameter		( NULL	)
{
}

CgGlFrameVariableBase :: ~CgGlFrameVariableBase()
{
}

void CgGlFrameVariableBase :: GetVariableLocation( char const * p_pVarName, CGprogram p_cgProgram)
{
	CheckCgError( m_cgParameter = cgGetNamedParameter( p_cgProgram, p_pVarName), str_utils::from_str( "CgGlFrameVariableBase :: GetVariableLocation - cgGetNamedParameter - " + std::string( p_pVarName ) ) );
}
#endif