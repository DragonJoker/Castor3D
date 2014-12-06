#include "PostFxPlugin.hpp"

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
#	if defined( _WIN64 )
#		if ( _MSC_VER < 1700 )
	static const String CreateEffectFunctionABIName			= cuT( "?CreateEffect@@YA?AV?$shared_ptr@VPostEffect@Castor3D@@@tr1@std@@PEAVRenderSystem@Castor3D@@@Z" );
#		else
	static const String CreateEffectFunctionABIName			= cuT( "?CreateEffect@@YA?AV?$shared_ptr@VPostEffect@Castor3D@@@std@@PEAVRenderSystem@Castor3D@@@Z" );
#		endif
#	else
#		if ( _MSC_VER < 1700 )
	static const String CreateEffectFunctionABIName			= cuT( "?CreateEffect@@YA?AV?$shared_ptr@VPostEffect@Castor3D@@@tr1@std@@PAVRenderSystem@Castor3D@@@Z" );
#		else
	static const String CreateEffectFunctionABIName			= cuT( "?CreateEffect@@YA?AV?$shared_ptr@VPostEffect@Castor3D@@@std@@PAVRenderSystem@Castor3D@@@Z" );
#		endif
#	endif
#elif defined( __GNUG__)
	static const String CreateEffectFunctionABIName			= cuT( "_Z12CreateEffectPN8Castor3D12RenderSystemE" );
#else
#	error "Implement ABI names for this compiler"
#endif

	PostFxPlugin::PostFxPlugin( DynamicLibrarySPtr p_pLibrary )
		:	PluginBase( ePLUGIN_TYPE_POSTFX, p_pLibrary )
	{
		if ( !p_pLibrary->GetFunction( m_pfnCreateEffect, CreateEffectFunctionABIName ) )
		{
			String l_strError = cuT( "Error encountered while loading dll [" ) + p_pLibrary->GetPath().GetFileName() + cuT( "] CreateEffect plugin function : " );
			l_strError += str_utils::to_string( dlerror() );
			CASTOR_PLUGIN_EXCEPTION( str_utils::to_str( l_strError ), false );
		}
	}

	PostFxPlugin::PostFxPlugin( PostFxPlugin const & p_plugin )
		:	PluginBase( p_plugin )
		,	m_pfnCreateEffect( p_plugin.m_pfnCreateEffect )
	{
	}

	PostFxPlugin::PostFxPlugin( PostFxPlugin && p_plugin )
		:	PluginBase( std::move( p_plugin ) )
		,	m_pfnCreateEffect( std::move( p_plugin.m_pfnCreateEffect ) )
	{
		p_plugin.m_pfnCreateEffect	= NULL;
	}

	PostFxPlugin::~PostFxPlugin()
	{
	}

	PostFxPlugin & PostFxPlugin::operator =( PostFxPlugin const & p_plugin )
	{
		PluginBase::operator =( p_plugin );
		m_pfnCreateEffect		= p_plugin.m_pfnCreateEffect;
		return * this;
	}

	PostFxPlugin & PostFxPlugin::operator =( PostFxPlugin && p_plugin )
	{
		PluginBase::operator =( std::move( p_plugin ) );

		if ( this != & p_plugin )
		{
			m_pfnCreateEffect			= std::move( p_plugin.m_pfnCreateEffect );
			p_plugin.m_pfnCreateEffect	= NULL;
		}

		return * this;
	}

	PostEffectSPtr PostFxPlugin::CreateEffect( RenderSystem * p_pRenderSystem )
	{
		PostEffectSPtr l_pReturn;

		if ( m_pfnCreateEffect )
		{
			l_pReturn = m_pfnCreateEffect( p_pRenderSystem );
		}

		return l_pReturn;
	}
}
