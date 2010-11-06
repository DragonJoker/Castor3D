#include "PrecompiledHeader.h"

#include "shader/Module_Shader.h"

#include "shader/ShaderManager.h"
#include "shader/ShaderProgram.h"
#include "shader/ShaderObject.h"
#include "render_system/RenderSystem.h"



using namespace Castor3D;

ShaderManager :: ShaderManager()
{
	m_inputPrimitiveType = DTTriangles;
	m_outputPrimitiveType = DTTriangleStrip;
	m_nbVerticesOut = 3;
}

ShaderManager :: ~ShaderManager()
{
	m_shaderPrograms.clear();
}

void ShaderManager :: Update()
{
	m_arrayToDelete.clear();
}

void ShaderManager :: ClearShaders()
{
	ShaderManager & l_pThis = GetSingleton();

	for (size_t i = 0 ; i < l_pThis.m_shaderPrograms.size() ; i++)
	{
		l_pThis.m_arrayToDelete.push_back( l_pThis.m_shaderPrograms[i]);
	}

	l_pThis.m_shaderPrograms.clear();
}

ShaderProgramPtr ShaderManager :: CreateShaderProgramFromFiles( const String & p_vertexFile, const String & p_fragmentFile, const String & p_geometryFile)
{
	ShaderProgramPtr l_pProgram = RenderSystem::GetSingletonPtr<RenderSystem>()->CreateShaderProgram( p_vertexFile, p_fragmentFile, p_geometryFile);
	m_shaderPrograms.push_back( l_pProgram);
	return l_pProgram;
}

bool ShaderManager :: RemoveProgram( ShaderProgramPtr p_program)
{
	bool l_bReturn = false;

	if (vector::eraseValue( m_shaderPrograms, p_program))
	{
		m_arrayToDelete.push_back( p_program);
		l_bReturn = true;
	}

	return l_bReturn;
}