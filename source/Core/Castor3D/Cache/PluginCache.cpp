#include "Castor3D/Cache/PluginCache.hpp"

#include "Castor3D/Engine.hpp"

#include "Castor3D/Plugin/Plugin.hpp"
#include "Castor3D/Plugin/PluginException.hpp"
#include "Castor3D/Miscellaneous/VersionException.hpp"

#include <CastorUtils/Miscellaneous/DynamicLibrary.hpp>

CU_ImplementSmartPtr( c3d, PluginCache )

namespace c3d
{
	const String PtrCacheTraitsT< Plugin, String >::Name = cuT( "Plugin" );

	namespace cacheplgn
	{
		static const String getTypeFunctionABIName = cuT( "getType" );
		static const String isDebugFunctionABIName = cuT( "isDebug" );
	}

	ResourceCacheT< Plugin, String, PluginCacheTraits >::ResourceCacheT( Engine & engine )
		: ElementCacheT{ engine.getLogger() }
		, m_engine{ engine }
	{
	}

	void ResourceCacheT< Plugin, String, PluginCacheTraits >::clear()noexcept
	{
		{
			auto lock( makeUniqueLock( m_mutexLoadedPluginTypes ) );
			m_loadedPluginTypes.clear();
		}

		{
			auto lock( makeUniqueLock( m_mutexLoadedPlugins ) );

			for ( auto & it : m_loadedPlugins )
			{
				it.clear();
			}
		}
	}

	PluginRPtr ResourceCacheT< Plugin, String, PluginCacheTraits >::loadPlugin( String const & pluginName, Path const & pathFolder )noexcept
	{
		Path strFilePath{ CU_SharedLibPrefix + pluginName + cuT( "." ) + CU_SharedLibExt };
		PluginRPtr result{};

		try
		{
			auto lockTypes( makeUniqueLock( m_mutexLoadedPluginTypes ) );
			result = doloadPlugin( strFilePath );
		}
		catch ( VersionException & exc )
		{
			log::warn << cuT( "loadPlugin - Fail - " ) << makeString( exc.getFullDescription() ) << std::endl;
		}
		catch ( PluginException & exc )
		{
			if ( !pathFolder.empty() )
			{
				result = loadPlugin( pathFolder / strFilePath );
			}
			else
			{
				log::warn << cuT( "loadPlugin - Fail - " ) << makeString( exc.getFullDescription() ) << std::endl;
			}
		}
		catch ( std::exception & exc )
		{
			log::warn << cuT( "loadPlugin - Fail - " ) << makeString( exc.what() ) << std::endl;
		}
		catch ( ... )
		{
			log::warn << cuT( "loadPlugin - Fail - Unknown error" ) << std::endl;
		}

		return result;
	}

	PluginRPtr ResourceCacheT< Plugin, String, PluginCacheTraits >::loadPlugin( Path const & fileFullPath )noexcept
	{
		PluginRPtr result{};

		try
		{
			auto lockTypes( makeUniqueLock( m_mutexLoadedPluginTypes ) );
			result = doloadPlugin( fileFullPath );
		}
		catch ( VersionException & exc )
		{
			log::warn << cuT( "loadPlugin - Fail - " ) << makeString( exc.getFullDescription() ) << std::endl;
		}
		catch ( PluginException & exc )
		{
			log::warn << cuT( "loadPlugin - Fail - " ) << makeString( exc.getFullDescription() ) << std::endl;
		}
		catch ( std::exception & exc )
		{
			log::warn << cuT( "loadPlugin - Fail - " ) << makeString( exc.what() ) << std::endl;
		}
		catch ( ... )
		{
			log::warn << cuT( "loadPlugin - Fail - Unknown error" ) << std::endl;
		}

		return result;
	}

	StringMap< PluginRPtr > ResourceCacheT< Plugin, String, PluginCacheTraits >::getPlugins( PluginType type )
	{
		auto lock( makeUniqueLock( m_mutexLoadedPlugins ) );
		StringMap< PluginRPtr > result;

		for ( auto const & [name, plugin] : m_loadedPlugins[size_t( type )] )
		{
			result.try_emplace( name, plugin.get() );
		}

		return result;
	}

