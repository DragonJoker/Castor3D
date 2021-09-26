#include "Castor3D/Cache/PluginCache.hpp"

#include "Castor3D/Engine.hpp"

#include "Castor3D/Plugin/DividerPlugin.hpp"
#include "Castor3D/Plugin/GeneratorPlugin.hpp"
#include "Castor3D/Plugin/GenericPlugin.hpp"
#include "Castor3D/Plugin/ImporterPlugin.hpp"
#include "Castor3D/Plugin/ParticlePlugin.hpp"
#include "Castor3D/Plugin/PluginException.hpp"
#include "Castor3D/Plugin/PostFxPlugin.hpp"
#include "Castor3D/Plugin/ToneMappingPlugin.hpp"
#include "Castor3D/Miscellaneous/VersionException.hpp"

#include <CastorUtils/Miscellaneous/DynamicLibrary.hpp>

CU_ImplementCUSmartPtr( castor3d, PluginCache )

namespace castor3d
{
	const castor::String PtrCacheTraitsT< castor3d::Plugin, castor::String >::Name = cuT( "Plugin" );
}

namespace castor
{
	using namespace castor3d;

	static const String getTypeFunctionABIName = cuT( "getType" );

	ResourceCacheT< Plugin, String, PluginCacheTraits >::ResourceCacheT( Engine & engine )
		: ElementCacheT{ engine.getLogger() }
		, m_engine{ engine }
	{
	}

	void ResourceCacheT< Plugin, String, PluginCacheTraits >::clear()
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

