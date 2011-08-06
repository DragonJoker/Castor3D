#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/ShaderObject.hpp"
#include "Castor3D/ShaderProgram.hpp"
#include "Castor3D/CgFrameVariable.hpp"
#include "Castor3D/ShaderProgram.hpp"

using namespace Castor3D;

CgShaderObject :: CgShaderObject( CgShaderProgram * p_pParent, eSHADER_TYPE p_eType)
	:	ShaderObjectBase( p_pParent, p_eType),
		m_cgProfile( CG_PROFILE_UNKNOWN),
		m_cgProgram( nullptr)
{
}

CgShaderObject :: ~CgShaderObject()
{
}

BEGIN_SERIALISE_MAP( CgShaderObject, Serialisable)
END_SERIALISE_MAP()