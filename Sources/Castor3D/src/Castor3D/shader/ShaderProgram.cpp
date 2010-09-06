#include "PrecompiledHeader.h"

#include "shader/Module_Shader.h"

#include "shader/ShaderProgram.h"
#include "shader/ShaderManager.h"
#include "shader/ShaderObject.h"
#include "shader/UniformVariable.h"
#include "render_system/RenderSystem.h"
#include "Log.h"

using namespace Castor3D;

ShaderProgram :: ShaderProgram( const String & p_vertexShaderFile, const String & p_fragmentShaderFile, const String & p_geometryShaderFile)
	:	m_isLinked( false),
		m_bError( false),
		m_enabled( true),
		m_usesGeometryShader( false),
		m_manageMemory( false),
		m_vertexFile( p_vertexShaderFile),
		m_fragmentFile( p_fragmentShaderFile),
		m_geometryFile( p_geometryShaderFile),
		m_pVertexShader( NULL),
		m_pFragmentShader( NULL),
		m_pGeometryShader( NULL),
		m_bFromMemory( false)
{
}

ShaderProgram :: ShaderProgram()
	:	m_isLinked( false),
		m_bError( false),
		m_enabled( true),
		m_usesGeometryShader( false),
		m_manageMemory( false),
		m_pVertexShader( NULL),
		m_pFragmentShader( NULL),
		m_pGeometryShader( NULL),
		m_bFromMemory( true)
{
}

ShaderProgram :: ~ShaderProgram()
{
}

void ShaderProgram :: Cleanup()
{
	delete m_pVertexShader;
	m_pVertexShader = NULL;

	delete m_pFragmentShader;
	m_pFragmentShader = NULL;

	delete m_pGeometryShader;
	m_pGeometryShader = NULL;
}

void ShaderProgram :: AddUniformVariable( UniformVariable * p_pUniformVariable)
{
	if (p_pUniformVariable != NULL)
	{
		if (m_mapUniformVariables.find( p_pUniformVariable->GetName()) == m_mapUniformVariables.end())
		{
			m_mapUniformVariables.insert( std::map <String, UniformVariable *>::value_type( p_pUniformVariable->GetName(), p_pUniformVariable));
		}
	}
}
/*
void ShaderProgram :: AddShader( ShaderObject * p_shaderObject)
{
	if (p_shaderObject != NULL)
	{
		bool l_bOK = true;

		if ( ! p_shaderObject->m_isCompiled)
		{
			Log::LogMessage( "**warning** please compile program before adding object! trying to compile now...");

			if ( ! p_shaderObject->Compile())
			{
				Log::LogMessage( "...compile ERROR!");
				l_bOK = false;
			}
			else
			{   
				Log::LogMessage( "...ok!");
			}
		}

		if (l_bOK)
		{
			m_shaderList.push_back( p_shaderObject);
		}
	}
}
*/
void ShaderProgram :: _initialiseFromMemory()
{
	if ( ! m_isLinked && ! m_bError)
	{
		Cleanup();

		m_usesGeometryShader = ! m_geometryFile.empty();

		m_pVertexShader = RenderSystem::GetSingletonPtr()->CreateVertexShader();
		m_pFragmentShader = RenderSystem::GetSingletonPtr()->CreateFragmentShader();

		if (m_usesGeometryShader)
		{
			m_pGeometryShader = RenderSystem::GetSingletonPtr()->CreateGeometryShader();
		}

		if ( ! m_vertexFile.empty())
		{
			if (m_pVertexShader->LoadFromMemory( m_vertexFile) != 0)
			{ 
				Log::LogMessage( " GLShaderManager :: LoadfromFile - can't load vertex shader");
				Cleanup();
				return;
			}
		}

		if ( ! m_fragmentFile.empty())
		{
			if (m_pFragmentShader->LoadFromMemory( m_fragmentFile) != 0)
			{
				Log::LogMessage( " GLShaderManager :: LoadfromFile - can't load fragment shader");
				Cleanup();
				return;
			}
		}

		if (m_usesGeometryShader)
		{
			if (m_pGeometryShader->LoadFromMemory( m_geometryFile) != 0)
			{
				Log::LogMessage( " GLShaderManager :: LoadfromFile - can't load fragment shader");
				Cleanup();
				return;
			}
		}

		if ( ! m_vertexFile.empty())
		{
			if ( ! m_pVertexShader->Compile())
			{
				Log::LogMessage( "***COMPILER ERROR (Vertex Shader)");
				Cleanup();
				return;
			}
			Log::LogMessage( "***GLSL Vertex Shader compiled successfully");
		}

		if ( ! m_fragmentFile.empty())
		{
			if ( ! m_pFragmentShader->Compile())
			{
				Log::LogMessage( "***COMPILER ERROR (Fragment Shader)");
				Cleanup();
				return;

			}
			Log::LogMessage( "***GLSL Fragment Shader compiled successfully");
		}

		if (m_usesGeometryShader)
		{
			if ( ! m_pGeometryShader->Compile())
			{
				Log::LogMessage( "***COMPILER ERROR (Geometry Shader)");
				Cleanup();
				return;

			}
			Log::LogMessage( "***GLSL Geometry Shader compiled successfully");
		}

		if ( ! Link())
		{
			Log::LogMessage( "**LINKER ERROR");
			Cleanup();
			return;
		}
		Log::LogMessage( "***GLSL Program Linked successfully");

		ManageMemory();

		ShaderManager::GetSingletonPtr()->AddProgram( this);
	}
}

