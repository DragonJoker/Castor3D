#include "Dx9RenderSystem/PrecompiledHeader.hpp"

#include "Dx9RenderSystem/DxShaderObject.hpp"
#include "Dx9RenderSystem/DxShaderProgram.hpp"
#include "Dx9RenderSystem/DxContext.hpp"
#include "Dx9RenderSystem/DxRenderSystem.hpp"
#include "Dx9RenderSystem/DxFrameVariable.hpp"
//#include "Dx9RenderSystem/DirectX9.hpp"

using namespace Castor3D;
using namespace Castor;
using namespace Dx9Render;

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
	:	ShaderObjectBase	( p_pParent, p_eType	)
	,	m_pShaderProgram	( p_pParent				)
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

	m_mapParamsByName.clear();
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
	bool l_bReturn = false;
	m_strLoadedSource.clear();
	DxRenderSystem * l_pRS = static_cast< DxRenderSystem * >( m_pParent->GetRenderSystem() );

	for( int i = eSHADER_MODEL_5; i >= eSHADER_MODEL_1 && m_strLoadedSource.empty(); i-- )
	{
		if( l_pRS->CheckSupport( eSHADER_MODEL( i ) ) )
		{
			m_strLoadedSource = m_arraySources[i];
		}
	}

	if( l_pRS->UseShaders() && m_eStatus != eSHADER_STATUS_ERROR && !m_strLoadedSource.empty() )
	{
		m_eStatus = eSHADER_STATUS_NOTCOMPILED;
		DxShaderProgram * l_pProgram = static_cast< DxShaderProgram * >( m_pParent );

		if( l_pProgram->GetShaderEffect() )
		{
			m_eStatus = eSHADER_STATUS_COMPILED;
		}
		
		if( m_eStatus == eSHADER_STATUS_ERROR )
		{
			Logger::LogMessage( m_strLoadedSource );
			m_strLoadedSource.clear();
		}
		
		l_bReturn = m_eStatus == eSHADER_STATUS_COMPILED;
	}

	return l_bReturn;
}

bool DxShaderObject :: HasParameter( Castor::String const & p_strName )
{
	return GetParameter( p_strName ) != NULL;
}

D3DXHANDLE DxShaderObject :: GetParameter( Castor::String const & p_strName )
{
	D3DXHANDLE l_hReturn = NULL;

	if( m_eStatus == eSHADER_STATUS_COMPILED )
	{
		HandleStrMap::iterator l_it = m_mapParamsByName.find( p_strName );

		if( l_it == m_mapParamsByName.end() )
		{
			m_mapParamsByName.insert( std::make_pair( p_strName, static_cast< DxShaderProgram * >( m_pParent )->GetShaderEffect()->GetParameterByName( NULL, str_utils::to_str( p_strName ).c_str() ) ) );
			l_it = m_mapParamsByName.find( p_strName );
		}

		l_hReturn = l_it->second;
	}

	return l_hReturn;
}

void DxShaderObject :: SetParameter( Castor::String const & p_strName, Castor::Matrix4x4r const & p_mtxValue )
{
	D3DXHANDLE l_hParam = GetParameter( p_strName );

	if( l_hParam )
	{
#if CASTOR_USE_DOUBLE
		real const * l_pTmp = p_mtxValue.const_ptr();
		D3DXMATRIX l_matTmp(	l_pTmp[0],	l_pTmp[1],	l_pTmp[2],	l_pTmp[3]
							,	l_pTmp[4],	l_pTmp[5],	l_pTmp[6],	l_pTmp[7]
							,	l_pTmp[8],	l_pTmp[9],	l_pTmp[10],	l_pTmp[11]
							,	l_pTmp[12],	l_pTmp[13],	l_pTmp[14],	l_pTmp[15]	);
#else
		D3DXMATRIX l_matTmp( p_mtxValue.const_ptr() );
#endif
		static_cast< DxShaderProgram * >( m_pParent )->GetShaderEffect()->SetMatrix( l_hParam, &l_matTmp );
	}
}

void DxShaderObject :: SetParameter( Castor::String const & p_strName, Castor::Matrix3x3r const & p_mtxValue )
{
	D3DXHANDLE l_hParam = GetParameter( p_strName );

	if( l_hParam )
	{
		real const * l_pTmp = p_mtxValue.const_ptr();
		D3DXMATRIX l_matTmp(	l_pTmp[0],	l_pTmp[1],	l_pTmp[2],	0
							,	l_pTmp[3],	l_pTmp[4],	l_pTmp[5],	0
							,	l_pTmp[6],	l_pTmp[7],	l_pTmp[8],	0
							,	0,			0,			0,			1 );
		
		static_cast< DxShaderProgram * >( m_pParent )->GetShaderEffect()->SetMatrix( l_hParam, &l_matTmp );
	}
}

//*************************************************************************************************

DxVertexShader :: DxVertexShader( DxShaderProgram * p_pParent)
	:	DxShaderObject( p_pParent, eSHADER_TYPE_VERTEX)
	,	m_pVertexShader( nullptr)
{
}

DxVertexShader :: ~DxVertexShader()
{
	if( m_pVertexShader )
	{
		m_pVertexShader->Release();
		m_pVertexShader = NULL;
	}
}

void DxVertexShader :: CreateProgram()
{
}

bool DxVertexShader :: Compile()
{
	bool l_bReturn = DxShaderObject::Compile();

	if (l_bReturn)
	{
		DoRetrieveShader( static_cast< DxShaderProgram * >( m_pParent )->GetShaderEffect() );
	}

	return l_bReturn;
}

void DxVertexShader :: Bind()
{
}

void DxVertexShader :: Unbind()
{
}

void DxVertexShader :: DoRetrieveShader( ID3DXEffect * p_pEffect )
{
	p_pEffect->GetVertexShader( "mainVx", &m_pVertexShader );
}

//*************************************************************************************************

DxFragmentShader :: DxFragmentShader( DxShaderProgram * p_pParent)
	:	DxShaderObject( p_pParent, eSHADER_TYPE_PIXEL)
	,	m_pPixelShader( nullptr)
{
}

DxFragmentShader :: ~DxFragmentShader()
{
	if( m_pPixelShader )
	{
		m_pPixelShader->Release();
		m_pPixelShader = NULL;
	}
}

void DxFragmentShader :: CreateProgram()
{
}

bool DxFragmentShader :: Compile()
{
	bool l_bReturn = DxShaderObject::Compile();

	if (l_bReturn)
	{
		DoRetrieveShader( static_cast< DxShaderProgram * >( m_pParent )->GetShaderEffect() );
	}

	return l_bReturn;
}

void DxFragmentShader :: Bind()
{
}

void DxFragmentShader :: Unbind()
{
}

void DxFragmentShader :: DoRetrieveShader( ID3DXEffect * p_pEffect )
{
	p_pEffect->GetPixelShader( "mainPx", &m_pPixelShader );
}

//*************************************************************************************************
