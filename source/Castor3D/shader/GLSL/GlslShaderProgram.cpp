#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/shader/ShaderProgram.h"
#include "Castor3D/shader/ShaderObject.h"

using namespace Castor3D;

GlslShaderProgram :: GlslShaderProgram( const String & p_vertexShaderFile, const String & p_fragmentShaderFile, const String & p_geometryShaderFile)
	:	ShaderProgram<GlslShaderObject>( eGlslShader, p_vertexShaderFile, p_fragmentShaderFile, p_geometryShaderFile)
{
}

GlslShaderProgram :: GlslShaderProgram()
	:	ShaderProgram<GlslShaderObject>( eGlslShader)
{
}

GlslShaderProgram :: ~GlslShaderProgram()
{
}

void GlslShaderProgram :: Cleanup()
{
	ShaderProgram<GlslShaderObject>::Cleanup();
}

void GlslShaderProgram :: Initialise()
{
	if ( ! m_isLinked && ! m_bError)
	{
		Cleanup();

		ShaderProgram<GlslShaderObject>::Initialise();
	}
}
