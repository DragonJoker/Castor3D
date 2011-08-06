#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/ShaderProgram.hpp"
#include "Castor3D/ShaderObject.hpp"
#include "Castor3D/CgFrameVariable.hpp"
#include "Castor3D/RenderSystem.hpp"

using namespace Castor3D;

//*************************************************************************************************

CgShaderProgram :: CgShaderProgram( const StringArray & p_shaderFiles)
	:	ShaderProgramBase( eSHADER_LANGUAGE_CG, p_shaderFiles)
{
}

CgShaderProgram :: CgShaderProgram()
	:	ShaderProgramBase( eSHADER_LANGUAGE_CG)
{
}

CgShaderProgram :: ~CgShaderProgram()
{
}

//*************************************************************************************************