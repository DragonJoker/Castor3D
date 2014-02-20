#include "Dx11RenderSystem/PrecompiledHeader.hpp"

#include "Dx11RenderSystem/DxShaderObject.hpp"
#include "Dx11RenderSystem/DxShaderProgram.hpp"
#include "Dx11RenderSystem/DxContext.hpp"
#include "Dx11RenderSystem/DxRenderSystem.hpp"
#include "Dx11RenderSystem/DxFrameVariable.hpp"
#include <D3DCompiler.h>

using namespace Castor3D;
using namespace Castor;
using namespace Dx11Render;

//*************************************************************************************************

struct VariableApply
{
	template <class T> void operator()(T & p) const
	{
		p->Apply();
	}
};

//*************************************************************************************************

DxShaderObject :: DxShaderObject( DxShaderProgram * p_pParent, eSHADER_TYPE p_eType)
	:	ShaderObjectBase	( p_pParent, p_eType												)
	,	m_pShaderProgram	( p_pParent															)
	,	m_pCompiled			( NULL																)
	,	m_pRenderSystem		( static_cast< DxRenderSystem* >( p_pParent->GetRenderSystem() )	)
{
	m_eShaderModel = eSHADER_MODEL_3;
}

DxShaderObject :: ~DxShaderObject()
{
	DestroyProgram();
}

void DxShaderObject :: DestroyProgram()
{
	if( m_eStatus == eSHADER_STATUS_COMPILED )
	{
		m_eStatus = eSHADER_STATUS_NOTCOMPILED;
	}

	SafeRelease( m_pCompiled );
}

void DxShaderObject :: RetrieveCompilerLog( String & p_strCompilerLog)
{
	int infologLength = 0;

	if (infologLength > 0)
	{
		char * infoLog = new char[infologLength];
		p_strCompilerLog = str_utils::from_str( infoLog );
		delete_array infoLog;
	}

	if (p_strCompilerLog.size() > 0)
	{
		p_strCompilerLog = p_strCompilerLog.substr( 0, p_strCompilerLog.size() - 1);
	}
}

bool DxShaderObject :: Compile()
{
	static std::string l_strProfiles[eSHADER_TYPE_COUNT][eSHADER_MODEL_COUNT] =
	{	{ "vs_4_0",	"vs_4_0",	"vs_4_0",	"vs_4_0",	"vs_5_0" }
	,	{ "",		"",			"gs_4_0",	"gs_4_0",	"gs_5_0" }
	,	{ "",		"",			"",			"hs_4_0",	"hs_5_0" }
	,	{ "",		"",			"",			"ds_4_0",	"ds_5_0" }
	,	{ "",		"ps_4_0",	"ps_4_0",	"ps_4_0",	"ps_5_0" }
	,	{ "",		"",			"",			"",			"cs_5_0" }
	};

	bool l_bReturn = false;
	std::string l_strSource;
	std::string l_strProfile;

	for( int i = eSHADER_MODEL_5; i >= eSHADER_MODEL_1 && m_strLoadedSource.empty(); i-- )
	{
		if( m_pRenderSystem->CheckSupport( eSHADER_MODEL( i ) ) )
		{
			m_strLoadedSource = m_arraySources[i];
			l_strProfile = l_strProfiles[m_eType][i];
		}
	}

	if( m_pRenderSystem->UseShaders() && m_eStatus != eSHADER_STATUS_ERROR && !m_strLoadedSource.empty() )
	{
		l_strSource = str_utils::to_str( m_strLoadedSource );
		m_eStatus = eSHADER_STATUS_NOTCOMPILED;
		ID3DBlob *	l_pErrors = NULL;
		UINT		l_uiFlags = 0;
#if !defined( NDEBUG )
		l_uiFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_WARNINGS_ARE_ERRORS;
#endif
		HRESULT l_hr = D3DX11CompileFromMemory( l_strSource.c_str(), UINT( l_strSource.size() ), NULL, NULL, NULL, str_utils::to_str( GetEntryPoint() ).c_str(), l_strProfile.c_str(), l_uiFlags, 0, NULL, &m_pCompiled, &l_pErrors, NULL );

		if( l_hr == S_OK )
		{
			Logger::LogMessage( cuT( "Shader compiled successfully" ) );
			DoRetrieveShader();
		}
		else if( l_pErrors )
		{
			Logger::LogMessage( str_utils::from_str( reinterpret_cast< char * >( l_pErrors->GetBufferPointer() ) ) );
			Logger::LogMessage( m_strLoadedSource );
			m_eStatus = eSHADER_STATUS_ERROR;
			m_strLoadedSource.clear();
		}

		SafeRelease( l_pErrors );
		l_bReturn = m_eStatus == eSHADER_STATUS_COMPILED;
	}

	return l_bReturn;
}

