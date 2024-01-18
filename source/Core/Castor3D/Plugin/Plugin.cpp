#include "Castor3D/Plugin/Plugin.hpp"

#include "Castor3D/Cache/PluginCache.hpp"
#include "Castor3D/Miscellaneous/Version.hpp"
#include "Castor3D/Plugin/PluginException.hpp"

#include <CastorUtils/Miscellaneous/DynamicLibrary.hpp>
#include <CastorUtils/Miscellaneous/Utils.hpp>

CU_ImplementSmartPtr( castor3d, Plugin )

namespace castor3d
{
	namespace plugin
	{
		static const castor::String GetNameFunctionABIName = cuT( "getName" );
		static const castor::String GetRequiredVersionFunctionABIName = cuT( "getRequiredVersion" );
		static const castor::String GetOnLoadFunctionABIName = cuT( "OnLoad" );
		static const castor::String GetOnUnloadFunctionABIName = cuT( "OnUnload" );
	}

	Plugin::Plugin( PluginType type, castor::DynamicLibraryUPtr library, Engine & engine )
		: OwnedBy< Engine >( engine )
		, m_library{ std::move( library ) }
		, m_type{ type }
	{
		if ( !m_library->getFunction( m_pfnGetName, plugin::GetNameFunctionABIName ) )
		{
			castor::String strError = cuT( "Error encountered while loading dll [" ) + m_library->getPath().getFileName() + cuT( "] plug-in getName function : " );
			strError += castor::system::getLastErrorText();
			C3D_PluginException( castor::string::stringCast< char >( strError ), true );
		}

		if ( !m_library->getFunction( m_pfnGetRequiredVersion, plugin::GetRequiredVersionFunctionABIName ) )
		{
			castor::String strError = cuT( "Error encountered while loading dll [" ) + m_library->getPath().getFileName() + cuT( "] plug-in getRequiredVersion function : " );
			strError += castor::system::getLastErrorText();
			C3D_PluginException( castor::string::stringCast< char >( strError ), true );
		}

		if ( !m_library->getFunction( m_pfnOnLoad, plugin::GetOnLoadFunctionABIName ) )
		{
			castor::String strError = cuT( "Error encountered while loading dll [" ) + m_library->getPath().getFileName() + cuT( "] plug-in OnLoad function : " );
			strError += castor::system::getLastErrorText();
			C3D_PluginException( castor::string::stringCast< char >( strError ), true );
		}

		if ( !m_library->getFunction( m_pfnOnUnload, plugin::GetOnUnloadFunctionABIName ) )
		{
			castor::String strError = cuT( "Error encountered while loading dll [" ) + m_library->getPath().getFileName() + cuT( "] plug-in OnUnload function : " );
			strError += castor::system::getLastErrorText();
			C3D_PluginException( castor::string::stringCast< char >( strError ), true );
		}
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
			if ( m_library )
			{
				castor::Debug::loadModule( *m_library );
			}
#endif

			m_pfnOnLoad( getEngine(), this );
		}
	}

	void Plugin::unload()const noexcept
	{
		if ( m_pfnOnUnload )
		{
			m_pfnOnUnload( getEngine() );

#if !defined( NDEBUG )
			if ( m_library )
			{
				castor::Debug::unloadModule( *m_library );
			}
#endif
		}
	}
}
