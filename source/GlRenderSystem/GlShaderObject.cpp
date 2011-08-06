#include "GlRenderSystem/PrecompiledHeader.hpp"

#include "GlRenderSystem/GlShaderObject.hpp"
#include "GlRenderSystem/GlShaderProgram.hpp"
#include "GlRenderSystem/OpenGl.hpp"

using namespace Castor3D;

//*************************************************************************************************

GlShaderObject :: GlShaderObject( GlShaderProgram * p_pParent, eSHADER_TYPE p_eType)
	:	GlslShaderObject( p_pParent, p_eType)
	,	m_shaderObject( 0)
	,	m_pShaderProgram( nullptr)
{
}

GlShaderObject :: ~GlShaderObject()
{
	DestroyProgram();
}

void GlShaderObject :: DestroyProgram()
{
	Detach();

	if (m_bCompiled)
	{
		OpenGl::DeleteShader( m_shaderObject);
		m_bCompiled = false;
	}
}

void GlShaderObject :: RetrieveCompilerLog( String & p_strCompilerLog)
{
	int infologLength = 0;
	int charsWritten  = 0;
	bool l_bReturn = true;
	l_bReturn &= OpenGl::GetShaderiv( m_shaderObject, GL_INFO_LOG_LENGTH, & infologLength);

	if (infologLength > 0)
	{
		char * infoLog = new char[infologLength];
		l_bReturn &= OpenGl::GetShaderInfoLog( m_shaderObject, infologLength, & charsWritten, infoLog);
		p_strCompilerLog = infoLog;
		delete [] infoLog;
	}

	if (p_strCompilerLog.size() > 0)
	{
		p_strCompilerLog = p_strCompilerLog.substr( 0, p_strCompilerLog.size() - 1);
	}
}

void GlShaderObject :: CreateProgram()
{
	if (RenderSystem::UseShaders() && RenderSystem::HasShaderType( m_eType))
	{
		m_shaderObject = OpenGl::CreateShader( GlEnum::Get( m_eType));
	}
}

bool GlShaderObject :: Compile()
{
	bool l_bReturn = false;

	if (RenderSystem::UseShaders() && ! m_bError && ! m_strSource.empty())
	{
		l_bReturn = true;
		m_bCompiled = false;

		GLint l_iCompiled = 0;
		GLint l_iLength = m_strSource.size();
		char const * l_pTmp = m_strSource.char_str();

		l_bReturn &= OpenGl::ShaderSource( m_shaderObject, 1, & l_pTmp, & l_iLength);
		l_bReturn &= OpenGl::CompileShader( m_shaderObject);
		l_bReturn &= OpenGl::GetShaderiv( m_shaderObject, GL_COMPILE_STATUS, & l_iCompiled);

		if (l_iCompiled != 0)
		{
			m_bCompiled = true;
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

		l_bReturn = m_bCompiled;
	}

	return l_bReturn;
}

void GlShaderObject :: Detach()
{
	if (m_bCompiled && m_pShaderProgram && ! m_bError)
	{
		OpenGl::DetachShader( m_pShaderProgram->GetProgramObject(), m_shaderObject);
		m_pShaderProgram = nullptr;
		// if you get an error here, you deleted the Program object first and then
		// the ShaderObject! Always delete ShaderObjects last!
	}
}

void GlShaderObject :: AttachTo( GlShaderProgram * p_pProgram)
{
	Detach();

	if (m_bCompiled && ! m_bError)
	{
		m_pShaderProgram = p_pProgram;
		OpenGl::AttachShader( m_pShaderProgram->GetProgramObject(), m_shaderObject);
	}
}

//*************************************************************************************************
