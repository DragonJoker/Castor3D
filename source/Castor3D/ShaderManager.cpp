#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/ShaderManager.hpp"
#include "Castor3D/ShaderProgram.hpp"
#include "Castor3D/ShaderObject.hpp"

using namespace Castor3D;

ShaderProgramPtrArray ShaderManager :: m_arrayPrograms;
ShaderProgramPtrArray ShaderManager :: m_arrayToDelete;
int ShaderManager :: m_nbVerticesOut = 0;

ShaderManager :: ShaderManager()
{
}

ShaderManager :: ~ShaderManager()
{
}

void ShaderManager :: Update()
{
	m_arrayToDelete.clear();
}

void ShaderManager :: Clear()
{
	ClearShaders();
	Update();
}

void ShaderManager :: ClearShaders()
{
	for (size_t i = 0 ; i < m_arrayPrograms.size() ; i++)
	{
		m_arrayToDelete.push_back( m_arrayPrograms[i]);
	}

	m_arrayPrograms.clear();
}

void ShaderManager :: AddProgram( ShaderProgramPtr p_program)
{
	m_arrayPrograms.push_back( p_program);
}

bool ShaderManager :: RemoveProgram( ShaderProgramPtr p_program)
{
	bool l_bReturn = false;

	if (vector::eraseValue( m_arrayPrograms, p_program))
	{
		m_arrayToDelete.push_back( p_program);
		l_bReturn = true;
	}

	return l_bReturn;
}