void ShaderProgram :: _initialiseFromFile()
{
	if ( ! m_isLinked && ! m_bError)
	{
		Cleanup();

		m_usesGeometryShader = ! m_geometryFile.empty();

		m_pVertexShader = RenderSystem::GetSingletonPtr()->CreateVertexShader();
		m_pFragmentShader = RenderSystem::GetSingletonPtr()->CreateFragmentShader();

		if (m_usesGeometryShader)
		{
			m_pGeometryShader = RenderSystem::GetSingletonPtr()->CreateGeometryShader();
		}

		if ( ! m_vertexFile.empty())
		{
			if (m_pVertexShader->Load( m_vertexFile) != 0)
			{ 
				Log::LogMessage( " GLShaderManager :: LoadfromFile - can't load vertex shader");
				Cleanup();
				return;
			}
		}

		if ( ! m_fragmentFile.empty())
		{
			if (m_pFragmentShader->Load( m_fragmentFile) != 0)
			{
				Log::LogMessage( " GLShaderManager :: LoadfromFile - can't load fragment shader");
				Cleanup();
				return;
			}
		}

		if (m_usesGeometryShader)
		{
			if (m_pGeometryShader->Load( m_geometryFile) != 0)
			{
				Log::LogMessage( " GLShaderManager :: LoadfromFile - can't load fragment shader");
				Cleanup();
				return;
			}
		}

		if ( ! m_vertexFile.empty())
		{
			if ( ! m_pVertexShader->Compile())
			{
				Log::LogMessage( "***COMPILER ERROR (Vertex Shader)");
				Cleanup();
				m_bError = true;
				return;
			}
			Log::LogMessage( "***GLSL Vertex Shader compiled successfully");
		}

		if ( ! m_fragmentFile.empty())
		{
			if ( ! m_pFragmentShader->Compile())
			{
				Log::LogMessage( "***COMPILER ERROR (Fragment Shader)");
				Cleanup();
				m_bError = true;
				return;

			}
			Log::LogMessage( "***GLSL Fragment Shader compiled successfully");
		}

		if (m_usesGeometryShader)
		{
			if ( ! m_pGeometryShader->Compile())
			{
				Log::LogMessage( "***COMPILER ERROR (Geometry Shader)");
				Cleanup();
				m_bError = true;
				return;

			}
			Log::LogMessage( "***GLSL Geometry Shader compiled successfully");
		}

		if ( ! Link())
		{
			Log::LogMessage( "**LINKER ERROR");
			Cleanup();
			m_bError = true;
			return;
		}

		Log::LogMessage( "***GLSL Program Linked successfully");

		ManageMemory();

		ShaderManager::GetSingletonPtr()->AddProgram( this);
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

void ShaderProgram :: SetVertexFileIO( const String & p_strFile)
{
	m_vertexFile = p_strFile;

	if (FileBase::FileExists( p_strFile))
	{
		FileIO l_file( p_strFile, FileIO::eRead);
		l_file.CopyToString( m_vertexProgram);
	}

	m_isLinked = false;
	m_bError = false;
}

void ShaderProgram :: SetFragmentFileIO( const String & p_strFile)
{
	m_fragmentFile = p_strFile;

	if (FileBase::FileExists( p_strFile))
	{
		FileIO l_file( p_strFile, FileIO::eRead);
		l_file.CopyToString( m_fragmentProgram);
	}

	m_isLinked = false;
	m_bError = false;
}

void ShaderProgram :: SetGeometryFileIO( const String & p_strFile)
{
	if (m_usesGeometryShader)
	{
		m_geometryFile = p_strFile;

		if (FileBase::FileExists( p_strFile))
		{
			FileIO l_file( p_strFile, FileIO::eRead);
			l_file.CopyToString( m_geometryProgram);
		}

		m_isLinked = false;
		m_bError = false;
	}
}