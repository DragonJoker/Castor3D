#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/ShaderObject.hpp"
#include "Castor3D/ShaderProgram.hpp"
#include "Castor3D/FrameVariable.hpp"
#include "Castor3D/ShaderProgram.hpp"

using namespace Castor3D;

HlslShaderObject :: HlslShaderObject( HlslShaderProgram * p_pParent, eSHADER_TYPE p_eType)
	:	ShaderObjectBase( p_pParent, p_eType)
{}

HlslShaderObject :: ~HlslShaderObject()
{
}

BEGIN_SERIALISE_MAP( HlslShaderObject, Serialisable)
END_SERIALISE_MAP()