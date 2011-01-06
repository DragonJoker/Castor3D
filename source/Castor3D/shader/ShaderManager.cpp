#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/shader/ShaderManager.h"
#include "Castor3D/shader/ShaderProgram.h"
#include "Castor3D/shader/ShaderObject.h"
#include "Castor3D/shader/Cg/CgShaderProgram.h"
#include "Castor3D/shader/Cg/CgShaderObject.h"
#include "Castor3D/render_system/RenderSystem.h"



using namespace Castor3D;

ShaderManager :: ShaderManager()
{
	m_inputPrimitiveType = eTriangles;
	m_outputPrimitiveType = eTriangles;
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
	for (size_t i = 0 ; i < m_shaderPrograms.size() ; i++)
	{
		m_arrayToDelete.push_back( m_shaderPrograms[i]);
	}

	m_shaderPrograms.clear();
}

ShaderProgramPtr ShaderManager :: CreateShaderProgramFromFiles( const String & p_vertexFile, const String & p_fragmentFile, const String & p_geometryFile)
{
	ShaderProgramPtr l_pProgram( RenderSystem::GetSingletonPtr()->CreateShaderProgram( p_vertexFile, p_fragmentFile, p_geometryFile));
	m_shaderPrograms.push_back( l_pProgram);
	return l_pProgram;
}

CgShaderProgramPtr ShaderManager :: CreateCgShaderProgramFromFiles( const String & p_vertexFile, const String & p_fragmentFile, const String & p_geometryFile)
{
	CgShaderProgramPtr l_pProgram( RenderSystem::GetSingletonPtr()->CreateCgShaderProgram( p_vertexFile, p_fragmentFile, p_geometryFile));
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