#include "ShaderProgramCg/PrecompiledHeader.hpp"

#if defined( Cg_Direct3D11 )
#include "ShaderProgramCg/CgDx11FrameVariable.hpp"

using namespace Castor3D;
using namespace Castor;
using namespace CgShader;

CgD3D11FrameVariableBase :: CgD3D11FrameVariableBase()
	:	m_bPresentInProgram	( true	)
	,	m_cgParameter		( NULL	)
{
}

CgD3D11FrameVariableBase :: ~CgD3D11FrameVariableBase()
{
}

void CgD3D11FrameVariableBase :: GetVariableLocation( char const * p_pVarName, CGprogram p_cgProgram)
{
	CheckCgError( m_cgParameter = cgGetNamedParameter( p_cgProgram, p_pVarName), str_utils::from_str( "CgD3D11FrameVariableBase :: GetVariableLocation - cgGetNamedParameter - " + std::string( p_pVarName ) ) );
}
#endif
