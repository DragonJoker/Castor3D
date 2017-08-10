#include "Plugin.hpp"

#include "Miscellaneous/Version.hpp"

#include <Miscellaneous/DynamicLibrary.hpp>

using namespace castor;

namespace castor3d
{
	static const String GetNameFunctionABIName = cuT( "getName" );
	static const String GetRequiredVersionFunctionABIName = cuT( "getRequiredVersion" );
	static const String GetOnLoadFunctionABIName = cuT( "OnLoad" );
	static const String GetOnUnloadFunctionABIName = cuT( "OnUnload" );

	Plugin::Plugin( PluginType p_type, DynamicLibrarySPtr p_library, Engine & engine )
		: OwnedBy< Engine >( engine )
		, m_pfnGetRequiredVersion( 0 )
		, m_pfnGetName( 0 )
		, m_type( p_type )
	{
		if ( !p_library->getFunction( m_pfnGetName, GetNameFunctionABIName ) )
		{
			String strError = cuT( "Error encountered while loading dll [" ) + p_library->getPath().getFileName() + cuT( "] plug-in getName function : " );
			strError += System::getLastErrorText();
			CASTOR_PLUGIN_EXCEPTION( string::stringCast< char >( strError ), true );
		}

		if ( !p_library->getFunction( m_pfnGetRequiredVersion, GetRequiredVersionFunctionABIName ) )
		{
			String strError = cuT( "Error encountered while loading dll [" ) + p_library->getPath().getFileName() + cuT( "] plug-in getRequiredVersion function : " );
			strError += System::getLastErrorText();
			CASTOR_PLUGIN_EXCEPTION( string::stringCast< char >( strError ), true );
		}

		if ( !p_library->getFunction( m_pfnOnLoad, GetOnLoadFunctionABIName ) )
		{
			String strError = cuT( "Error encountered while loading dll [" ) + p_library->getPath().getFileName() + cuT( "] plug-in OnLoad function : " );
			strError += System::getLastErrorText();
			CASTOR_PLUGIN_EXCEPTION( string::stringCast< char >( strError ), true );
		}

		if ( !p_library->getFunction( m_pfnOnUnload, GetOnUnloadFunctionABIName ) )
		{
			String strError = cuT( "Error encountered while loading dll [" ) + p_library->getPath().getFileName() + cuT( "] plug-in OnUnload function : " );
			strError += System::getLastErrorText();
			CASTOR_PLUGIN_EXCEPTION( string::stringCast< char >( strError ), true );
		}
	}

	Plugin::~Plugin()
	{
	}

	void Plugin::getRequiredVersion( Version & p_version )const
	{
		if ( m_pfnGetRequiredVersion )
		{
			m_pfnGetRequiredVersion( &p_version );
		}
	}

	String Plugin::getName()const
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
