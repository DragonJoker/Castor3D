#include "PrecompiledHeader.h"

#include "shader/Module_Shader.h"

#include "shader/ShaderProgram.h"
#include "shader/ShaderManager.h"
#include "shader/ShaderObject.h"
#include "shader/UniformVariable.h"
#include "render_system/RenderSystem.h"



using namespace Castor3D;

ShaderProgram :: ShaderProgram( const String & p_vertexShaderFile, const String & p_fragmentShaderFile, const String & p_geometryShaderFile)
	:	m_isLinked( false),
		m_bError( false),
		m_enabled( true),
		m_usesGeometryShader( false),
		m_vertexFile( p_vertexShaderFile),
		m_fragmentFile( p_fragmentShaderFile),
		m_geometryFile( p_geometryShaderFile),
		m_bFromMemory( false)
{
}

ShaderProgram :: ShaderProgram()
	:	m_isLinked( false),
		m_bError( false),
		m_enabled( true),
		m_usesGeometryShader( false),
		m_bFromMemory( true)
{
}

ShaderProgram :: ~ShaderProgram()
{
}

void ShaderProgram :: Cleanup()
{
	m_pVertexShader.reset();
	m_pFragmentShader.reset();
	m_pGeometryShader.reset();
}

void ShaderProgram :: AddUniformVariable( UniformVariablePtr p_pUniformVariable)
{
	if ( ! p_pUniformVariable.null())
	{
		if (m_mapUniformVariables.find( p_pUniformVariable->GetName()) == m_mapUniformVariables.end())
		{
			m_mapUniformVariables.insert( UniformVariablePtrStrMap::value_type( p_pUniformVariable->GetName(), p_pUniformVariable));
		}
	}
}

void ShaderProgram :: _initialiseFromMemory()
{
	if ( ! m_isLinked && ! m_bError)
	{
		Cleanup();

		m_usesGeometryShader = ! m_geometryFile.empty();

		m_pVertexShader = RenderSystem::GetSingletonPtr<RenderSystem>()->CreateVertexShader();
		m_pFragmentShader = RenderSystem::GetSingletonPtr<RenderSystem>()->CreateFragmentShader();

		if (m_usesGeometryShader)
		{
			m_pGeometryShader = RenderSystem::GetSingletonPtr<RenderSystem>()->CreateGeometryShader();
		}

		if ( ! m_vertexProgram.empty())
		{
			if (m_pVertexShader->LoadFromMemory( m_vertexProgram) != 0)
			{ 
				Log::LogMessage( "ShaderProgram :: LoadfromMemory - can't load vertex shader");
				Cleanup();
				return;
			}
		}

		if ( ! m_fragmentProgram.empty())
		{
			if (m_pFragmentShader->LoadFromMemory( m_fragmentProgram) != 0)
			{
				Log::LogMessage( "ShaderProgram :: LoadfromMemory - can't load fragment shader");
				Cleanup();
				return;
			}
		}

		if (m_usesGeometryShader)
		{
			if (m_pGeometryShader->LoadFromMemory( m_geometryProgram) != 0)
			{
				Log::LogMessage( "ShaderProgram :: LoadfromMemory - can't load fragment shader");
				Cleanup();
				return;
			}
		}

		if ( ! m_vertexProgram.empty())
		{
			if ( ! m_pVertexShader->Compile())
			{
				Log::LogMessage( "ShaderProgram :: LoadfromMemory - ***COMPILER ERROR (Vertex Shader)");
				Cleanup();
				return;
			}
			Log::LogMessage( "ShaderProgram :: LoadfromMemory - ***Vertex Shader compiled successfully");
		}

		if ( ! m_fragmentProgram.empty())
		{
			if ( ! m_pFragmentShader->Compile())
			{
				Log::LogMessage( "ShaderProgram :: LoadfromMemory - ***COMPILER ERROR (Fragment Shader)");
				Cleanup();
				return;

			}
			Log::LogMessage( "ShaderProgram :: LoadfromMemory - ***Fragment Shader compiled successfully");
		}

		if (m_usesGeometryShader)
		{
			if ( ! m_pGeometryShader->Compile())
			{
				Log::LogMessage( "ShaderProgram :: LoadfromMemory - ***COMPILER ERROR (Geometry Shader)");
				Cleanup();
				return;

			}
			Log::LogMessage( "ShaderProgram :: LoadfromMemory - ***Geometry Shader compiled successfully");
		}

		if ( ! Link())
		{
			Log::LogMessage( "ShaderProgram :: LoadfromMemory - **LINKER ERROR");
			Cleanup();
			return;
		}

		Log::LogMessage( "ShaderProgram :: LoadfromMemory - ***Program Linked successfully");
	}
}

