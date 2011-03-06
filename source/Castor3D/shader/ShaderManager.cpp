#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/shader/ShaderManager.h"
#include "Castor3D/shader/ShaderProgram.h"
#include "Castor3D/shader/ShaderObject.h"
//#include "Castor3D/render_system/RenderSystem.h"



using namespace Castor3D;

ShaderManager :: ShaderManager()
{
}

ShaderManager :: ~ShaderManager()
{
	m_arrayGLSLPrograms.clear();
	m_arrayHLSLPrograms.clear();
	m_arrayCgPrograms.clear();
}

void ShaderManager :: Update()
{
	m_arrayGLSLToDelete.clear();
	m_arrayHLSLToDelete.clear();
	m_arrayCgToDelete.clear();
}

void ShaderManager :: ClearShaders()
{
	for (size_t i = 0 ; i < m_arrayGLSLPrograms.size() ; i++)
	{
		m_arrayGLSLToDelete.push_back( m_arrayGLSLPrograms[i]);
	}

	for (size_t i = 0 ; i < m_arrayHLSLPrograms.size() ; i++)
	{
		m_arrayHLSLToDelete.push_back( m_arrayHLSLPrograms[i]);
	}

	for (size_t i = 0 ; i < m_arrayCgPrograms.size() ; i++)
	{
		m_arrayCgToDelete.push_back( m_arrayCgPrograms[i]);
	}

	m_arrayGLSLPrograms.clear();
	m_arrayHLSLPrograms.clear();
	m_arrayCgPrograms.clear();
}

void ShaderManager :: AddGLSLProgram( GlslShaderProgramPtr p_program)
{
	m_arrayGLSLPrograms.push_back( p_program);
}

void ShaderManager :: AddHLSLProgram( HlslShaderProgramPtr p_program)
{
	m_arrayHLSLPrograms.push_back( p_program);
}

void ShaderManager :: AddCgProgram( CgShaderProgramPtr p_program)
{
	m_arrayCgPrograms.push_back( p_program);
}

bool ShaderManager :: RemoveGLSLProgram( GlslShaderProgramPtr p_program)
{
	bool l_bReturn = false;

	if (vector::eraseValue( m_arrayGLSLPrograms, p_program))
	{
		m_arrayGLSLToDelete.push_back( p_program);
		l_bReturn = true;
	}

	return l_bReturn;
}

bool ShaderManager :: RemoveHLSLProgram( HlslShaderProgramPtr p_program)
{
	bool l_bReturn = false;

	if (vector::eraseValue( m_arrayHLSLPrograms, p_program))
	{
		m_arrayHLSLToDelete.push_back( p_program);
		l_bReturn = true;
	}

	return l_bReturn;
}

bool ShaderManager :: RemoveCgProgram( CgShaderProgramPtr p_program)
{
	bool l_bReturn = false;

	if (vector::eraseValue( m_arrayCgPrograms, p_program))
	{
		m_arrayCgToDelete.push_back( p_program);
		l_bReturn = true;
	}

	return l_bReturn;
}
