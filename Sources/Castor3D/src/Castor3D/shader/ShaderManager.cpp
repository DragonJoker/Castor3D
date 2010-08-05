#include "PrecompiledHeader.h"

#include "shader/Module_Shader.h"

#include "shader/ShaderManager.h"
#include "shader/ShaderProgram.h"
#include "shader/ShaderObject.h"
#include "render_system/RenderSystem.h"
#include "Log.h"

using namespace Castor3D;


ShaderManager :: ShaderManager()
{
	m_inputPrimitiveType = DTTriangles;
	m_outputPrimitiveType = DTTriangleStrip;
	m_nbVerticesOut = 3;
}

ShaderManager :: ~ShaderManager()
{
	vector::deleteAll( m_shaderPrograms);
}

void ShaderManager :: Update()
{
	vector::deleteAll( m_arrayToDelete);
}

void ShaderManager :: ClearShaders()
{
	for (size_t i = 0 ; i < m_shaderPrograms.size() ; i++)
	{
		m_arrayToDelete.push_back( m_shaderPrograms[i]);
	}

	m_shaderPrograms.clear();
}

void ShaderManager :: AddProgram( ShaderProgram * p_program)
{
	m_shaderPrograms.push_back( p_program);
}

bool ShaderManager :: RemoveProgram( ShaderProgram * p_program)
{
	bool l_bReturn = false;

	if (vector::eraseValue( m_shaderPrograms, p_program))
	{
		m_arrayToDelete.push_back( p_program);
		l_bReturn = true;
	}

	return l_bReturn;
}