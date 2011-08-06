#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/ShaderProgram.hpp"
#include "Castor3D/ShaderObject.hpp"

using namespace Castor3D;

GlslShaderProgram :: GlslShaderProgram( const StringArray & p_shaderFiles)
	:	ShaderProgramBase( eSHADER_LANGUAGE_GLSL, p_shaderFiles)
{
}

GlslShaderProgram :: GlslShaderProgram()
	:	ShaderProgramBase( eSHADER_LANGUAGE_GLSL)
{
}

GlslShaderProgram :: ~GlslShaderProgram()
{
}