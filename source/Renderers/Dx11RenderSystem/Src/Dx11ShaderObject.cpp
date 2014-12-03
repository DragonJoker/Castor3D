#include "Dx11ShaderObject.hpp"
#include "Dx11ShaderProgram.hpp"
#include "Dx11RenderSystem.hpp"

#include <D3DCompiler.h>
#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace Dx11Render
{
	DxShaderObject::DxShaderObject( DxShaderProgram * p_pParent, eSHADER_TYPE p_eType )
		:	ShaderObjectBase( p_pParent, p_eType )
		,	m_pShaderProgram( p_pParent )
		,	m_pCompiled( NULL )
		,	m_pRenderSystem( static_cast< DxRenderSystem * >( p_pParent->GetRenderSystem() ) )
	{
		m_eShaderModel = eSHADER_MODEL_3;
	}

	DxShaderObject::~DxShaderObject()
	{
		DestroyProgram();
	}

	void DxShaderObject::DestroyProgram()
	{
		if ( m_eStatus == eSHADER_STATUS_COMPILED )
		{
			m_eStatus = eSHADER_STATUS_NOTCOMPILED;
		}

		SafeRelease( m_pCompiled );
	}

	void DxShaderObject::RetrieveCompilerLog( String & p_strCompilerLog )
	{
		int infologLength = 0;

		if ( infologLength > 0 )
		{
			char * infoLog = new char[infologLength];
			p_strCompilerLog = str_utils::from_str( infoLog );
			delete [] infoLog;
		}

		if ( p_strCompilerLog.size() > 0 )
		{
			p_strCompilerLog = p_strCompilerLog.substr( 0, p_strCompilerLog.size() - 1 );
		}
	}

	bool DxShaderObject::Compile()
	{
		static std::string l_strProfiles[eSHADER_TYPE_COUNT][eSHADER_MODEL_COUNT] =
		{
			{ "vs_4_0",	"vs_4_0",	"vs_4_0",	"vs_4_0",	"vs_5_0" }
			,	{ "",		"",			"gs_4_0",	"gs_4_0",	"gs_5_0" }
			,	{ "",		"",			"",			"hs_4_0",	"hs_5_0" }
			,	{ "",		"",			"",			"ds_4_0",	"ds_5_0" }
			,	{ "",		"ps_4_0",	"ps_4_0",	"ps_4_0",	"ps_5_0" }
			,	{ "",		"",			"",			"",			"cs_5_0" }
		};
		bool l_bReturn = false;
		std::string l_strSource;
		std::string l_strProfile;

		for ( int i = eSHADER_MODEL_5; i >= eSHADER_MODEL_1 && m_strLoadedSource.empty(); i-- )
		{
			if ( m_pRenderSystem->CheckSupport( eSHADER_MODEL( i ) ) )
			{
				m_strLoadedSource = m_arraySources[i];
				l_strProfile = l_strProfiles[m_eType][i];
			}
		}

		if ( m_pRenderSystem->UseShaders() && m_eStatus != eSHADER_STATUS_ERROR && !m_strLoadedSource.empty() )
		{
			l_strSource = str_utils::to_str( m_strLoadedSource );
			m_eStatus = eSHADER_STATUS_NOTCOMPILED;
			ID3DBlob *	l_pErrors = NULL;
			UINT		l_uiFlags = 0;
#if !defined( NDEBUG )
			l_uiFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_WARNINGS_ARE_ERRORS;
#endif
			HRESULT l_hr = D3DX11CompileFromMemory( l_strSource.c_str(), UINT( l_strSource.size() ), NULL, NULL, NULL, str_utils::to_str( GetEntryPoint() ).c_str(), l_strProfile.c_str(), l_uiFlags, 0, NULL, &m_pCompiled, &l_pErrors, NULL );

			if ( l_hr == S_OK )
			{
				Logger::LogMessage( cuT( "Shader compiled successfully" ) );
				DoRetrieveShader();
			}
			else if ( l_pErrors )
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
}
