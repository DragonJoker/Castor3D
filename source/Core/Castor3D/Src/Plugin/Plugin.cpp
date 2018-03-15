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

	Plugin::Plugin( PluginType type, DynamicLibrarySPtr library, Engine & engine )
		: OwnedBy< Engine >( engine )
		, m_pfnGetRequiredVersion( 0 )
		, m_pfnGetName( 0 )
		, m_type( type )
		, m_library( library )
	{
		if ( !library->getFunction( m_pfnGetName, GetNameFunctionABIName ) )
		{
			String strError = cuT( "Error encountered while loading dll [" ) + library->getPath().getFileName() + cuT( "] plug-in getName function : " );
			strError += System::getLastErrorText();
			CASTOR_PLUGIN_EXCEPTION( string::stringCast< char >( strError ), true );
		}

		if ( !library->getFunction( m_pfnGetRequiredVersion, GetRequiredVersionFunctionABIName ) )
		{
			String strError = cuT( "Error encountered while loading dll [" ) + library->getPath().getFileName() + cuT( "] plug-in getRequiredVersion function : " );
			strError += System::getLastErrorText();
			CASTOR_PLUGIN_EXCEPTION( string::stringCast< char >( strError ), true );
		}

		if ( !library->getFunction( m_pfnOnLoad, GetOnLoadFunctionABIName ) )
		{
			String strError = cuT( "Error encountered while loading dll [" ) + library->getPath().getFileName() + cuT( "] plug-in OnLoad function : " );
			strError += System::getLastErrorText();
			CASTOR_PLUGIN_EXCEPTION( string::stringCast< char >( strError ), true );
		}

		if ( !library->getFunction( m_pfnOnUnload, GetOnUnloadFunctionABIName ) )
		{
			String strError = cuT( "Error encountered while loading dll [" ) + library->getPath().getFileName() + cuT( "] plug-in OnUnload function : " );
			strError += System::getLastErrorText();
			CASTOR_PLUGIN_EXCEPTION( string::stringCast< char >( strError ), true );
		}

	}

	Plugin::~Plugin()
	{
	}

	void Plugin::getRequiredVersion( Version & version )const
	{
		if ( m_pfnGetRequiredVersion )
		{
			m_pfnGetRequiredVersion( &version );
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


	void Plugin::load()
	{
		if ( m_pfnOnLoad )
		{
#if !defined( NDEBUG )
			auto library = m_library.lock();

			if ( library )
			{
				Debug::loadModule( *library );
			}
#endif

			m_pfnOnLoad( getEngine(), this );
		}
	}

	void Plugin::unload()
	{
		if ( m_pfnOnUnload )
		{
			m_pfnOnUnload( getEngine() );

#if !defined( NDEBUG )
			auto library = m_library.lock();

			if ( library )
			{
				Debug::unloadModule( *library );
			}
#endif
		}
	}
}
