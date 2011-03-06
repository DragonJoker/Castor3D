#include "OpenGlCommon/PrecompiledHeader.h"

#include "OpenGlCommon/GlShaderObject.h"
#include "OpenGlCommon/GlShaderProgram.h"

using namespace Castor3D;

//*************************************************************************************************

GlShaderObject :: GlShaderObject( eSHADER_PROGRAM_TYPE p_eType)
	:	GlslShaderObject( p_eType),
		m_shaderObject( 0),
		m_pShaderProgram( NULL)
{
}

GlShaderObject :: ~GlShaderObject()
{
	DestroyProgram();
}

void GlShaderObject :: DestroyProgram()
{
	Detach();

	if (m_isCompiled)
	{
		CheckGlError( glDeleteShader( m_shaderObject), CU_T( "GlShaderObject :: ~GlShaderObject - glDeleteShader"));
		m_isCompiled = false;
	}
}

void GlShaderObject :: RetrieveCompilerLog( String & p_strCompilerLog)
{
	int infologLength = 0;
	int charsWritten  = 0;
	CheckGlError( glGetShaderiv( m_shaderObject, GL_INFO_LOG_LENGTH, & infologLength), CU_T( "GlShaderObject :: RetrieveCompilerLog - glGetShaderiv"));

	if (infologLength > 0)
	{
		char * infoLog = new char[infologLength];
		CheckGlError( glGetShaderInfoLog( m_shaderObject, infologLength, & charsWritten, infoLog), CU_T( "GlShaderObject :: RetrieveCompilerLog - glGetShaderInfoLog"));
		p_strCompilerLog = infoLog;
		delete [] infoLog;
	}

	if (p_strCompilerLog.size() > 0)
	{
		p_strCompilerLog = p_strCompilerLog.substr( 0, p_strCompilerLog.size() - 1);
	}
}

bool GlShaderObject :: Compile()
{
	bool l_bReturn = false;

	if (RenderSystem::UseShaders() && ! m_bError && ! m_shaderSource.empty())
	{
		m_isCompiled = false;

		GLint l_iCompiled = 0;
		GLint l_iLength = m_shaderSource.size();
		const char * l_pTmp = m_shaderSource.char_str();

		CheckGlError( glShaderSource( m_shaderObject, 1, & l_pTmp, & l_iLength), CU_T( "GlShaderObject :: Compile - glShaderSource"));
		CheckGlError( glCompileShader( m_shaderObject), CU_T( "GlShaderObject :: Compile - glCompileShader"));
		CheckGlError( glGetShaderiv( m_shaderObject, GL_COMPILE_STATUS, & l_iCompiled), CU_T( "GlShaderObject :: Compile - glGetShaderiv"));

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

void GlShaderObject :: Detach()
{
	if (m_isCompiled && m_pShaderProgram != NULL && ! m_bError)
	{
		try
		{
			CheckGlError( glDetachShader( m_pShaderProgram->GetProgramObject(), m_shaderObject), CU_T( "GlShaderObject :: Detach - glDetachShader"));
		}
		catch ( ... )
		{
			Logger::LogMessage( CU_T( "GlShaderObject :: Detach - glDetachShader - Exception"));
		}

		m_pShaderProgram = NULL;
		// if you get an error here, you deleted the Program object first and then
		// the ShaderObject! Always delete ShaderObjects last!
	}
}

void GlShaderObject :: AttachTo( GlShaderProgram * p_pProgram)
{
	Detach();

	if (m_isCompiled && ! m_bError)
	{
		m_pShaderProgram = p_pProgram;
		CheckGlError( glAttachShader( m_pShaderProgram->GetProgramObject(), m_shaderObject), CU_T( "GlShaderObject :: AttachTo - glAttachShader"));
	}
}

//*************************************************************************************************

GlVertexShader :: GlVertexShader()
	:	GlShaderObject( eVertexShader)
{
}

GlVertexShader :: ~GlVertexShader()
{
}

void GlVertexShader :: CreateProgram()
{
	if (RenderSystem::UseShaders())
	{
		CheckGlError( m_shaderObject = glCreateShader( GL_VERTEX_SHADER), CU_T( "GlVertexShader :: GlVertexShader - glCreateShader"));
	}
}

//*************************************************************************************************

GlFragmentShader :: GlFragmentShader()
	:	GlShaderObject( ePixelShader)
{
}

GlFragmentShader :: ~GlFragmentShader()
{
}

void GlFragmentShader :: CreateProgram()
{
	if (RenderSystem::UseShaders())
	{
		CheckGlError( m_shaderObject = glCreateShader( GL_FRAGMENT_SHADER), CU_T( "GlFragmentShader :: GlFragmentShader - glCreateShader"));
	}
}

//*************************************************************************************************

GlGeometryShader :: GlGeometryShader()
	:	GlShaderObject( eGeometryShader)
{
}

GlGeometryShader :: ~GlGeometryShader()
{
}

void GlGeometryShader :: CreateProgram()
{
	if (RenderSystem::UseShaders() && RenderSystem::HasGeometryShader())
	{
		CheckGlError( m_shaderObject = glCreateShader( GL_GEOMETRY_SHADER), CU_T( "GlGeometryShader :: GlGeometryShader - glCreateShader"));
	}
}

//*************************************************************************************************