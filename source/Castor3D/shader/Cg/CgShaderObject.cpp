#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/shader/Cg/CgShaderObject.h"
#include "Castor3D/shader/Cg/CgFrameVariable.h"

using namespace Castor3D;

CgShaderObject :: CgShaderObject()
	:	ShaderObject(),
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
		static_pointer_cast<CgFrameVariable>( it->second)->SetProgram( m_cgProgram);
	}

	return true;
}