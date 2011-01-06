#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/shader/ShaderProgram.h"
#include "Castor3D/shader/ShaderManager.h"
#include "Castor3D/shader/ShaderObject.h"
#include "Castor3D/shader/FrameVariable.h"
#include "Castor3D/render_system/RenderSystem.h"

using namespace Castor3D;

ShaderProgram :: ShaderProgram( const String & p_vertexShaderFile, const String & p_fragmentShaderFile, const String & p_geometryShaderFile)
	:	m_isLinked( false)
	,	m_bError( false)
	,	m_enabled( true)
	,	m_usesGeometryShader( false)
{
	m_pShaders[eVertexShader] = ShaderObjectPtr( RenderSystem::GetSingletonPtr()->CreateVertexShader());
	m_pShaders[eVertexShader]->SetParent( this);
	m_pShaders[ePixelShader] = ShaderObjectPtr( RenderSystem::GetSingletonPtr()->CreateFragmentShader());
	m_pShaders[ePixelShader]->SetParent( this);

	if (RenderSystem::HasGeometryShader())
	{
		m_pShaders[eGeometryShader] = ShaderObjectPtr( RenderSystem::GetSingletonPtr()->CreateGeometryShader());
		m_pShaders[eGeometryShader]->SetParent( this);
	}

	SetVertexFile( p_vertexShaderFile);
	SetFragmentFile( p_fragmentShaderFile);
	SetGeometryFile( p_geometryShaderFile);
}

ShaderProgram :: ShaderProgram()
	:	m_isLinked( false)
	,	m_bError( false)
	,	m_enabled( true)
	,	m_usesGeometryShader( false)
{
}

ShaderProgram :: ~ShaderProgram()
{
	for (int i = eVertexShader ; i < eNbShaderTypes ; i++)
	{
		m_pShaders[i].reset();
	}

	m_mapFrameVariables.clear();
}

void ShaderProgram :: Cleanup()
{
	for (int i = eVertexShader ; i < eNbShaderTypes ; i++)
	{
		m_pShaders[i]->DestroyProgram();
	}

	m_mapFrameVariables.clear();
}

void ShaderProgram :: AddFrameVariable( FrameVariablePtr p_pVariable, ShaderObjectPtr p_pObject)
{
	if ( ! p_pVariable == NULL)
	{
		if (m_mapFrameVariables.find( p_pVariable->GetName()) == m_mapFrameVariables.end())
		{
			m_mapFrameVariables.insert( FrameVariablePtrStrMap::value_type( p_pVariable->GetName(), p_pVariable));
		}
	}

	p_pObject->AddFrameVariable( p_pVariable);
}

void ShaderProgram :: AddFrameVariable( FrameVariablePtr p_pVariable, eSHADER_PROGRAM_TYPE p_eType)
{
	CASTOR_ASSERT( p_eType > eShaderNone && p_eType < eNbShaderTypes);
	AddFrameVariable( p_pVariable, m_pShaders[p_eType]);
}

void ShaderProgram :: Initialise()
{
	if ( ! m_isLinked && ! m_bError)
	{
		Cleanup();

		m_usesGeometryShader = ! m_pShaders[eGeometryShader] == NULL && ! m_pShaders[eGeometryShader]->GetSource().empty();

		for (int i = eVertexShader ; i < eNbShaderTypes ; i++)
		{
			if ( ! m_pShaders[i] == NULL && ! m_pShaders[i]->GetSource().empty())
			{
				m_pShaders[i]->CreateProgram();

				if ( ! m_pShaders[i]->Compile())
				{
					Logger::LogError( CU_T( "ShaderProgram :: LoadfromMemory - ***COMPILER ERROR (Shader %i)"), i);
					Cleanup();
					return;
				}
			}
		}

		if ( ! Link())
		{
			Logger::LogError( CU_T( "ShaderProgram :: LoadfromMemory - **LINKER ERROR"));
			Cleanup();
			return;
		}

		Logger::LogMessage( CU_T( "ShaderProgram :: LoadfromMemory - ***Program Linked successfully"));
	}
}

void ShaderProgram :: SetVertexFile( const String & p_strFile)
{
	_setProgramFile( p_strFile, eVertexShader);
}

void ShaderProgram :: SetFragmentFile( const String & p_strFile)
{
	_setProgramFile( p_strFile, ePixelShader);
}

void ShaderProgram :: SetGeometryFile( const String & p_strFile)
{
	if (RenderSystem::HasGeometryShader())
	{
		_setProgramFile( p_strFile, eGeometryShader);
	}
}

void ShaderProgram :: SetVertexText( const String & p_strContent)
{
	_setProgramText( p_strContent, eVertexShader);
}

void ShaderProgram :: SetFragmentText( const String & p_strContent)
{
	_setProgramText( p_strContent, ePixelShader);
}

void ShaderProgram :: SetGeometryText( const String & p_strContent)
{
	if (RenderSystem::HasGeometryShader())
	{
		_setProgramText( p_strContent, eGeometryShader);
	}
}

void ShaderProgram :: _setProgramFile( const String & p_strFile, eSHADER_PROGRAM_TYPE p_eType)
{
	m_pShaders[eVertexShader]->SetFile( p_strFile);
	m_isLinked = false;
	m_bError = false;
}

void ShaderProgram :: _setProgramText( const String & p_strContent, eSHADER_PROGRAM_TYPE p_eType)
{
	m_pShaders[p_eType]->SetFile( C3DEmptyString);
	m_pShaders[p_eType]->SetSource( p_strContent);
	m_isLinked = false;
	m_bError = false;
}