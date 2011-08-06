#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/ShaderProgram.hpp"
#include "Castor3D/ShaderObject.hpp"

using namespace Castor3D;

HlslShaderProgram :: HlslShaderProgram ( const StringArray & p_shaderFiles)
	:	ShaderProgramBase( eSHADER_LANGUAGE_HLSL, p_shaderFiles)
{
}

HlslShaderProgram  :: HlslShaderProgram ()
	:	ShaderProgramBase( eSHADER_LANGUAGE_HLSL)
{
}

HlslShaderProgram  :: ~HlslShaderProgram ()
{
}