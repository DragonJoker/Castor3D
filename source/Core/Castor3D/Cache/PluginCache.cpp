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

CU_ImplementSmartPtr( castor3d, PluginCache )

namespace castor3d
{
	const castor::String PtrCacheTraitsT< castor3d::Plugin, castor::String >::Name = cuT( "Plugin" );
}

namespace castor
{
	using namespace castor3d;

	namespace cacheplgn
	{
		static const String getTypeFunctionABIName = cuT( "getType" );
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

	castor::StringMap< PluginRPtr > ResourceCacheT< Plugin, String, PluginCacheTraits >::getPlugins( PluginType type )
	{
		auto lock( makeUniqueLock( m_mutexLoadedPlugins ) );
		castor::StringMap< PluginRPtr > result;

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
		auto lockTypes( makeUniqueLock( m_mutexLoadedPluginTypes ) );

		if ( auto it = m_loadedPluginTypes.find( pathFile );
			it == m_loadedPluginTypes.end() )
		{
			if ( !File::fileExists( pathFile ) )
			{
				CU_Exception( cuT( "File [" ) + pathFile + cuT( "] does not exist" ) );
			}

			DynamicLibraryUPtr library{ castor::makeUnique< DynamicLibrary >( pathFile ) };
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
				plugin = castor::makeUniqueDerived< Plugin, DividerPlugin >( castor::move( library ), &m_engine );
				break;

			case PluginType::eImporter:
				plugin = castor::makeUniqueDerived< Plugin, ImporterPlugin >( castor::move( library ), &m_engine );
				break;

			case PluginType::eGeneric:
				plugin = castor::makeUniqueDerived< Plugin, GenericPlugin >( castor::move( library ), &m_engine );
				break;

			case PluginType::eToneMapping:
				plugin = castor::makeUniqueDerived< Plugin, ToneMappingPlugin >( castor::move( library ), &m_engine );
				break;

			case PluginType::ePostEffect:
				plugin = castor::makeUniqueDerived< Plugin, PostFxPlugin >( castor::move( library ), &m_engine );
				break;

			case PluginType::eParticle:
				plugin = castor::makeUniqueDerived< Plugin, ParticlePlugin >( castor::move( library ), &m_engine );
				break;

			case PluginType::eGenerator:
				plugin = castor::makeUniqueDerived< Plugin, GeneratorPlugin >( castor::move( library ), &m_engine );
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
					result = m_loadedPlugins[size_t( type )].try_emplace( pathFile, castor::move( plugin ) ).first->second.get();
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
