#include "ShaderPlugin.hpp"

#if defined( _WIN32 )
#	include <Windows.h>
#else
#	include <dlfcn.h>
#endif

#include <DynamicLibrary.hpp>

using namespace Castor;

namespace Castor3D
{
#pragma warning( disable:4290 )
#if defined( _MSC_VER)
	static const String GetShaderLanguageFunctionABIName	= cuT( "?GetShaderLanguage@@YA?AW4eSHADER_LANGUAGE@Castor3D@@XZ" );
#	if defined( _WIN64 )
	static const String CreatePipelineFunctionABIName		= cuT( "?CreatePipeline@@YAPEAVIPipelineImpl@Castor3D@@PEAVPipeline@2@PEAVRenderSystem@2@@Z" );
	static const String DestroyPipelineFunctionABIName		= cuT( "?DestroyPipeline@@YAXPEAVIPipelineImpl@Castor3D@@@Z" );
#		if ( _MSC_VER < 1700 )
	static const String CreateShaderFunctionABIName			= cuT( "?CreateShader@@YA?AV?$shared_ptr@VShaderProgramBase@Castor3D@@@tr1@std@@PEAVRenderSystem@Castor3D@@@Z" );
#		else
	static const String CreateShaderFunctionABIName			= cuT( "?CreateShader@@YA?AV?$shared_ptr@VShaderProgramBase@Castor3D@@@std@@PEAVRenderSystem@Castor3D@@@Z" );
#		endif
#	else
	static const String CreatePipelineFunctionABIName		= cuT( "?CreatePipeline@@YAPAVIPipelineImpl@Castor3D@@PAVPipeline@2@PAVRenderSystem@2@@Z" );
	static const String DestroyPipelineFunctionABIName		= cuT( "?DestroyPipeline@@YAXPAVIPipelineImpl@Castor3D@@@Z" );
#		if ( _MSC_VER < 1700 )
	static const String CreateShaderFunctionABIName			= cuT( "?CreateShader@@YA?AV?$shared_ptr@VShaderProgramBase@Castor3D@@@tr1@std@@PAVRenderSystem@Castor3D@@@Z" );
#		else
	static const String CreateShaderFunctionABIName			= cuT( "?CreateShader@@YA?AV?$shared_ptr@VShaderProgramBase@Castor3D@@@std@@PAVRenderSystem@Castor3D@@@Z" );
#		endif
#	endif
#elif defined( __GNUG__)
	static const String GetShaderLanguageFunctionABIName	= cuT( "_Z17GetShaderLanguagev" );
	static const String CreateShaderFunctionABIName			= cuT( "_Z12CreateShaderPN8Castor3D12RenderSystemE" );
	static const String CreatePipelineFunctionABIName		= cuT( "_Z14CreatePipelinePN8Castor3D12RenderSystemE" );
	static const String DestroyPipelineFunctionABIName		= cuT( "_Z15DestroyPipelinePN8Castor3D13IPipelineImplE" );
#else
#	error "Implement ABI names for this compiler"
#endif

	ShaderPlugin::ShaderPlugin( DynamicLibrarySPtr p_pLibrary )
		:	PluginBase( ePLUGIN_TYPE_PROGRAM, p_pLibrary )
	{
		if ( !p_pLibrary->GetFunction( m_pfnGetShaderLanguage, GetShaderLanguageFunctionABIName ) )
		{
			String l_strError = cuT( "Error encountered while loading dll [" ) + p_pLibrary->GetPath().GetFileName() + cuT( "] GetShaderLanguage plugin function : " );
			l_strError += str_utils::to_string( dlerror() );
			CASTOR_PLUGIN_EXCEPTION( str_utils::to_str( l_strError ), false );
		}

		if ( !p_pLibrary->GetFunction( m_pfnCreateShader, CreateShaderFunctionABIName ) )
		{
			String l_strError = cuT( "Error encountered while loading dll [" ) + p_pLibrary->GetPath().GetFileName() + cuT( "] CreateShader plugin function : " );
			l_strError += str_utils::to_string( dlerror() );
			CASTOR_PLUGIN_EXCEPTION( str_utils::to_str( l_strError ), false );
		}

		if ( !p_pLibrary->GetFunction( m_pfnCreatePipeline, CreatePipelineFunctionABIName ) )
		{
			String l_strError = cuT( "Error encountered while loading dll [" ) + p_pLibrary->GetPath().GetFileName() + cuT( "] CreatePipeline plugin function : " );
			l_strError += str_utils::to_string( dlerror() );
			CASTOR_PLUGIN_EXCEPTION( str_utils::to_str( l_strError ), false );
		}

		if ( !p_pLibrary->GetFunction( m_pfnDestroyPipeline, DestroyPipelineFunctionABIName ) )
		{
			String l_strError = cuT( "Error encountered while loading dll [" ) + p_pLibrary->GetPath().GetFileName() + cuT( "] DestroyPipeline plugin function : " );
			l_strError += str_utils::to_string( dlerror() );
			CASTOR_PLUGIN_EXCEPTION( str_utils::to_str( l_strError ), false );
		}
	}

