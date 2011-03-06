#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/shader/ShaderProgram.h"
#include "Castor3D/shader/ShaderObject.h"
#include "Castor3D/shader/Cg/CgFrameVariable.h"
#include "Castor3D/render_system/RenderSystem.h"

using namespace Castor3D;

CgShaderProgram :: CgShaderProgram( const String & p_vertexShaderFile, const String & p_fragmentShaderFile, const String & p_geometryShaderFile)
	:	ShaderProgram<CgShaderObject>( eCgShader, p_vertexShaderFile, p_fragmentShaderFile, p_geometryShaderFile)
	,	m_cgContext( NULL)
{
}

CgShaderProgram :: CgShaderProgram()
	:	ShaderProgram<CgShaderObject>( eCgShader)
{
}

CgShaderProgram :: ~CgShaderProgram()
{
}

void CgShaderProgram :: Initialise()
{
	if ( ! m_isLinked && ! m_bError)
	{
		Cleanup();

		ShaderProgram<CgShaderObject>::Initialise();
	}
}

void CgShaderProgram :: Cleanup()
{
	ShaderProgram<CgShaderObject>::Cleanup();
}
