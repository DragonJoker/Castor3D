#include "Dx11ShaderObject.hpp"
#include "Dx11ShaderProgram.hpp"
#include "Dx11RenderSystem.hpp"

#include <D3DCompiler.h>
#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace Dx11Render
{
	DxShaderObject::DxShaderObject( DxShaderProgram * p_parent, eSHADER_TYPE p_type )
		: ShaderObjectBase( p_parent, p_type )
		, m_pShaderProgram( p_parent )
		, m_pCompiled( NULL )
		, m_renderSystem( static_cast< DxRenderSystem * >( p_parent->GetRenderSystem() ) )
	{
		m_eShaderModel = eSHADER_MODEL_3;
	}

	DxShaderObject::~DxShaderObject()
	{
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
			p_strCompilerLog = string::string_cast< xchar >( infoLog );
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
			{ "vs_2_0",	"vs_2_0",	"vs_4_0",	"vs_4_1",	"vs_5_0" },
			{ "",		"",			"gs_4_0",	"gs_4_1",	"gs_5_0" },
			{ "",		"",			"",			"hs_4_1",	"hs_5_0" },
			{ "",		"",			"",			"ds_4_1",	"ds_5_0" },
			{ "ps_2_0",	"ps_2_0",	"ps_4_0",	"ps_4_1",	"ps_5_0" },
			{ "",		"",			"",			"",			"cs_5_0" },
		};
		bool l_return = false;
		std::string l_strSource;
		std::string l_strProfile;

		for ( int i = eSHADER_MODEL_5; i >= eSHADER_MODEL_1 && m_strLoadedSource.empty(); i-- )
		{
			if ( m_renderSystem->CheckSupport( eSHADER_MODEL( i ) ) )
			{
				m_strLoadedSource = m_arraySources[i];
				l_strProfile = l_strProfiles[m_type][i];
			}
		}

		if ( m_renderSystem->UseShaders() && m_eStatus != eSHADER_STATUS_ERROR && !m_strLoadedSource.empty() )
		{
			l_strSource = string::string_cast< char >( m_strLoadedSource );
			m_eStatus = eSHADER_STATUS_NOTCOMPILED;
			ID3DBlob * l_pErrors = NULL;
			UINT l_uiFlags = 0;
#if !defined( NDEBUG )
			l_uiFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_WARNINGS_ARE_ERRORS;
#endif
			HRESULT l_hr = D3DX11CompileFromMemory( l_strSource.c_str(), UINT( l_strSource.size() ), NULL, NULL, NULL, string::string_cast< char >( GetEntryPoint() ).c_str(), l_strProfile.c_str(), l_uiFlags, 0, NULL, &m_pCompiled, &l_pErrors, NULL );

			if ( l_hr == S_OK )
			{
				Logger::LogInfo( cuT( "Shader compiled successfully" ) );
			}
			else if ( l_pErrors )
			{
				Logger::LogInfo( string::string_cast< xchar >( reinterpret_cast< char * >( l_pErrors->GetBufferPointer() ) ) );
				Logger::LogInfo( m_strLoadedSource );
				m_eStatus = eSHADER_STATUS_ERROR;
				m_strLoadedSource.clear();
			}

			SafeRelease( l_pErrors );
			l_return = m_eStatus == eSHADER_STATUS_COMPILED;
		}

		return l_return;
	}
}
