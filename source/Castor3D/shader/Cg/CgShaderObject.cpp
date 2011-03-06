#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/shader/ShaderObject.h"
#include "Castor3D/shader/Cg/CgFrameVariable.h"

using namespace Castor3D;

CgShaderObject :: CgShaderObject( eSHADER_PROGRAM_TYPE p_eType)
	:	ShaderObject<CgShaderObject>( p_eType),
		m_cgProfile( CG_PROFILE_UNKNOWN),
		m_cgProgram( NULL)
{
}

CgShaderObject :: ~CgShaderObject()
{
}

bool CgShaderObject :: Compile()
{
	for (FrameVariablePtrStrMap::iterator it = m_mapFrameVariables.begin() ; it != m_mapFrameVariables.end() ; ++it)
	{
		reinterpret_cast<CgFrameVariable *>( it->second.get())->SetProgram( m_cgProgram);
	}

	return true;
}