//*************************************************************************************************

DxVertexShader :: DxVertexShader( DxShaderProgram * p_pParent)
	:	DxShaderObject	( p_pParent, eSHADER_TYPE_VERTEX	)
	,	m_pVertexShader		( NULL								)
{
}

DxVertexShader :: ~DxVertexShader()
{
	SafeRelease( m_pVertexShader );
}

void DxVertexShader :: Bind()
{
	ID3D11DeviceContext * l_pDeviceContext;
	m_pRenderSystem->GetDevice()->GetImmediateContext( &l_pDeviceContext );
	l_pDeviceContext->VSSetShader( m_pVertexShader, NULL, 0 );
	l_pDeviceContext->Release();
}

void DxVertexShader :: Unbind()
{
	ID3D11DeviceContext * l_pDeviceContext;
	m_pRenderSystem->GetDevice()->GetImmediateContext( &l_pDeviceContext );
	l_pDeviceContext->VSSetShader( NULL, NULL, 0 );
	l_pDeviceContext->Release();
}

void DxVertexShader :: DoRetrieveShader()
{
	if( m_pCompiled )
	{
		ID3D11Device * l_pDevice = m_pRenderSystem->GetDevice();

		if( l_pDevice )
		{
			HRESULT l_hr = l_pDevice->CreateVertexShader( reinterpret_cast< DWORD* >( m_pCompiled->GetBufferPointer() ), m_pCompiled->GetBufferSize(), NULL, &m_pVertexShader );
			dxDebugName( m_pVertexShader, VSShader );
			
			if( l_hr == S_OK )
			{
				m_eStatus = eSHADER_STATUS_COMPILED;
			}
		}
	}
}

//*************************************************************************************************

DxFragmentShader :: DxFragmentShader( DxShaderProgram * p_pParent)
	:	DxShaderObject	( p_pParent, eSHADER_TYPE_PIXEL	)
	,	m_pPixelShader		( NULL							)
{
}

DxFragmentShader :: ~DxFragmentShader()
{
	SafeRelease( m_pPixelShader );
}

void DxFragmentShader :: Bind()
{
	ID3D11DeviceContext * l_pDeviceContext;
	m_pRenderSystem->GetDevice()->GetImmediateContext( &l_pDeviceContext );
	l_pDeviceContext->PSSetShader( m_pPixelShader, NULL, 0 );
	l_pDeviceContext->Release();
}

void DxFragmentShader :: Unbind()
{
	ID3D11DeviceContext * l_pDeviceContext;
	m_pRenderSystem->GetDevice()->GetImmediateContext( &l_pDeviceContext );
	l_pDeviceContext->PSSetShader( NULL, NULL, 0 );
	l_pDeviceContext->Release();
}

void DxFragmentShader :: DoRetrieveShader()
{
	if( m_pCompiled )
	{
		ID3D11Device * l_pDevice = m_pRenderSystem->GetDevice();

		if( l_pDevice )
		{
			HRESULT l_hr = l_pDevice->CreatePixelShader( reinterpret_cast< DWORD* >( m_pCompiled->GetBufferPointer() ), m_pCompiled->GetBufferSize(), NULL, &m_pPixelShader );
			dxDebugName( m_pPixelShader, PSShader );
			
			if( l_hr == S_OK )
			{
				m_eStatus = eSHADER_STATUS_COMPILED;
			}
		}
	}
}

//*************************************************************************************************

DxGeometryShader :: DxGeometryShader( DxShaderProgram * p_pParent)
	:	DxShaderObject	( p_pParent, eSHADER_TYPE_GEOMETRY	)
	,	m_pGeometryShader	( NULL								)
{
}

DxGeometryShader :: ~DxGeometryShader()
{
	SafeRelease( m_pGeometryShader );
}

void DxGeometryShader :: Bind()
{
	ID3D11DeviceContext * l_pDeviceContext;
	m_pRenderSystem->GetDevice()->GetImmediateContext( &l_pDeviceContext );
	l_pDeviceContext->GSSetShader( m_pGeometryShader, NULL, 0 );
	l_pDeviceContext->Release();
}

