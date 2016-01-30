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
	static const String CreateEffectFunctionABIName = cuT( "?CreateEffect@@YA?AV?$shared_ptr@VPostEffect@Castor3D@@@tr1@std@@PEAVRenderSystem@Castor3D@@EAVRenderTarget@4@ABVParameters@4@@Z" );
#		else
	static const String CreateEffectFunctionABIName = cuT( "?CreateEffect@@YA?AV?$shared_ptr@VPostEffect@Castor3D@@@std@@PEAVRenderSystem@Castor3D@@EAVRenderTarget@4@ABVParameters@4@@Z" );
#		endif
#	else
#		if ( _MSC_VER < 1700 )
	static const String CreateEffectFunctionABIName = cuT( "?CreateEffect@@YA?AV?$shared_ptr@VPostEffect@Castor3D@@@tr1@std@@PAVRenderSystem@Castor3D@@AAVRenderTarget@4@ABVParameters@4@@Z" );
#		else
	static const String CreateEffectFunctionABIName = cuT( "?CreateEffect@@YA?AV?$shared_ptr@VPostEffect@Castor3D@@@std@@PAVRenderSystem@Castor3D@@AAVRenderTarget@4@ABVParameters@4@@Z" );
#		endif
#	endif
#	if CASTOR_USE_UNICODE
	static const String GetPostEffectTypeFunctionABIName = cuT( "?GetPostEffectType@@YA?AV?$basic_string@_WU?$char_traits@_W@std@@V?$allocator@_W@2@@std@@XZ" );
#	else
	static const String GetPostEffectTypeFunctionABIName = cuT( "?GetPostEffectType@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ" );
#	endif
#elif defined( __GNUG__)
	static const String CreateEffectFunctionABIName = cuT( "_Z12CreateEffectPN8Castor3D12RenderSystemERNS_12RenderTargetERKNS_10ParametersE" );
#	if GCC_VERSION >= 50300
	static const String GetPostEffectTypeFunctionABIName = cuT( "_Z17GetPostEffectTypeB5cxx11v" );
#else
	static const String GetPostEffectTypeFunctionABIName = cuT( "_Z17GetPostEffectTypev" );
#endif
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

		if ( !p_pLibrary->GetFunction( m_pfnGetPostEffectType, GetPostEffectTypeFunctionABIName ) )
		{
			String l_strError = cuT( "Error encountered while loading dll [" ) + p_pLibrary->GetPath().GetFileName() + cuT( "] GetPostEffectType plugin function : " );
			l_strError += System::GetLastErrorText();
			CASTOR_PLUGIN_EXCEPTION( string::string_cast< char >( l_strError ), false );
		}

		if ( m_pfnOnLoad )
		{
			m_pfnOnLoad( GetEngine() );
		}
	}

	PostFxPlugin::~PostFxPlugin()
	{
		if ( m_pfnOnUnload )
		{
			m_pfnOnUnload( GetEngine() );
		}
	}

	PostEffectSPtr PostFxPlugin::CreateEffect( RenderSystem * p_renderSystem, RenderTarget & p_renderTarget, Parameters const & p_params )
	{
		PostEffectSPtr l_return;

		if ( m_pfnCreateEffect )
		{
			l_return = m_pfnCreateEffect( p_renderSystem, p_renderTarget, p_params );
		}

		return l_return;
	}

	String PostFxPlugin::GetPostEffectType()
	{
		String l_strReturn;

		if ( m_pfnGetPostEffectType )
		{
			l_strReturn = m_pfnGetPostEffectType();
		}

		return l_strReturn;
	}
}
