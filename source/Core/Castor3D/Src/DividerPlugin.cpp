#include "DividerPlugin.hpp"

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
#	if defined( _UNICODE )
	static const String GetDividerTypeFunctionABIName		= cuT( "?GetDividerType@@YA?AV?$basic_string@_WU?$char_traits@_W@std@@V?$allocator@_W@2@@std@@XZ" );
#	else
	static const String GetDividerTypeFunctionABIName		= cuT( "?GetDividerType@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ" );
#	endif
	static const String CreateDividerFunctionABIName		= cuT( "?CreateDivider@@YAPAVSubdivider@Castor3D@@XZ" );
	static const String DestroyDividerFunctionABIName		= cuT( "?DestroyDivider@@YAXPAVSubdivider@Castor3D@@@Z" );
#elif defined( __GNUG__)
	static const String GetDividerTypeFunctionABIName		= cuT( "_Z14GetDividerTypev" );
	static const String CreateDividerFunctionABIName		= cuT( "_Z13CreateDividerv" );
	static const String DestroyDividerFunctionABIName		= cuT( "_Z14DestroyDividerPN8Castor3D10SubdividerE" );
#else
#	error "Implement ABI names for this compiler"
#endif

	DividerPlugin::DividerPlugin( DynamicLibrarySPtr p_pLibrary, Engine * p_engine )
		:	PluginBase( ePLUGIN_TYPE_DIVIDER, p_pLibrary, p_engine )
	{
		if ( !p_pLibrary->GetFunction( m_pfnGetDividerType, GetDividerTypeFunctionABIName ) )
		{
			String l_strError = cuT( "Error encountered while loading dll [" ) + p_pLibrary->GetPath().GetFileName() + cuT( "] GetDividerType plugin function : " );
			l_strError += str_utils::to_string( dlerror() );
			CASTOR_PLUGIN_EXCEPTION( str_utils::to_str( l_strError ), false );
		}

		if ( !p_pLibrary->GetFunction( m_pfnCreateDivider, CreateDividerFunctionABIName ) )
		{
			String l_strError = cuT( "Error encountered while loading dll [" ) + p_pLibrary->GetPath().GetFileName() + cuT( "] CreateDivider plugin function : " );
			l_strError += str_utils::to_string( dlerror() );
			CASTOR_PLUGIN_EXCEPTION( str_utils::to_str( l_strError ), false );
		}

		if ( !p_pLibrary->GetFunction( m_pfnDestroyDivider, DestroyDividerFunctionABIName ) )
		{
			String l_strError = cuT( "Error encountered while loading dll [" ) + p_pLibrary->GetPath().GetFileName() + cuT( "] DestroyDivider plugin function : " );
			l_strError += str_utils::to_string( dlerror() );
			CASTOR_PLUGIN_EXCEPTION( str_utils::to_str( l_strError ), false );
		}

		if ( m_pfnOnLoad )
		{
			m_pfnOnLoad( m_engine );
		}
	}

	DividerPlugin::~DividerPlugin()
	{
		if ( m_pfnOnUnload )
		{
			m_pfnOnUnload( m_engine );
		}
	}

	Subdivider * DividerPlugin::CreateDivider()
	{
		Subdivider * l_pReturn = nullptr;

		if ( m_pfnCreateDivider )
		{
			l_pReturn = m_pfnCreateDivider();
		}

		return l_pReturn;
	}

	void DividerPlugin::DestroyDivider( Subdivider * p_pDivider )
	{
		if ( m_pfnDestroyDivider )
		{
			m_pfnDestroyDivider( p_pDivider );
		}
	}

	String DividerPlugin::GetDividerType()
	{
		String l_strReturn;

		if ( m_pfnGetDividerType )
		{
			l_strReturn = m_pfnGetDividerType();
		}

		return l_strReturn;
	}
}
