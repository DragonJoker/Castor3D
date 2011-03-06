#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/shader/ShaderProgram.h"
#include "Castor3D/shader/ShaderObject.h"

using namespace Castor3D;

HlslShaderProgram :: HlslShaderProgram ( const String & p_vertexShaderFile, const String & p_fragmentShaderFile, const String & p_geometryShaderFile)
	:	ShaderProgram<HlslShaderObject>( eHlslShader, p_vertexShaderFile, p_fragmentShaderFile, p_geometryShaderFile)
{
}

HlslShaderProgram  :: HlslShaderProgram ()
	:	ShaderProgram<HlslShaderObject>( eHlslShader)
{
}

HlslShaderProgram  :: ~HlslShaderProgram ()
{
}

void HlslShaderProgram  :: Cleanup()
{
	ShaderProgram<HlslShaderObject>::Cleanup();
}

void HlslShaderProgram  :: Initialise()
{
	if ( ! m_isLinked && ! m_bError)
	{
		Cleanup();

		ShaderProgram<HlslShaderObject>::Initialise();
	}
}
