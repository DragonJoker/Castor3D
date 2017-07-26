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

	Plugin::Plugin( PluginType p_type, DynamicLibrarySPtr p_library, Engine & engine )
		: OwnedBy< Engine >( engine )
		, m_pfnGetRequiredVersion( 0 )
		, m_pfnGetName( 0 )
		, m_type( p_type )
	{
		if ( !p_library->GetFunction( m_pfnGetName, GetNameFunctionABIName ) )
		{
			String strError = cuT( "Error encountered while loading dll [" ) + p_library->GetPath().GetFileName() + cuT( "] plug-in GetName function : " );
			strError += System::GetLastErrorText();
			CASTOR_PLUGIN_EXCEPTION( string::string_cast< char >( strError ), true );
		}

		if ( !p_library->GetFunction( m_pfnGetRequiredVersion, GetRequiredVersionFunctionABIName ) )
		{
			String strError = cuT( "Error encountered while loading dll [" ) + p_library->GetPath().GetFileName() + cuT( "] plug-in GetRequiredVersion function : " );
			strError += System::GetLastErrorText();
			CASTOR_PLUGIN_EXCEPTION( string::string_cast< char >( strError ), true );
		}

		if ( !p_library->GetFunction( m_pfnOnLoad, GetOnLoadFunctionABIName ) )
		{
			String strError = cuT( "Error encountered while loading dll [" ) + p_library->GetPath().GetFileName() + cuT( "] plug-in OnLoad function : " );
			strError += System::GetLastErrorText();
			CASTOR_PLUGIN_EXCEPTION( string::string_cast< char >( strError ), true );
		}

		if ( !p_library->GetFunction( m_pfnOnUnload, GetOnUnloadFunctionABIName ) )
		{
			String strError = cuT( "Error encountered while loading dll [" ) + p_library->GetPath().GetFileName() + cuT( "] plug-in OnUnload function : " );
			strError += System::GetLastErrorText();
			CASTOR_PLUGIN_EXCEPTION( string::string_cast< char >( strError ), true );
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
		String strReturn;

		if ( m_pfnGetName )
		{
			char const * name;
			m_pfnGetName( &name );
			strReturn = name;
		}

		return strReturn;
	}
}