	ShaderPlugin::ShaderPlugin( ShaderPlugin const & p_plugin )
		:	PluginBase( p_plugin )
		,	m_pfnCreateShader( p_plugin.m_pfnCreateShader )
		,	m_pfnCreatePipeline( p_plugin.m_pfnCreatePipeline )
		,	m_pfnDestroyPipeline( p_plugin.m_pfnDestroyPipeline )
		,	m_pfnGetShaderLanguage( p_plugin.m_pfnGetShaderLanguage )
	{
	}

	ShaderPlugin::ShaderPlugin( ShaderPlugin && p_plugin )
		:	PluginBase( std::move( p_plugin ) )
		,	m_pfnCreateShader( std::move( p_plugin.m_pfnCreateShader ) )
		,	m_pfnCreatePipeline( std::move( p_plugin.m_pfnCreatePipeline ) )
		,	m_pfnDestroyPipeline( std::move( p_plugin.m_pfnDestroyPipeline ) )
		,	m_pfnGetShaderLanguage( std::move( p_plugin.m_pfnGetShaderLanguage ) )
	{
		p_plugin.m_pfnCreateShader		= NULL;
		p_plugin.m_pfnCreatePipeline	= NULL;
		p_plugin.m_pfnDestroyPipeline	= NULL;
		p_plugin.m_pfnGetShaderLanguage	= NULL;
	}

	ShaderPlugin::~ShaderPlugin()
	{
	}

	ShaderPlugin & ShaderPlugin::operator =( ShaderPlugin const & p_plugin )
	{
		PluginBase::operator =( p_plugin );
		m_pfnCreateShader		= p_plugin.m_pfnCreateShader;
		m_pfnGetShaderLanguage	= p_plugin.m_pfnGetShaderLanguage;
		m_pfnCreatePipeline		= p_plugin.m_pfnCreatePipeline;
		m_pfnDestroyPipeline	= p_plugin.m_pfnDestroyPipeline;
		return * this;
	}

	ShaderPlugin & ShaderPlugin::operator =( ShaderPlugin && p_plugin )
	{
		PluginBase::operator =( std::move( p_plugin ) );

		if ( this != & p_plugin )
		{
			m_pfnCreateShader		= std::move( p_plugin.m_pfnCreateShader );
			m_pfnGetShaderLanguage	= std::move( p_plugin.m_pfnGetShaderLanguage );
			m_pfnCreatePipeline		= std::move( p_plugin.m_pfnCreatePipeline );
			m_pfnDestroyPipeline	= std::move( p_plugin.m_pfnDestroyPipeline );
			p_plugin.m_pfnCreateShader		= NULL;
			p_plugin.m_pfnCreatePipeline	= NULL;
			p_plugin.m_pfnDestroyPipeline	= NULL;
			p_plugin.m_pfnGetShaderLanguage	= NULL;
		}

		return * this;
	}

	ShaderProgramBaseSPtr ShaderPlugin::CreateShader( RenderSystem * p_pRenderSystem )
	{
		ShaderProgramBaseSPtr l_pReturn;

		if ( m_pfnCreateShader )
		{
			l_pReturn = m_pfnCreateShader( p_pRenderSystem );
		}

		return l_pReturn;
	}

	IPipelineImpl * ShaderPlugin::CreatePipeline( Pipeline * p_pPipeline, RenderSystem * p_pRenderSystem )
	{
		IPipelineImpl * l_pReturn = NULL;

		if ( m_pfnCreateShader )
		{
			l_pReturn = m_pfnCreatePipeline( p_pPipeline, p_pRenderSystem );
		}

		return l_pReturn;
	}

	void ShaderPlugin::DestroyPipeline( IPipelineImpl * p_pPipeline )
	{
		if ( m_pfnDestroyPipeline )
		{
			m_pfnDestroyPipeline( p_pPipeline );
		}
	}

	eSHADER_LANGUAGE ShaderPlugin::GetShaderLanguage()
	{
		eSHADER_LANGUAGE l_eReturn = eSHADER_LANGUAGE_COUNT;

		if ( m_pfnGetShaderLanguage )
		{
			l_eReturn = m_pfnGetShaderLanguage();
		}

		return l_eReturn;
	}
}
