#include "ShaderProgramCg/PrecompiledHeader.hpp"

#if defined( Cg_Direct3D9 )
#include "ShaderProgramCg/CgDx9FrameVariable.hpp"

using namespace Castor3D;
using namespace Castor;
using namespace CgShader;

CgD3D9FrameVariableBase :: CgD3D9FrameVariableBase()
	:	m_bPresentInProgram	( true	)
	,	m_cgParameter		( NULL	)
{
}

CgD3D9FrameVariableBase :: ~CgD3D9FrameVariableBase()
{
}

void CgD3D9FrameVariableBase :: GetVariableLocation( char const * p_pVarName, CGprogram p_cgProgram)
{
	CheckCgError( m_cgParameter = cgGetNamedParameter( p_cgProgram, p_pVarName), str_utils::from_str( "CgD3D9FrameVariableBase :: GetVariableLocation - cgGetNamedParameter - " + std::string( p_pVarName ) ) );
}
#endif