void DxGeometryShader :: Unbind()
{
	ID3D11DeviceContext * l_pDeviceContext;
	m_pRenderSystem->GetDevice()->GetImmediateContext( &l_pDeviceContext );
	l_pDeviceContext->GSSetShader( NULL, NULL, 0 );
	l_pDeviceContext->Release();
}

void DxGeometryShader :: DoRetrieveShader()
{
	if( m_pCompiled )
	{
		ID3D11Device * l_pDevice = m_pRenderSystem->GetDevice();

		if( l_pDevice )
		{
			HRESULT l_hr = l_pDevice->CreateGeometryShader( reinterpret_cast< DWORD* >( m_pCompiled->GetBufferPointer() ), m_pCompiled->GetBufferSize(), NULL, &m_pGeometryShader );
			dxDebugName( m_pGeometryShader, GSShader );

			if( l_hr == S_OK )
			{
				m_eStatus = eSHADER_STATUS_COMPILED;
			}
		}
	}
}

//*************************************************************************************************

DxHullShader :: DxHullShader( DxShaderProgram * p_pParent)
	:	DxShaderObject	( p_pParent, eSHADER_TYPE_HULL	)
	,	m_pHullShader		( NULL							)
{
}

DxHullShader :: ~DxHullShader()
{
	SafeRelease( m_pHullShader );
}

void DxHullShader :: Bind()
{
	ID3D11DeviceContext * l_pDeviceContext;
	m_pRenderSystem->GetDevice()->GetImmediateContext( &l_pDeviceContext );
	l_pDeviceContext->HSSetShader( m_pHullShader, NULL, 0 );
	l_pDeviceContext->Release();
}

void DxHullShader :: Unbind()
{
	ID3D11DeviceContext * l_pDeviceContext;
	m_pRenderSystem->GetDevice()->GetImmediateContext( &l_pDeviceContext );
	l_pDeviceContext->HSSetShader( NULL, NULL, 0 );
	l_pDeviceContext->Release();
}

void DxHullShader :: DoRetrieveShader()
{
	if( m_pCompiled )
	{
		ID3D11Device * l_pDevice = m_pRenderSystem->GetDevice();

		if( l_pDevice )
		{
			HRESULT l_hr = l_pDevice->CreateHullShader( reinterpret_cast< DWORD* >( m_pCompiled->GetBufferPointer() ), m_pCompiled->GetBufferSize(), NULL, &m_pHullShader );
			dxDebugName( m_pHullShader, HSShader );

			if( l_hr == S_OK )
			{
				m_eStatus = eSHADER_STATUS_COMPILED;
			}
		}
	}
}

//*************************************************************************************************

DxDomainShader :: DxDomainShader( DxShaderProgram * p_pParent)
	:	DxShaderObject	( p_pParent, eSHADER_TYPE_DOMAIN	)
	,	m_pDomainShader		( NULL								)
{
}

DxDomainShader :: ~DxDomainShader()
{
	SafeRelease( m_pDomainShader );
}

void DxDomainShader :: Bind()
{
	ID3D11DeviceContext * l_pDeviceContext;
	m_pRenderSystem->GetDevice()->GetImmediateContext( &l_pDeviceContext );
	l_pDeviceContext->DSSetShader( m_pDomainShader, NULL, 0 );
	l_pDeviceContext->Release();
}

void DxDomainShader :: Unbind()
{
	ID3D11DeviceContext * l_pDeviceContext;
	m_pRenderSystem->GetDevice()->GetImmediateContext( &l_pDeviceContext );
	l_pDeviceContext->DSSetShader( NULL, NULL, 0 );
	l_pDeviceContext->Release();
}

void DxDomainShader :: DoRetrieveShader()
{
	if( m_pCompiled )
	{
		ID3D11Device * l_pDevice = m_pRenderSystem->GetDevice();

		if( l_pDevice )
		{
			HRESULT l_hr = l_pDevice->CreateDomainShader( reinterpret_cast< DWORD* >( m_pCompiled->GetBufferPointer() ), m_pCompiled->GetBufferSize(), NULL, &m_pDomainShader );
			dxDebugName( m_pDomainShader, DSShader );

			if( l_hr == S_OK )
			{
				m_eStatus = eSHADER_STATUS_COMPILED;
			}
		}
	}
}

//*************************************************************************************************
