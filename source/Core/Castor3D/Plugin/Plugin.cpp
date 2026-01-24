#include "Castor3D/Plugin/Plugin.hpp"

#include "Castor3D/Miscellaneous/Version.hpp"
#include "Castor3D/Plugin/PluginException.hpp"

#include <CastorUtils/Miscellaneous/DynamicLibrary.hpp>
#include <CastorUtils/Miscellaneous/Utils.hpp>

CU_ImplementSmartPtr( c3d, Plugin )

namespace c3d
{
	namespace plugin
	{
		static const String GetNameFunctionABIName = cuT( "getName" );
		static const String GetRequiredVersionFunctionABIName = cuT( "getRequiredVersion" );
		static const String GetOnLoadFunctionABIName = cuT( "onLoad" );
		static const String GetOnUnloadFunctionABIName = cuT( "onUnload" );
	}

	Plugin::Plugin( PluginType type, DynamicLibraryUPtr library, Engine & engine )
		: OwnedBy< Engine >( engine )
		, m_library{ c3d::move( library ) }
		, m_type{ type }
	{
		if ( !m_library->getFunction( m_pfnGetName, plugin::GetNameFunctionABIName ) )
		{
			String strError = cuT( "Error encountered while loading dll [" ) + m_library->getPath().getFileName() + cuT( "] plug-in getName function : " );
			strError += system::getLastErrorText();
			C3D_PluginException( toUtf8( strError ), true );
		}

		if ( !m_library->getFunction( m_pfnGetRequiredVersion, plugin::GetRequiredVersionFunctionABIName ) )
		{
			String strError = cuT( "Error encountered while loading dll [" ) + m_library->getPath().getFileName() + cuT( "] plug-in getRequiredVersion function : " );
			strError += system::getLastErrorText();
			C3D_PluginException( toUtf8( strError ), true );
		}

		if ( !m_library->getFunction( m_pfnOnLoad, plugin::GetOnLoadFunctionABIName ) )
		{
			String strError = cuT( "Error encountered while loading dll [" ) + m_library->getPath().getFileName() + cuT( "] plug-in onLoad function : " );
			strError += system::getLastErrorText();
			C3D_PluginException( toUtf8( strError ), true );
		}

		if ( !m_library->getFunction( m_pfnOnUnload, plugin::GetOnUnloadFunctionABIName ) )
		{
			String strError = cuT( "Error encountered while loading dll [" ) + m_library->getPath().getFileName() + cuT( "] plug-in onUnload function : " );
			strError += system::getLastErrorText();
			C3D_PluginException( toUtf8( strError ), true );
		}

		load();
	}

	Plugin::~Plugin()noexcept
	{
		unload();
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
			strReturn = makeString( name );
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
				debug::loadModule( *m_library );
			}
#endif

			m_pfnOnLoad( getEngine() );
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
				debug::unloadModule( *m_library );
			}
#endif
		}
	}
}
