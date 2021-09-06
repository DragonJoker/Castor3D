#include "Castor3D/Plugin/Plugin.hpp"

#include "Castor3D/Cache/PluginCache.hpp"
#include "Castor3D/Miscellaneous/Version.hpp"
#include "Castor3D/Plugin/PluginException.hpp"

#include <CastorUtils/Miscellaneous/DynamicLibrary.hpp>
#include <CastorUtils/Miscellaneous/Utils.hpp>

namespace castor3d
{
	static const castor::String GetNameFunctionABIName = cuT( "getName" );
	static const castor::String GetRequiredVersionFunctionABIName = cuT( "getRequiredVersion" );
	static const castor::String GetOnLoadFunctionABIName = cuT( "OnLoad" );
	static const castor::String GetOnUnloadFunctionABIName = cuT( "OnUnload" );

	Plugin::Plugin( PluginType type, castor::DynamicLibrarySPtr library, Engine & engine )
		: OwnedBy< Engine >( engine )
		, m_pfnGetRequiredVersion( 0 )
		, m_pfnGetName( 0 )
		, m_type( type )
		, m_library( library )
	{
		if ( !library->getFunction( m_pfnGetName, GetNameFunctionABIName ) )
		{
			castor::String strError = cuT( "Error encountered while loading dll [" ) + library->getPath().getFileName() + cuT( "] plug-in getName function : " );
			strError += castor::System::getLastErrorText();
			CASTOR_PLUGIN_EXCEPTION( castor::string::stringCast< char >( strError ), true );
		}

		if ( !library->getFunction( m_pfnGetRequiredVersion, GetRequiredVersionFunctionABIName ) )
		{
			castor::String strError = cuT( "Error encountered while loading dll [" ) + library->getPath().getFileName() + cuT( "] plug-in getRequiredVersion function : " );
			strError += castor::System::getLastErrorText();
			CASTOR_PLUGIN_EXCEPTION( castor::string::stringCast< char >( strError ), true );
		}

		if ( !library->getFunction( m_pfnOnLoad, GetOnLoadFunctionABIName ) )
		{
			castor::String strError = cuT( "Error encountered while loading dll [" ) + library->getPath().getFileName() + cuT( "] plug-in OnLoad function : " );
			strError += castor::System::getLastErrorText();
			CASTOR_PLUGIN_EXCEPTION( castor::string::stringCast< char >( strError ), true );
		}

		if ( !library->getFunction( m_pfnOnUnload, GetOnUnloadFunctionABIName ) )
		{
			castor::String strError = cuT( "Error encountered while loading dll [" ) + library->getPath().getFileName() + cuT( "] plug-in OnUnload function : " );
			strError += castor::System::getLastErrorText();
			CASTOR_PLUGIN_EXCEPTION( castor::string::stringCast< char >( strError ), true );
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

	castor::String Plugin::getName()const
	{
		castor::String strReturn;

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
				castor::Debug::loadModule( *library );
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
				castor::Debug::unloadModule( *library );
			}
#endif
		}
	}
}
