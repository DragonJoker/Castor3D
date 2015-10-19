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
#if defined( _MSC_VER)
#	if defined( _WIN64 )
#		if ( _MSC_VER < 1700 )
	static const String CreateEffectFunctionABIName = cuT( "?CreateEffect@@YA?AV?$shared_ptr@VPostEffect@Castor3D@@@tr1@std@@PEAVRenderSystem@Castor3D@@@Z" );
#		else
	static const String CreateEffectFunctionABIName = cuT( "?CreateEffect@@YA?AV?$shared_ptr@VPostEffect@Castor3D@@@std@@PEAVRenderSystem@Castor3D@@@Z" );
#		endif
#	else
#		if ( _MSC_VER < 1700 )
	static const String CreateEffectFunctionABIName = cuT( "?CreateEffect@@YA?AV?$shared_ptr@VPostEffect@Castor3D@@@tr1@std@@PAVRenderSystem@Castor3D@@@Z" );
#		else
	static const String CreateEffectFunctionABIName = cuT( "?CreateEffect@@YA?AV?$shared_ptr@VPostEffect@Castor3D@@@std@@PAVRenderSystem@Castor3D@@@Z" );
#		endif
#	endif
#elif defined( __GNUG__)
	static const String CreateEffectFunctionABIName = cuT( "_Z12CreateEffectPN8Castor3D12RenderSystemE" );
#else
#	error "Implement ABI names for this compiler"
#endif

	PostFxPlugin::PostFxPlugin( DynamicLibrarySPtr p_pLibrary, Engine * p_engine )
		: PluginBase( ePLUGIN_TYPE_POSTFX, p_pLibrary, *p_engine )
	{
		if ( !p_pLibrary->GetFunction( m_pfnCreateEffect, CreateEffectFunctionABIName ) )
		{
			String l_strError = cuT( "Error encountered while loading dll [" ) + p_pLibrary->GetPath().GetFileName() + cuT( "] CreateEffect plugin function : " );
			l_strError += System::GetLastErrorText();
			CASTOR_PLUGIN_EXCEPTION( string::string_cast< char >( l_strError ), false );
		}

		if ( m_pfnOnLoad )
		{
			m_pfnOnLoad( GetOwner() );
		}
	}

	PostFxPlugin::~PostFxPlugin()
	{
		if ( m_pfnOnUnload )
		{
			m_pfnOnUnload( GetOwner() );
		}
	}

	PostEffectSPtr PostFxPlugin::CreateEffect( RenderSystem * p_renderSystem )
	{
		PostEffectSPtr l_return;

		if ( m_pfnCreateEffect )
		{
			l_return = m_pfnCreateEffect( p_renderSystem );
		}

		return l_return;
	}
}
