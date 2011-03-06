#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/shader/ShaderObject.h"
#include "Castor3D/shader/FrameVariable.h"

using namespace Castor3D;

GlslShaderObject :: GlslShaderObject( eSHADER_PROGRAM_TYPE p_eType)
	:	ShaderObject<GlslShaderObject>( p_eType)
{}

GlslShaderObject :: ~GlslShaderObject()
{
}

void GlslShaderObject :: AddFrameVariable( FrameVariablePtr p_pVariable)
{
	ShaderObject<GlslShaderObject>::AddFrameVariable( p_pVariable);
}