	void ResourceCacheT< Plugin, String, PluginCacheTraits >::loadAllPlugins( Path const & folder )
	{
		PathArray files;
		File::listDirectoryFiles( folder, files );
		auto lockTypes( makeUniqueLock( m_mutexLoadedPluginTypes ) );

		if ( !files.empty() )
		{
			for ( auto const & file : files )
			{
				if ( file.getExtension() == CU_SharedLibExt )
				{
					try
					{
						doloadPlugin( file );
					}
					catch ( ... )
					{
						log::warn << cuT( "Can't load plug-in : " ) << file << std::endl;
					}
				}
			}
		}
	}

	PluginRPtr ResourceCacheT< Plugin, String, PluginCacheTraits >::doloadPlugin( Path const & pathFile )
	{
		PluginRPtr result{};

		if ( auto it = m_loadedPluginTypes.find( pathFile );
			it == m_loadedPluginTypes.end() )
		{
			if ( !File::fileExists( pathFile ) )
			{
				CU_Exception( cuT( "File [" ) + pathFile + cuT( "] does not exist" ) );
			}

			DynamicLibraryUPtr library{ makeUnique< DynamicLibrary >( pathFile ) };
			Plugin::IsDebugFunction pfnIsDebug;

			if ( !library->getFunction( pfnIsDebug, cacheplgn::isDebugFunctionABIName ) )
			{
				String strError = cuT( "Error encountered while loading file [" ) + pathFile.getFileName( true ) + cuT( "] isDebug plug-in function => Not a Castor3D plug-in" );
				C3D_PluginException( strError, true );
			}

			int isDebugPlugin{};
			pfnIsDebug( &isDebugPlugin );

			if ( int isDebug = system::isDebug() ? 1 : 0;
				isDebug != isDebugPlugin )
			{
				return nullptr;
			}

			Plugin::GetTypeFunction pfnGetType;

			if ( !library->getFunction( pfnGetType, cacheplgn::getTypeFunctionABIName ) )
			{
				String strError = cuT( "Error encountered while loading file [" ) + pathFile.getFileName( true ) + cuT( "] getType plug-in function => Not a Castor3D plug-in" );
				C3D_PluginException( strError, true );
			}

			PluginType type{ PluginType::eCount };
			pfnGetType( &type );
			PluginUPtr plugin;

			switch ( type )
			{
			case PluginType::eDivider:
			case PluginType::eImporter:
			case PluginType::eGeneric:
			case PluginType::eToneMapping:
			case PluginType::ePostEffect:
			case PluginType::eParticle:
			case PluginType::eGenerator:
				plugin = makeUnique< Plugin >( type, c3d::move( library ), m_engine );
				break;

			default:
				CU_Failure( "Unknown plug-in type" );
				{
					String strError = cuT( "Error encountered while loading plug-in [" ) + pathFile.getFileName() + cuT( "] Unknown plug-in type" );
					C3D_PluginException( strError, true );
				}
			}

			Version toCheck( 0, 0 );
			plugin->getRequiredVersion( toCheck );
			Version version = m_engine.getVersion();

			if ( toCheck <= version )
			{
				m_loadedPluginTypes.try_emplace( pathFile, type );
				{
					auto lockPlugins( makeUniqueLock( m_mutexLoadedPlugins ) );
					result = m_loadedPlugins[size_t( type )].try_emplace( pathFile, c3d::move( plugin ) ).first->second.get();
				}
				log::info << cuT( "Plug-in [" ) << result->getName() << cuT( "] - Required engine version : " ) << toCheck << cuT( ", loaded" ) << std::endl;
			}
			else
			{
				C3D_VersionException( toCheck, version );
			}
		}
		else
		{
			PluginType type = it->second;
			auto lockPlugins( makeUniqueLock( m_mutexLoadedPlugins ) );
			result = m_loadedPlugins[size_t( type )].find( pathFile )->second.get();
		}

		return result;
	}
}