void ShaderProgram :: _initialiseFromFile()
{
	if ( ! m_isLinked && ! m_bError)
	{
		Cleanup();

		m_usesGeometryShader = ! m_geometryFile.empty();

		m_pVertexShader = RenderSystem::GetSingletonPtr<RenderSystem>()->CreateVertexShader();
		m_pFragmentShader = RenderSystem::GetSingletonPtr<RenderSystem>()->CreateFragmentShader();

		if (m_usesGeometryShader)
		{
			m_pGeometryShader = RenderSystem::GetSingletonPtr<RenderSystem>()->CreateGeometryShader();
		}

		if ( ! m_vertexFile.empty())
		{
			if (m_pVertexShader->Load( m_vertexFile) != 0)
			{ 
				Log::LogMessage( "ShaderProgram :: LoadfromFile - can't load vertex shader");
				Cleanup();
				return;
			}
		}

		if ( ! m_fragmentFile.empty())
		{
			if (m_pFragmentShader->Load( m_fragmentFile) != 0)
			{
				Log::LogMessage( "ShaderProgram :: LoadfromFile - can't load fragment shader");
				Cleanup();
				return;
			}
		}

		if (m_usesGeometryShader)
		{
			if (m_pGeometryShader->Load( m_geometryFile) != 0)
			{
				Log::LogMessage( "ShaderProgram :: LoadfromFile - can't load fragment shader");
				Cleanup();
				return;
			}
		}

		if ( ! m_vertexFile.empty())
		{
			if ( ! m_pVertexShader->Compile())
			{
				Log::LogMessage( "ShaderProgram :: LoadfromFile - ***COMPILER ERROR (Vertex Shader : " + m_vertexFile + ")");
				Cleanup();
				m_bError = true;
				return;
			}
			Log::LogMessage( "ShaderProgram :: LoadfromFile - ***Vertex Shader compiled successfully");
		}

		if ( ! m_fragmentFile.empty())
		{
			if ( ! m_pFragmentShader->Compile())
			{
				Log::LogMessage( "ShaderProgram :: LoadfromFile - ***COMPILER ERROR (Fragment Shader : " + m_fragmentFile + ")");
				Cleanup();
				m_bError = true;
				return;

			}
			Log::LogMessage( "ShaderProgram :: LoadFromFile - ***Fragment Shader compiled successfully");
		}

		if (m_usesGeometryShader)
		{
			if ( ! m_pGeometryShader->Compile())
			{
				Log::LogMessage( "ShaderProgram :: LoadfromFile - ***COMPILER ERROR (Geometry Shader : " + m_geometryFile + ")");
				Cleanup();
				m_bError = true;
				return;

			}
			Log::LogMessage( "ShaderProgram :: LoadfromFile - ***Geometry Shader compiled successfully");
		}

		if ( ! Link())
		{
			Log::LogMessage( "ShaderProgram :: LoadfromFile - **LINKER ERROR");
			Cleanup();
			m_bError = true;
			return;
		}

		Log::LogMessage( "ShaderProgram :: LoadfromFile - ***Program Linked successfully");
	}
}

void ShaderProgram :: Initialise()
{
	if (m_bFromMemory)
	{
		_initialiseFromMemory();
	}
	else
	{
		_initialiseFromFile();
	}
}

void ShaderProgram :: SetVertexFile( const String & p_strFile)
{
	m_vertexFile = p_strFile;

	if (File::FileExists( p_strFile))
	{
		File l_file( p_strFile, File::eRead);
		l_file.CopyToString( m_vertexProgram);
	}

	m_isLinked = false;
	m_bError = false;
}

void ShaderProgram :: SetFragmentFile( const String & p_strFile)
{
	m_fragmentFile = p_strFile;

	if (File::FileExists( p_strFile))
	{
		File l_file( p_strFile, File::eRead);
		l_file.CopyToString( m_fragmentProgram);
	}

	m_isLinked = false;
	m_bError = false;
}

void ShaderProgram :: SetGeometryFile( const String & p_strFile)
{
	if (m_usesGeometryShader)
	{
		m_geometryFile = p_strFile;

		if (File::FileExists( p_strFile))
		{
			File l_file( p_strFile, File::eRead);
			l_file.CopyToString( m_geometryProgram);
		}

		m_isLinked = false;
		m_bError = false;
	}
}

void ShaderProgram :: SetVertexText( const String & p_strContent)
{
	m_vertexFile.clear();
	m_bFromMemory = true;

	m_vertexProgram = p_strContent;

	m_isLinked = false;
	m_bError = false;
}

void ShaderProgram :: SetFragmentText( const String & p_strContent)
{
	m_fragmentFile.clear();
	m_bFromMemory = true;

	m_fragmentProgram = p_strContent;

	m_isLinked = false;
	m_bError = false;
}

void ShaderProgram :: SetGeometryText( const String & p_strContent)
{
	if (m_usesGeometryShader)
	{
		m_geometryFile.clear();
		m_bFromMemory = true;

		m_geometryProgram = p_strContent;

		m_isLinked = false;
		m_bError = false;
	}
}