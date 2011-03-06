#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/shader/ShaderObject.h"
#include "Castor3D/shader/FrameVariable.h"

using namespace Castor3D;

HlslShaderObject :: HlslShaderObject( eSHADER_PROGRAM_TYPE p_eType)
	:	ShaderObject<HlslShaderObject>( p_eType)
{}

HlslShaderObject :: ~HlslShaderObject()
{
}

void HlslShaderObject :: AddFrameVariable( FrameVariablePtr p_pVariable)
{
	ShaderObject<HlslShaderObject>::AddFrameVariable( p_pVariable);
}
