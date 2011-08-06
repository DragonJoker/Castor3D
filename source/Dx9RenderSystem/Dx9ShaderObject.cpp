#include "Dx9RenderSystem/PrecompiledHeader.hpp"

#include "Dx9RenderSystem/Dx9ShaderObject.hpp"
#include "Dx9RenderSystem/Dx9ShaderProgram.hpp"
#include "Dx9RenderSystem/Dx9Context.hpp"
#include "Dx9RenderSystem/Dx9RenderSystem.hpp"
#include "Dx9RenderSystem/Dx9FrameVariable.hpp"

using namespace Castor3D;

//*************************************************************************************************

struct VariableApply
{
	template <class T> void operator()(T & p) const
	{
		p->Apply();
	}
};

//*************************************************************************************************

Dx9ShaderObject :: Dx9ShaderObject( Dx9ShaderProgram * p_pParent, eSHADER_TYPE p_eType)
	:	HlslShaderObject( p_pParent, p_eType)
	,	m_pShaderProgram( nullptr)
	,	m_pErrors( nullptr)
	,	m_pInputConstants( nullptr)
	,	m_pCompiledShader( nullptr)
{
}

Dx9ShaderObject :: ~Dx9ShaderObject()
{
	DestroyProgram();
}

void Dx9ShaderObject :: DestroyProgram()
{
	Detach();

	if (m_bCompiled)
	{
		m_bCompiled = false;
	}
}

void Dx9ShaderObject :: RetrieveCompilerLog( String & p_strCompilerLog)
{
	int infologLength = 0;
	int charsWritten  = 0;

	if (infologLength > 0)
	{
		char * infoLog = new char[infologLength];
		p_strCompilerLog = infoLog;
		delete [] infoLog;
	}

	if (p_strCompilerLog.size() > 0)
	{
		p_strCompilerLog = p_strCompilerLog.substr( 0, p_strCompilerLog.size() - 1);
	}
}

bool Dx9ShaderObject :: Compile()
{
	bool l_bReturn = false;

	if (RenderSystem::UseShaders() && ! m_bError && ! m_strSource.empty())
	{
		m_bCompiled = false;

		size_t l_uiLength = m_strSource.size();
		char * l_pTmp = new char[l_uiLength + 1];
		memcpy( l_pTmp, m_strSource.char_str(), l_uiLength);
		l_pTmp[l_uiLength] = 0;

		try
		{
			if (CheckDxError( D3DXCompileShader( l_pTmp, l_uiLength, nullptr, nullptr, m_strEntryPoint.char_str(), m_strProfile.char_str(), 0, & m_pCompiledShader, & m_pErrors, & m_pInputConstants), "Dx9ShaderObject :: Compile - D3DXCompileShader", false))
			{
				m_bCompiled = true;
			}
		}
		catch ( ... )
		{
			m_bCompiled = false;
		}

		delete [] l_pTmp;

		if (m_pErrors)
		{
			Logger::LogMessage( String( (xchar *)m_pErrors->GetBufferPointer()));
			m_pErrors->Release();
			m_pErrors = nullptr;
			m_bError = true;
		}

		l_bReturn = m_bCompiled;
	}

	return l_bReturn;
}

void Dx9ShaderObject :: Detach()
{
	if (m_bCompiled && m_pShaderProgram && ! m_bError)
	{
		m_pShaderProgram = nullptr;
	}
}

void Dx9ShaderObject :: AttachTo( Dx9ShaderProgram * p_pProgram)
{
	Detach();

	if (m_bCompiled && ! m_bError)
	{
		m_pShaderProgram = p_pProgram;
	}
}

//*************************************************************************************************

Dx9VertexShader :: Dx9VertexShader( Dx9ShaderProgram * p_pParent)
	:	Dx9ShaderObject( p_pParent, eSHADER_TYPE_VERTEX)
	,	m_pVertexShader( nullptr)
{
}

Dx9VertexShader :: ~Dx9VertexShader()
{
	if( m_pVertexShader )
	{
		m_pVertexShader->Release();
		m_pVertexShader = nullptr;
	}
}

void Dx9VertexShader :: CreateProgram()
{
}

bool Dx9VertexShader :: Compile()
{
	m_strProfile = D3DXGetVertexShaderProfile( Dx9RenderSystem::GetDevice());
	bool l_bReturn = Dx9ShaderObject::Compile();

	if (l_bReturn)
	{
		Logger::LogMessage( "Vertex shader compiled successfully");
		CheckDxError( Dx9RenderSystem::GetDevice()->CreateVertexShader( (DWORD*)m_pCompiledShader->GetBufferPointer(), & m_pVertexShader), "Dx9FragmentShader :: Compile - CreateVertexShader", false);
		m_pCompiledShader->Release();
		m_pCompiledShader = nullptr;
	}

	return l_bReturn;
}

void Dx9VertexShader :: Bind()
{
	if (m_bCompiled)
	{
		CheckDxError( Dx9RenderSystem::GetDevice()->SetVertexShader( m_pVertexShader), cuT( "Dx9VertexShader :: Begin"), false);
	}
}

void Dx9VertexShader :: Unbind()
{
	CheckDxError( Dx9RenderSystem::GetDevice()->SetVertexShader( nullptr), cuT( "Dx9VertexShader :: End"), false);
}

//*************************************************************************************************

Dx9FragmentShader :: Dx9FragmentShader( Dx9ShaderProgram * p_pParent)
	:	Dx9ShaderObject( p_pParent, eSHADER_TYPE_PIXEL)
	,	m_pPixelShader( nullptr)
{
}

Dx9FragmentShader :: ~Dx9FragmentShader()
{
	if( m_pPixelShader )
	{
		m_pPixelShader->Release();
		m_pPixelShader = nullptr;
	}
}

void Dx9FragmentShader :: CreateProgram()
{
}

bool Dx9FragmentShader :: Compile()
{
	m_strProfile = D3DXGetPixelShaderProfile( Dx9RenderSystem::GetDevice());
	bool l_bReturn = Dx9ShaderObject::Compile();

	if (l_bReturn)
	{
		Logger::LogMessage( "Pixel shader compiled successfully");
		CheckDxError( Dx9RenderSystem::GetDevice()->CreatePixelShader( (DWORD*)m_pCompiledShader->GetBufferPointer(), & m_pPixelShader), "Dx9FragmentShader :: Compile - CreatePixelShader", false);
		m_pCompiledShader->Release();
		m_pCompiledShader = nullptr;
	}

	return l_bReturn;
}

void Dx9FragmentShader :: Bind()
{
	if (m_bCompiled)
	{
		CheckDxError( Dx9RenderSystem::GetDevice()->SetPixelShader( m_pPixelShader), cuT( "Dx9FragmentShader :: Begin"), false);
	}
}

void Dx9FragmentShader :: Unbind()
{
	CheckDxError( Dx9RenderSystem::GetDevice()->SetPixelShader( nullptr), cuT( "Dx9FragmentShader :: End"), false);
}

//*************************************************************************************************
