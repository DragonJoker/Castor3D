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
#if defined( _MSC_VER)
#	if CASTOR_USE_UNICODE
	static const String GetDividerTypeFunctionABIName = cuT( "?GetDividerType@@YA?AV?$basic_string@_WU?$char_traits@_W@std@@V?$allocator@_W@2@@std@@XZ" );
#	else
	static const String GetDividerTypeFunctionABIName = cuT( "?GetDividerType@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ" );
#	endif
#	if defined( _WIN64 )
	static const String CreateDividerFunctionABIName = cuT( "?CreateDivider@@YAPEAVSubdivider@Castor3D@@XZ" );
	static const String DestroyDividerFunctionABIName = cuT( "?DestroyDivider@@YAXPEAVSubdivider@Castor3D@@@Z" );
# else
	static const String CreateDividerFunctionABIName = cuT( "?CreateDivider@@YAPAVSubdivider@Castor3D@@XZ" );
	static const String DestroyDividerFunctionABIName = cuT( "?DestroyDivider@@YAXPAVSubdivider@Castor3D@@@Z" );
# endif
#elif defined( __GNUG__)
	static const String GetDividerTypeFunctionABIName = cuT( "_Z14GetDividerTypev" );
	static const String CreateDividerFunctionABIName = cuT( "_Z13CreateDividerv" );
	static const String DestroyDividerFunctionABIName = cuT( "_Z14DestroyDividerPN8Castor3D10SubdividerE" );
#else
#	error "Implement ABI names for this compiler"
#endif

	DividerPlugin::DividerPlugin( DynamicLibrarySPtr p_pLibrary, Engine * p_engine )
		: PluginBase( ePLUGIN_TYPE_DIVIDER, p_pLibrary, *p_engine )
	{
		if ( !p_pLibrary->GetFunction( m_pfnGetDividerType, GetDividerTypeFunctionABIName ) )
		{
			String l_strError = cuT( "Error encountered while loading dll [" ) + p_pLibrary->GetPath().GetFileName() + cuT( "] GetDividerType plugin function : " );
			l_strError += System::GetLastErrorText();
			CASTOR_PLUGIN_EXCEPTION( string::string_cast< char >( l_strError ), false );
		}

		if ( !p_pLibrary->GetFunction( m_pfnCreateDivider, CreateDividerFunctionABIName ) )
		{
			String l_strError = cuT( "Error encountered while loading dll [" ) + p_pLibrary->GetPath().GetFileName() + cuT( "] CreateDivider plugin function : " );
			l_strError += System::GetLastErrorText();
			CASTOR_PLUGIN_EXCEPTION( string::string_cast< char >( l_strError ), false );
		}

		if ( !p_pLibrary->GetFunction( m_pfnDestroyDivider, DestroyDividerFunctionABIName ) )
		{
			String l_strError = cuT( "Error encountered while loading dll [" ) + p_pLibrary->GetPath().GetFileName() + cuT( "] DestroyDivider plugin function : " );
			l_strError += System::GetLastErrorText();
			CASTOR_PLUGIN_EXCEPTION( string::string_cast< char >( l_strError ), false );
		}

		if ( m_pfnOnLoad )
		{
			m_pfnOnLoad( GetOwner() );
		}
	}

	DividerPlugin::~DividerPlugin()
	{
		if ( m_pfnOnUnload )
		{
			m_pfnOnUnload( GetOwner() );
		}
	}

	Subdivider * DividerPlugin::CreateDivider()
	{
		Subdivider * l_return = nullptr;

		if ( m_pfnCreateDivider )
		{
			l_return = m_pfnCreateDivider();
		}

		return l_return;
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
