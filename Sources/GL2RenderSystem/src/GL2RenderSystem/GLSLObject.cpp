#include "PrecompiledHeader.h"

#include "GLSLObject.h"
#include "GLRenderSystem.h"
#include "GLSLProgram.h"

using namespace Castor3D;

GLShaderObject :: GLShaderObject()
	:	ShaderObject(),
		m_shaderObject( 0),
		m_pShaderProgram( NULL)
{
}

GLShaderObject :: ~GLShaderObject()
{
	Detach();

	if (m_isCompiled)
	{
		glDeleteShader( m_shaderObject);
		CheckGLError( "GLShaderObject :: ~GLShaderObject - glDeleteShader");
	}
}

int GLShaderObject :: Load( const String & p_filename)
{
	m_bError = false;

	File l_file( p_filename.c_str(), File::eRead);

	if ( ! l_file.IsOk())
	{
		return -1;
	}

	size_t l_len = size_t( l_file.GetLength());

	if (l_len == 0)
	{
		return -2;
	}

	l_file.CopyToString( m_shaderSource);

	return 0;
}

int GLShaderObject :: LoadFromMemory( const String & p_program)
{
	m_bError = false;

	m_shaderSource = p_program;

	return 0;
}

void GLShaderObject :: RetrieveCompilerLog( String & p_strCompilerLog)
{
	int infologLength = 0;
	int charsWritten  = 0;
	glGetShaderiv( m_shaderObject, GL_INFO_LOG_LENGTH, & infologLength);
	CheckGLError( "GLShaderObject :: RetrieveCompilerLog - glGetShaderiv");

	if (infologLength > 0)
	{
		char * infoLog = new char[infologLength];
		glGetShaderInfoLog( m_shaderObject, infologLength, & charsWritten, infoLog);
		CheckGLError( "GLShaderObject :: RetrieveCompilerLog - glGetShaderInfoLog");
		p_strCompilerLog.assign( infoLog);
		delete [] infoLog;
	}

	if (p_strCompilerLog.size() > 0)
	{
		p_strCompilerLog = p_strCompilerLog.substr( 0, p_strCompilerLog.size() - 1);
	}
}

bool GLShaderObject :: Compile()
{
	bool l_bReturn = false;

	if (GLRenderSystem::UseShaders() && ! m_bError && ! m_shaderSource.empty())
	{
		m_isCompiled = false;

		GLint l_iCompiled = 0;
		GLint l_iLength = m_shaderSource.size();
		const char * l_pTmp = m_shaderSource.char_str();

		glShaderSourceARB( m_shaderObject, 1, & l_pTmp, & l_iLength);
		GLRenderSystem::GL_CheckError( "GLShaderObject :: Compile - glShaderSourceARB");

		glCompileShaderARB( m_shaderObject); 
		GLRenderSystem::GL_CheckError( "GLShaderObject :: Compile - glCompileShaderARB");

		glGetObjectParameterivARB( m_shaderObject, GL_COMPILE_STATUS, & l_iCompiled);
		GLRenderSystem::GL_CheckError( "GLShaderObject :: Compile - glGetObjectParameterivARB");

		if (l_iCompiled != 0)
		{
			m_isCompiled = true;
		}
		else
		{
			RetrieveCompilerLog( m_compilerLog);
			Log::LogMessage( m_compilerLog);
			m_bError = true;
		}

		l_bReturn = m_isCompiled;
	}

	return l_bReturn;
}

void GLShaderObject :: Detach()
{
	if (m_isCompiled && m_pShaderProgram != NULL && ! m_bError)
	{
		try
		{
			glDetachShader( m_pShaderProgram->GetProgramObject(), m_shaderObject);
			CheckGLError( "GLShaderObject :: Detach - glDetachShader");
		}
		catch ( ... )
		{
			Log::LogMessage( "GLShaderObject :: Detach - glDetachShader - Exception");
		}

		m_pShaderProgram = NULL;
		// if you get an error here, you deleted the Program object first and then
		// the ShaderObject! Always delete ShaderObjects last!
	}
}

void GLShaderObject :: AttachTo( GLShaderProgram * p_pProgram)
{
	Detach();

	if (m_isCompiled && ! m_bError)
	{
		m_pShaderProgram = p_pProgram;
		glAttachShader( m_pShaderProgram->GetProgramObject(), m_shaderObject);
		CheckGLError( "GLShaderObject :: AttachTo - glAttachShader");
	}
}

GLVertexShader::GLVertexShader()
	:	GLShaderObject()
{
	m_programType = ShaderObject::eVertex; 

	if (GLRenderSystem::UseShaders())
	{
		m_shaderObject = glCreateShaderObjectARB( GL_VERTEX_SHADER_ARB);
		GLRenderSystem::GL_CheckError( "GLShaderObject :: glCreateShaderObjectARB - glCreateShaderObjectARB");
	}
}

GLVertexShader::~GLVertexShader()
{
}

GLFragmentShader::GLFragmentShader()
	:	GLShaderObject()
{
	m_programType = ShaderObject::eFragment;

	if (GLRenderSystem::UseShaders())
	{
		m_shaderObject = glCreateShaderObjectARB( GL_FRAGMENT_SHADER_ARB);
		GLRenderSystem::GL_CheckError( "GLShaderObject :: glCreateShaderObjectARB - glCreateShaderObjectARB");
	}
}

GLFragmentShader::~GLFragmentShader()
{
}


GLGeometryShader::GLGeometryShader()
	:	GLShaderObject()
{
	m_programType = ShaderObject::eGeometry; 

	if (GLRenderSystem::UseShaders() && GLRenderSystem::HasGeometryShader())
	{
		m_shaderObject = glCreateShaderObjectARB( GL_GEOMETRY_SHADER_ARB);
		GLRenderSystem::GL_CheckError( "GLShaderObject :: glCreateShaderObjectARB - glCreateShaderObjectARB");
	}
}

GLGeometryShader::~GLGeometryShader()
{
}