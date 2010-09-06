#include "PrecompiledHeader.h"

#include "GLSLObject.h"
#include "GLRenderSystem.h"
#include "GLSLProgram.h"

using namespace Castor3D;

GLShaderObject :: GLShaderObject()
	:	ShaderObject(),
		m_compilerLog( 0),
		m_shaderObject( 0),
		m_pShaderProgram( NULL)
{
}

GLShaderObject :: ~GLShaderObject()
{
	if (m_compilerLog != NULL)
	{
		free( m_compilerLog);
	}

	if (m_isCompiled)
	{ 
//		glDeleteObjectARB( m_shaderObject);
		GLRenderSystem::GL_CheckError( "GLShaderObject :: Delete");
	}
}

unsigned long getFileLength( std::ifstream & p_file)
{
	if( ! p_file.good())
	{
		return 0;
	}

	unsigned long pos = p_file.tellg();
	p_file.seekg( 0, std::ios::end);
	unsigned long len = p_file.tellg();
	p_file.seekg( std::ios::beg);

	return len;
}


int GLShaderObject :: Load( const String & p_filename)
{
	m_bError = false;

	std::ifstream l_file;
	l_file.open( p_filename.c_str(), std::ios::in);
	if ( ! l_file)
	{
		return -1;
	}

	unsigned long l_len = getFileLength( l_file);

	if (l_len == 0)
	{
		return -2;
	}

	if (m_shaderSource != 0 && m_memAlloc)
	{
		delete [] m_shaderSource;
	}

	m_shaderSource = (GLubyte *)(new char[l_len + 1]);
	if (m_shaderSource == 0)
	{
		return -3;
	}
	m_memAlloc = true;

	m_shaderSource[l_len] = 0;
	
	unsigned int i = 0;
	while (l_file.good())
	{
		m_shaderSource[i] = l_file.get();
		if ( ! l_file.eof())
		{
			i++;
		}
	}

	m_shaderSource[i] = 0;

	l_file.close();

	return 0;
}

int GLShaderObject :: LoadFromMemory( const String & p_program)
{
	m_bError = false;

	if (m_shaderSource != 0 && m_memAlloc)
	{
		delete [] m_shaderSource;
	}

	m_memAlloc = false;
	m_shaderSource = (GLubyte*) p_program.c_str();

	return 0;
}

void GLShaderObject :: RetrieveCompilerLog( String & p_strCompilerLog)
{
	int infologLength = 0;
	int charsWritten  = 0;
	glGetShaderiv( m_shaderObject, GL_INFO_LOG_LENGTH, & infologLength);

	if (infologLength > 0)
	{
		char * infoLog;
		infoLog = new char[infologLength];
		glGetShaderInfoLog( m_shaderObject, infologLength, & charsWritten, infoLog);
		p_strCompilerLog.assign( infoLog);
		delete infoLog;
	}
}

bool GLShaderObject :: Compile()
{
	if ( ! GLRenderSystem::UseGLSL() || m_bError || m_shaderSource == NULL)
	{
		return false;
	}

	m_isCompiled = false;

	GLint compiled = 0;
	GLint length = (GLint) strlen( (const char*)m_shaderSource);

	glShaderSourceARB( m_shaderObject, 1, (const GLcharARB **)&m_shaderSource, &length);
	GLRenderSystem::GL_CheckError( "GLShaderObject :: Compile - glShaderSourceARB");

	glCompileShaderARB( m_shaderObject); 
	GLRenderSystem::GL_CheckError( "GLShaderObject :: Compile - glCompileShaderARB");

	glGetObjectParameterivARB( m_shaderObject, GL_COMPILE_STATUS, &compiled);
	GLRenderSystem::GL_CheckError( "GLShaderObject :: Compile - glGetObjectParameterivARB");

	String l_strCompilerLog;
	RetrieveCompilerLog( l_strCompilerLog);
	Log::LogMessage( l_strCompilerLog);

	if (compiled)
	{
		m_isCompiled = true;
	}
	else
	{
		m_bError = true;
	}

	return m_isCompiled;
}

GLint GLShaderObject :: GetAttribLocation( char * p_attribName)
{
	return glGetAttribLocationARB(m_shaderObject, p_attribName);
}

void GLShaderObject :: Detach()
{
	if (m_isCompiled && m_pShaderProgram != NULL && ! m_bError)
	{
		try
		{
//			glDetachShader( m_pShaderProgram->GetProgramObject(), m_shaderObject);
//			CheckGLError( "GLShaderObject :: Detach - glDetachShader");
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

	if (GLRenderSystem::UseGLSL())
	{
		m_shaderObject = glCreateShaderObjectARB( GL_VERTEX_SHADER_ARB);
		GLRenderSystem::GL_CheckError( "GLShaderObject :: Compile - glCreateShaderObjectARB");
	}
}

GLVertexShader::~GLVertexShader()
{
}

GLFragmentShader::GLFragmentShader()
	:	GLShaderObject()
{
	m_programType = ShaderObject::eFragment;

	if (GLRenderSystem::UseGLSL())
	{
		m_shaderObject = glCreateShaderObjectARB( GL_FRAGMENT_SHADER_ARB);
		GLRenderSystem::GL_CheckError( "GLShaderObject :: Compile - glCreateShaderObjectARB");
	}
}

GLFragmentShader::~GLFragmentShader()
{
}


GLGeometryShader::GLGeometryShader()
	:	GLShaderObject()
{
	m_programType = ShaderObject::eGeometry; 

	if (GLRenderSystem::UseGLSL() && GLRenderSystem::HasGeometryShader())
	{
		m_shaderObject = glCreateShaderObjectARB( GL_GEOMETRY_SHADER_ARB);
		GLRenderSystem::GL_CheckError( "GLShaderObject :: Compile - glCreateShaderObjectARB");
	}
}

GLGeometryShader::~GLGeometryShader()
{
}