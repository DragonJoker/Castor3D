#include "Dx9RenderSystem/PrecompiledHeader.h"

#include "Dx9RenderSystem/Dx9ShaderObject.h"
#include "Dx9RenderSystem/Dx9ShaderProgram.h"
#include "Dx9RenderSystem/Dx9Context.h"
#include "Dx9RenderSystem/Dx9RenderSystem.h"
#include "Dx9RenderSystem/Dx9FrameVariable.h"

using namespace Castor3D;

//*************************************************************************************************

Dx9ShaderObject :: Dx9ShaderObject( eSHADER_PROGRAM_TYPE p_eType, const String & p_strMainFunction)
	:	HlslShaderObject( p_eType)
	,	m_pShaderProgram( NULL)
	,	m_pErrors( NULL)
	,	m_pInputConstants( NULL)
	,	m_pCompiledShader( NULL)
	,	m_strMainFunction( p_strMainFunction)
{
}

Dx9ShaderObject :: ~Dx9ShaderObject()
{
	DestroyProgram();
}

void Dx9ShaderObject :: DestroyProgram()
{
	Detach();

	if (m_isCompiled)
	{
		m_isCompiled = false;
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

	if (RenderSystem::UseShaders() && ! m_bError && ! m_shaderSource.empty())
	{
		m_isCompiled = false;

		size_t l_uiLength = m_shaderSource.size();
		const char * l_pTmp = m_shaderSource.char_str();

		if (CheckDxError( D3DXCompileShader( l_pTmp, l_uiLength, NULL, NULL, m_strMainFunction.char_str(), m_strProfile.char_str(), 0, & m_pCompiledShader, & m_pErrors, & m_pInputConstants), "Dx9ShaderObject :: Compile - D3DXCompileShader", false))
		{
			m_isCompiled = true;

			for (Dx9FrameVariablePtrStrMap::iterator l_it = m_mapDx9Variables.begin() ; l_it != m_mapDx9Variables.end() ; ++l_it)
			{
				l_it->second->SetConstants( m_pInputConstants);
			}
		}
		else if (m_pErrors != NULL)
		{
			Logger::LogError( String( (xchar *)m_pErrors->GetBufferPointer()), false);
			m_pErrors->Release();
			m_pErrors = NULL;
			m_bError = true;
		}

		l_bReturn = m_isCompiled;
	}

	return l_bReturn;
}

void Dx9ShaderObject :: Detach()
{
	if (m_isCompiled && m_pShaderProgram != NULL && ! m_bError)
	{
		m_pShaderProgram = NULL;
	}
}

void Dx9ShaderObject :: AttachTo( Dx9ShaderProgram * p_pProgram)
{
	Detach();

	if (m_isCompiled && ! m_bError)
	{
		m_pShaderProgram = p_pProgram;
	}
}

Dx9FrameVariablePtr Dx9ShaderObject :: GetD3dFrameVariable( const String & p_strName)
{
	Dx9FrameVariablePtr l_pReturn;
	Dx9FrameVariablePtrStrMap::iterator l_it = m_mapDx9Variables.find( p_strName);

	if (l_it != m_mapDx9Variables.end())
	{
		l_pReturn = l_it->second;
	}

	return l_pReturn;
}

//*************************************************************************************************

Dx9VertexShader :: Dx9VertexShader()
	:	Dx9ShaderObject( eVertexShader, "mainVS")
	,	m_pVertexShader( NULL)
{
}

Dx9VertexShader :: ~Dx9VertexShader()
{
	if( m_pVertexShader != NULL )
	{
		m_pVertexShader->Release(); 
		m_pVertexShader = NULL;
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
		CheckDxError( Dx9RenderSystem::GetDevice()->CreateVertexShader( (DWORD*)m_pCompiledShader->GetBufferPointer(), & m_pVertexShader), "Dx9FragmentShader :: Compile - CreateVertexShader", false);
		m_pCompiledShader->Release();
		m_pCompiledShader = NULL;
	}

	return l_bReturn;
}

void Dx9VertexShader :: Begin()
{
	if (m_isCompiled)
	{
		CheckDxError( Dx9RenderSystem::GetDevice()->SetVertexShader( m_pVertexShader), CU_T( "Dx9VertexShader :: Begin"), false);

		for (Dx9FrameVariablePtrStrMap::iterator l_it = m_mapDx9Variables.begin() ; l_it != m_mapDx9Variables.end() ; ++l_it)
		{
			l_it->second->Apply();
		}
	}
}

void Dx9VertexShader :: End()
{
	CheckDxError( Dx9RenderSystem::GetDevice()->SetVertexShader( NULL), CU_T( "Dx9VertexShader :: End"), false);
}

//*************************************************************************************************

Dx9FragmentShader :: Dx9FragmentShader()
	:	Dx9ShaderObject( ePixelShader, "mainPS")
	,	m_pPixelShader( NULL)
{
	m_programType = ePixelShader;
}

Dx9FragmentShader :: ~Dx9FragmentShader()
{
	if( m_pPixelShader != NULL )
	{
		m_pPixelShader->Release(); 
		m_pPixelShader = NULL;
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
		CheckDxError( Dx9RenderSystem::GetDevice()->CreatePixelShader( (DWORD*)m_pCompiledShader->GetBufferPointer(), & m_pPixelShader), "Dx9FragmentShader :: Compile - CreatePixelShader", false);
		m_pCompiledShader->Release();
		m_pCompiledShader = NULL;
	}

	return l_bReturn;
}

void Dx9FragmentShader :: Begin()
{
	if (m_isCompiled)
	{
		CheckDxError( Dx9RenderSystem::GetDevice()->SetPixelShader( m_pPixelShader), CU_T( "Dx9FragmentShader :: Begin"), false);

		for (Dx9FrameVariablePtrStrMap::iterator l_it = m_mapDx9Variables.begin() ; l_it != m_mapDx9Variables.end() ; ++l_it)
		{
			l_it->second->Apply();
		}
	}
}

void Dx9FragmentShader :: End()
{
	CheckDxError( Dx9RenderSystem::GetDevice()->SetPixelShader( NULL), CU_T( "Dx9FragmentShader :: End"), false);
}

//*************************************************************************************************