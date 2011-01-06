#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/shader/Cg/CgShaderProgram.h"
#include "Castor3D/shader/Cg/CgShaderObject.h"
#include "Castor3D/shader/Cg/CgFrameVariable.h"
#include "Castor3D/render_system/RenderSystem.h"

using namespace Castor3D;

CgShaderProgram :: CgShaderProgram( const String & p_vertexShaderFile, const String & p_fragmentShaderFile, const String & p_geometryShaderFile)
	:	ShaderProgram( p_vertexShaderFile, p_fragmentShaderFile, p_geometryShaderFile),
		m_cgContext( NULL)
{
	m_eType = eCgShader;
	m_pShaders[eVertexShader] = CgShaderObjectPtr( RenderSystem::GetSingletonPtr()->CreateCgVertexShader());
	m_pShaders[eVertexShader]->SetParent( this);
	m_pShaders[ePixelShader] = CgShaderObjectPtr( RenderSystem::GetSingletonPtr()->CreateCgFragmentShader());
	m_pShaders[ePixelShader]->SetParent( this);

	if (RenderSystem::HasGeometryShader())
	{
		m_pShaders[eGeometryShader] = CgShaderObjectPtr( RenderSystem::GetSingletonPtr()->CreateCgGeometryShader());
		m_pShaders[eGeometryShader]->SetParent( this);
	}
}

CgShaderProgram :: CgShaderProgram()
	:	ShaderProgram()
{
}

CgShaderProgram :: ~CgShaderProgram()
{
}

void CgShaderProgram :: Initialise()
{
	ShaderProgram::Initialise();
}

void CgShaderProgram :: Cleanup()
{
	ShaderProgram::Cleanup();
}