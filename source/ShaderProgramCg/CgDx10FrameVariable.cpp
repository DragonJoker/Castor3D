#include "ShaderProgramCg/PrecompiledHeader.hpp"

#if defined( Cg_Direct3D10 )
#include "ShaderProgramCg/CgDx10FrameVariable.hpp"

using namespace Castor3D;
using namespace Castor;
using namespace CgShader;

CgD3D10FrameVariableBase :: CgD3D10FrameVariableBase()
	:	m_bPresentInProgram	( true	)
	,	m_cgParameter		( NULL	)
{
}

CgD3D10FrameVariableBase :: ~CgD3D10FrameVariableBase()
{
}

void CgD3D10FrameVariableBase :: GetVariableLocation( char const * p_pVarName, CGprogram p_cgProgram)
{
	CheckCgError( m_cgParameter = cgGetNamedParameter( p_cgProgram, p_pVarName), str_utils::from_str( "CgD3D10FrameVariableBase :: GetVariableLocation - cgGetNamedParameter - " + std::string( p_pVarName ) ) );
}
#endif
