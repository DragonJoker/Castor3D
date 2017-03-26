#include "Plugin.hpp"

#include "Miscellaneous/Version.hpp"

#include <Miscellaneous/DynamicLibrary.hpp>

using namespace Castor;

namespace Castor3D
{
	static const String GetNameFunctionABIName = cuT( "GetName" );
	static const String GetRequiredVersionFunctionABIName = cuT( "GetRequiredVersion" );
	static const String GetOnLoadFunctionABIName = cuT( "OnLoad" );
	static const String GetOnUnloadFunctionABIName = cuT( "OnUnload" );

	Plugin::Plugin( PluginType p_type, DynamicLibrarySPtr p_library, Engine & p_engine )
		: OwnedBy< Engine >( p_engine )
		, m_pfnGetRequiredVersion( 0 )
		, m_pfnGetName( 0 )
		, m_type( p_type )
	{
		if ( !p_library->GetFunction( m_pfnGetName, GetNameFunctionABIName ) )
		{
			String l_strError = cuT( "Error encountered while loading dll [" ) + p_library->GetPath().GetFileName() + cuT( "] plug-in GetName function : " );
			l_strError += System::GetLastErrorText();
			CASTOR_PLUGIN_EXCEPTION( string::string_cast< char >( l_strError ), true );
		}

		if ( !p_library->GetFunction( m_pfnGetRequiredVersion, GetRequiredVersionFunctionABIName ) )
		{
			String l_strError = cuT( "Error encountered while loading dll [" ) + p_library->GetPath().GetFileName() + cuT( "] plug-in GetRequiredVersion function : " );
			l_strError += System::GetLastErrorText();
			CASTOR_PLUGIN_EXCEPTION( string::string_cast< char >( l_strError ), true );
		}

		if ( !p_library->GetFunction( m_pfnOnLoad, GetOnLoadFunctionABIName ) )
		{
			String l_strError = cuT( "Error encountered while loading dll [" ) + p_library->GetPath().GetFileName() + cuT( "] plug-in OnLoad function : " );
			l_strError += System::GetLastErrorText();
			CASTOR_PLUGIN_EXCEPTION( string::string_cast< char >( l_strError ), true );
		}

		if ( !p_library->GetFunction( m_pfnOnUnload, GetOnUnloadFunctionABIName ) )
		{
			String l_strError = cuT( "Error encountered while loading dll [" ) + p_library->GetPath().GetFileName() + cuT( "] plug-in OnUnload function : " );
			l_strError += System::GetLastErrorText();
			CASTOR_PLUGIN_EXCEPTION( string::string_cast< char >( l_strError ), true );
		}
	}

	Plugin::~Plugin()
	{
	}

	void Plugin::GetRequiredVersion( Version & p_version )const
	{
		if ( m_pfnGetRequiredVersion )
		{
			m_pfnGetRequiredVersion( &p_version );
		}
	}

	String Plugin::GetName()const
	{
		String l_strReturn;

		if ( m_pfnGetName )
		{
			char const * l_name;
			m_pfnGetName( &l_name );
			l_strReturn = l_name;
		}

		return l_strReturn;
	}
}