		{
			auto lock( makeUniqueLock( m_mutexLibraries ) );

			for ( auto & it : m_libraries )
			{
				it.clear();
			}
		}
	}

	PluginSPtr ResourceCacheT< Plugin, String, PluginCacheTraits >::loadPlugin( String const & pluginName, Path const & pathFolder )noexcept
	{
		Path strFilePath{ CU_SharedLibPrefix + pluginName + cuT( "." ) + CU_SharedLibExt };
		PluginSPtr result;

		try
		{
			result = doloadPlugin( strFilePath );
		}
		catch ( VersionException & exc )
		{
			log::warn << "loadPlugin - Fail - " << exc.getFullDescription() << std::endl;
		}
		catch ( PluginException & exc )
		{
			if ( !pathFolder.empty() )
			{
				result = loadPlugin( pathFolder / strFilePath );
			}
			else
			{
				log::warn << "loadPlugin - Fail - " + exc.getFullDescription() << std::endl;
			}
		}
		catch ( std::exception & exc )
		{
			log::warn << cuT( "loadPlugin - Fail - " ) + string::stringCast< xchar >( exc.what() ) << std::endl;
		}
		catch ( ... )
		{
			log::warn << cuT( "loadPlugin - Fail - Unknown error" ) << std::endl;
		}

		return result;
	}

	PluginSPtr ResourceCacheT< Plugin, String, PluginCacheTraits >::loadPlugin( Path const & fileFullPath )noexcept
	{
		PluginSPtr result;

		try
		{
			result = doloadPlugin( fileFullPath );
		}
		catch ( VersionException & exc )
		{
			log::warn << "loadPlugin - Fail - " + exc.getFullDescription() << std::endl;
		}
		catch ( PluginException & exc )
		{
			log::warn << "loadPlugin - Fail - " + exc.getFullDescription() << std::endl;
		}
		catch ( std::exception & exc )
		{
			log::warn << cuT( "loadPlugin - Fail - " ) + string::stringCast< xchar >( exc.what() ) << std::endl;
		}
		catch ( ... )
		{
			log::warn << cuT( "loadPlugin - Fail - Unknown error" ) << std::endl;
		}

		return result;
	}

	PluginStrMap ResourceCacheT< Plugin, String, PluginCacheTraits >::getPlugins( PluginType type )
	{
		auto lock( makeUniqueLock( m_mutexLoadedPlugins ) );
		return m_loadedPlugins[size_t( type )];
	}

	void ResourceCacheT< Plugin, String, PluginCacheTraits >::loadAllPlugins( Path const & folder )
	{
		PathArray files;
		File::listDirectoryFiles( folder, files );

		if ( !files.empty() )
		{
			for ( auto file : files )
			{
				if ( file.getExtension() == CU_SharedLibExt )
				{
					try
					{
						doloadPlugin( file );
					}
					catch ( ... )
					{
						log::warn << cuT( "Can't load plug-in : " ) + file << std::endl;
					}
				}
			}
		}
	}

	PluginSPtr ResourceCacheT< Plugin, String, PluginCacheTraits >::doloadPlugin( Path const & pathFile )
	{
		PluginSPtr result;
		auto lockTypes( makeUniqueLock( m_mutexLoadedPluginTypes ) );
		auto it = m_loadedPluginTypes.find( pathFile );

		if ( it == m_loadedPluginTypes.end() )
		{
			if ( !File::fileExists( pathFile ) )
			{
				CU_Exception( string::stringCast< char >( cuT( "File [" ) + pathFile + cuT( "] does not exist" ) ) );
			}

			DynamicLibrarySPtr library{ std::make_shared< DynamicLibrary >( pathFile ) };
			Plugin::PGetTypeFunction pfnGetType;

			if ( !library->getFunction( pfnGetType, getTypeFunctionABIName ) )
			{
				String strError = cuT( "Error encountered while loading file [" ) + pathFile.getFileName( true ) + cuT( "] getType plug-in function => Not a Castor3D plug-in" );
				CASTOR_PLUGIN_EXCEPTION( string::stringCast< char >( strError ), true );
			}

			PluginType type{ PluginType::eCount };
			pfnGetType( &type );

			switch ( type )
			{
			case PluginType::eDivider:
				result = std::make_shared< DividerPlugin >( library, &m_engine );
				break;

			case PluginType::eImporter:
				result = std::make_shared< ImporterPlugin >( library, &m_engine );
				break;

			case PluginType::eGeneric:
				result = std::make_shared< GenericPlugin >( library, &m_engine );
				break;

			case PluginType::eToneMapping:
				result = std::make_shared< ToneMappingPlugin >( library, &m_engine );
				break;

			case PluginType::ePostEffect:
				result = std::make_shared< PostFxPlugin >( library, &m_engine );
				break;

			case PluginType::eParticle:
				result = std::make_shared< ParticlePlugin >( library, &m_engine );
				break;

			case PluginType::eGenerator:
				result = std::make_shared< GeneratorPlugin >( library, &m_engine );
				break;

			default:
				CU_Failure( "Unknown plug-in type" );
				{
					String strError = cuT( "Error encountered while loading plug-in [" ) + pathFile.getFileName() + cuT( "] Unknown plug-in type" );
					CASTOR_PLUGIN_EXCEPTION( string::stringCast< char >( strError ), true );
				}
			}

			Version toCheck( 0, 0 );
			result->getRequiredVersion( toCheck );
			Version version = m_engine.getVersion();

			if ( toCheck <= version )
			{
				m_loadedPluginTypes.insert( std::make_pair( pathFile, type ) );
				{
					auto lockPlugins( makeUniqueLock( m_mutexLoadedPlugins ) );
					m_loadedPlugins[size_t( type )].insert( std::make_pair( pathFile, result ) );
				}
				{
					auto lockLibraries( makeUniqueLock( m_mutexLibraries ) );
					m_libraries[size_t( type )].insert( std::make_pair( pathFile, library ) );
				}
				log::info << cuT( "Plug-in [" ) << result->getName() << cuT( "] - Required engine version : " ) << toCheck << cuT( ", loaded" ) << std::endl;
			}
			else
			{
				CASTOR_VERSION_EXCEPTION( toCheck, version );
			}
		}
		else
		{
			PluginType type = it->second;
			auto lockPlugins( makeUniqueLock( m_mutexLoadedPlugins ) );
			result = m_loadedPlugins[size_t( type )].find( pathFile )->second;
		}

		return result;
	}
}
