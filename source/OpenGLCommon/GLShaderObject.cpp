#include "OpenGLCommon/PrecompiledHeader.h"

#include "OpenGLCommon/GLShaderObject.h"
#include "OpenGLCommon/GLShaderProgram.h"

using namespace Castor3D;

//*************************************************************************************************

GLShaderObject :: GLShaderObject()
	:	ShaderObject(),
		m_shaderObject( 0),
		m_pShaderProgram( NULL)
{
}

GLShaderObject :: ~GLShaderObject()
{
	DestroyProgram();
}

void GLShaderObject :: DestroyProgram()
{
	Detach();

	if (m_isCompiled)
	{
		glDeleteShader( m_shaderObject);
		CheckGLError( CU_T( "GLShaderObject :: ~GLShaderObject - glDeleteShader"));
		m_isCompiled = false;
	}
}

void GLShaderObject :: RetrieveCompilerLog( String & p_strCompilerLog)
{
	int infologLength = 0;
	int charsWritten  = 0;
	glGetShaderiv( m_shaderObject, GL_INFO_LOG_LENGTH, & infologLength);
	CheckGLError( CU_T( "GLShaderObject :: RetrieveCompilerLog - glGetShaderiv"));

	if (infologLength > 0)
	{
		char * infoLog = new char[infologLength];
		glGetShaderInfoLog( m_shaderObject, infologLength, & charsWritten, infoLog);
		CheckGLError( CU_T( "GLShaderObject :: RetrieveCompilerLog - glGetShaderInfoLog"));
		p_strCompilerLog = infoLog;
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

	if (RenderSystem::UseShaders() && ! m_bError && ! m_shaderSource.empty())
	{
		m_isCompiled = false;

		GLint l_iCompiled = 0;
		GLint l_iLength = m_shaderSource.size();
		const char * l_pTmp = m_shaderSource.char_str();

		glShaderSource( m_shaderObject, 1, & l_pTmp, & l_iLength);
		CheckGLError( CU_T( "GLShaderObject :: Compile - glShaderSource"));

		glCompileShader( m_shaderObject); 
		CheckGLError( CU_T( "GLShaderObject :: Compile - glCompileShader"));

		glGetShaderiv( m_shaderObject, GL_COMPILE_STATUS, & l_iCompiled);
		CheckGLError( CU_T( "GLShaderObject :: Compile - glGetShaderiv"));

		if (l_iCompiled != 0)
		{
			m_isCompiled = true;
		}
		else
		{
			m_bError = true;
		}

		RetrieveCompilerLog( m_compilerLog);

		if (m_compilerLog.size() > 0)
		{
			Logger::LogMessage( m_compilerLog);
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
			CheckGLError( CU_T( "GLShaderObject :: Detach - glDetachShader"));
		}
		catch ( ... )
		{
			Logger::LogMessage( CU_T( "GLShaderObject :: Detach - glDetachShader - Exception"));
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
		CheckGLError( CU_T( "GLShaderObject :: AttachTo - glAttachShader"));
	}
}

//*************************************************************************************************

GLVertexShader :: GLVertexShader()
	:	GLShaderObject()
{
	m_programType = eVertexShader; 
}

GLVertexShader :: ~GLVertexShader()
{
}

void GLVertexShader :: CreateProgram()
{
	if (RenderSystem::UseShaders())
	{
		m_shaderObject = glCreateShader( GL_VERTEX_SHADER);
		CheckGLError( CU_T( "GLVertexShader :: GLVertexShader - glCreateShader"));
	}
}

//*************************************************************************************************

GLFragmentShader :: GLFragmentShader()
	:	GLShaderObject()
{
	m_programType = ePixelShader;
}

GLFragmentShader :: ~GLFragmentShader()
{
}

void GLFragmentShader :: CreateProgram()
{
	if (RenderSystem::UseShaders())
	{
		m_shaderObject = glCreateShader( GL_FRAGMENT_SHADER);
		CheckGLError( CU_T( "GLFragmentShader :: GLFragmentShader - glCreateShader"));
	}
}

//*************************************************************************************************

GLGeometryShader :: GLGeometryShader()
	:	GLShaderObject()
{
	m_programType = eGeometryShader; 
}

GLGeometryShader :: ~GLGeometryShader()
{
}

void GLGeometryShader :: CreateProgram()
{
	if (RenderSystem::UseShaders() && RenderSystem::HasGeometryShader())
	{
		m_shaderObject = glCreateShader( GL_GEOMETRY_SHADER);
		CheckGLError( CU_T( "GLGeometryShader :: GLGeometryShader - glCreateShader"));
	}
}

//*************************************************************************************************