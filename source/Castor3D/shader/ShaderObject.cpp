#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/shader/ShaderObject.h"
#include "Castor3D/shader/FrameVariable.h"

using namespace Castor3D;

ShaderObject :: ShaderObject()
	:	m_isCompiled( false)
	,	m_programType( eShaderNone)
	,	m_bError( false)
{}

ShaderObject :: ~ShaderObject()
{
}

void ShaderObject :: SetFile( const String & p_filename)
{
	m_bError = false;

	m_strFile.clear();
	m_shaderSource.clear();

	if ( ! p_filename.empty())
	{
		File l_file( p_filename.c_str(), File::eRead);

		if (l_file.IsOk())
		{
			size_t l_len = size_t( l_file.GetLength());

			if (l_len > 0)
			{
				m_strFile = p_filename;
				l_file.CopyToString( m_shaderSource);
			}
		}
	}
}

void ShaderObject :: SetSource( const String & p_program)
{
	m_bError = false;
	m_shaderSource = p_program;
}

void ShaderObject :: AddFrameVariable( FrameVariablePtr p_pVariable)
{
	if ( ! p_pVariable == NULL)
	{
		if (m_mapFrameVariables.find( p_pVariable->GetName()) == m_mapFrameVariables.end())
		{
			m_mapFrameVariables.insert( FrameVariablePtrStrMap::value_type( p_pVariable->GetName(), p_pVariable));
		}
	}
}