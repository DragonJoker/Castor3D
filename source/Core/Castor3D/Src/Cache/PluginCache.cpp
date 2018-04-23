#include "PluginCache.hpp"

#include "Engine.hpp"

#include "Plugin/DividerPlugin.hpp"
#include "Plugin/GeneratorPlugin.hpp"
#include "Plugin/GenericPlugin.hpp"
#include "Plugin/ImporterPlugin.hpp"
#include "Plugin/ParticlePlugin.hpp"
#include "Plugin/PostFxPlugin.hpp"
#include "Plugin/RendererPlugin.hpp"
#include "Plugin/TechniquePlugin.hpp"
#include "Plugin/ToneMappingPlugin.hpp"
#include "Miscellaneous/VersionException.hpp"

#include <Miscellaneous/DynamicLibrary.hpp>

using namespace castor;

namespace castor3d
{
	template<> const String CacheTraits< Plugin, String >::Name = cuT( "Plugin" );
	static const String getTypeFunctionABIName = cuT( "getType" );

	PluginCache::Cache( Engine & engine
		, Producer && produce
		, Initialiser && initialise
		, Cleaner && clean
		, Merger && merge )
		: MyCacheType( engine
			, std::move( produce )
			, std::move( initialise )
			, std::move( clean )
			, std::move( merge ) )
	{
	}

	PluginCache::~Cache()
	{
	}

	void PluginCache::clear()
	{
		{
			auto lock = makeUniqueLock( m_mutexLoadedPluginTypes );
			m_loadedPluginTypes.clear();
		}

		{
			auto lock = makeUniqueLock( m_mutexLoadedPlugins );

			for ( auto & it : m_loadedPlugins )
			{
				it.clear();
			}
		}

		{
			auto lock = makeUniqueLock( m_mutexLibraries );

			for ( auto & it : m_libraries )
			{
				it.clear();
			}
		}
	}

	PluginSPtr PluginCache::loadPlugin( String const & pluginName, Path const & pathFolder )throw( )
	{
		Path strFilePath{ CASTOR_DLL_PREFIX + pluginName + cuT( "." ) + CASTOR_DLL_EXT };
		PluginSPtr result;

		try
		{
			result = doloadPlugin( strFilePath );
		}
		catch ( VersionException & exc )
		{
			Logger::logWarning( "loadPlugin - Fail - " + exc.getFullDescription() );
		}
		catch ( PluginException & exc )
		{
			if ( !pathFolder.empty() )
			{
				result = loadPlugin( pathFolder / strFilePath );
			}
			else
			{
				Logger::logWarning( "loadPlugin - Fail - " + exc.getFullDescription() );
			}
		}
		catch ( std::exception & exc )
		{
			Logger::logWarning( cuT( "loadPlugin - Fail - " ) + string::stringCast< xchar >( exc.what() ) );
		}
		catch ( ... )
		{
			Logger::logWarning( cuT( "loadPlugin - Fail - Unknown error" ) );
		}

		return result;
	}

	PluginSPtr PluginCache::loadPlugin( Path const & fileFullPath )throw( )
	{
		PluginSPtr result;

		try
		{
			result = doloadPlugin( fileFullPath );
		}
		catch ( VersionException & exc )
		{
			Logger::logWarning( "loadPlugin - Fail - " + exc.getFullDescription() );
		}
		catch ( PluginException & exc )
		{
			Logger::logWarning( "loadPlugin - Fail - " + exc.getFullDescription() );
		}
		catch ( std::exception & exc )
		{
			Logger::logWarning( cuT( "loadPlugin - Fail - " ) + string::stringCast< xchar >( exc.what() ) );
		}
		catch ( ... )
		{
			Logger::logWarning( cuT( "loadPlugin - Fail - Unknown error" ) );
		}

		return result;
	}

	PluginStrMap PluginCache::getPlugins( PluginType type )
	{
		auto lock = makeUniqueLock( m_mutexLoadedPlugins );
		return m_loadedPlugins[size_t( type )];
	}

	void PluginCache::loadAllPlugins( Path const & folder )
	{
		PathArray files;
		File::listDirectoryFiles( folder, files );

		if ( !files.empty() )
		{
			for ( auto file : files )
			{
				if ( file.getExtension() == CASTOR_DLL_EXT )
				{
					try
					{
						doloadPlugin( file );
					}
					catch ( ... )
					{
						Logger::logWarning( cuT( "Can't load plug-in : " ) + file );
					}
				}
			}
		}
	}

	PluginSPtr PluginCache::doloadPlugin( Path const & pathFile )
	{
		PluginSPtr result;
		auto lockTypes = makeUniqueLock( m_mutexLoadedPluginTypes );
		auto it = m_loadedPluginTypes.find( pathFile );

		if ( it == m_loadedPluginTypes.end() )
		{
			if ( !File::fileExists( pathFile ) )
			{
				CASTOR_EXCEPTION( string::stringCast< char >( cuT( "File [" ) + pathFile + cuT( "] does not exist" ) ) );
			}

			DynamicLibrarySPtr library = std::make_shared< DynamicLibrary >( pathFile );
			Plugin::PGetTypeFunction pfnGetType;

			if ( !library->getFunction( pfnGetType, getTypeFunctionABIName ) )
			{
				String strError = cuT( "Error encountered while loading file [" ) + pathFile.getFileName( true ) + cuT( "] getType plug-in function => Not a Castor3D plug-in" );
				CASTOR_PLUGIN_EXCEPTION( string::stringCast< char >( strError ), true );
			}

			PluginType type = PluginType::eCount;
			pfnGetType( &type );

			switch ( type )
			{
			case PluginType::eDivider:
				result = std::make_shared< DividerPlugin >( library, getEngine() );
				break;

			case PluginType::eImporter:
				result = std::make_shared< ImporterPlugin >( library, getEngine() );
				break;

			case PluginType::eRenderer:
				result = std::make_shared< RendererPlugin >( library, getEngine() );
				break;

			case PluginType::eGeneric:
				result = std::make_shared< GenericPlugin >( library, getEngine() );
				break;

			case PluginType::eTechnique:
				result = std::make_shared< TechniquePlugin >( library, getEngine() );
				break;

			case PluginType::eToneMapping:
				result = std::make_shared< ToneMappingPlugin >( library, getEngine() );
				break;

			case PluginType::ePostEffect:
				result = std::make_shared< PostFxPlugin >( library, getEngine() );
				break;

			case PluginType::eParticle:
				result = std::make_shared< ParticlePlugin >( library, getEngine() );
				break;

			case PluginType::eGenerator:
				result = std::make_shared< GeneratorPlugin >( library, getEngine() );
				break;

			default:
				FAILURE( "Unknown plug-in type" );
				{
					String strError = cuT( "Error encountered while loading plug-in [" ) + pathFile.getFileName() + cuT( "] Unknown plug-in type" );
					CASTOR_PLUGIN_EXCEPTION( string::stringCast< char >( strError ), true );
				}
				break;
			}

			Version toCheck( 0, 0 );
			result->getRequiredVersion( toCheck );
			Version version = getEngine()->getVersion();

			if ( toCheck <= version )
			{
				m_loadedPluginTypes.insert( std::make_pair( pathFile, type ) );
				{
					auto lockPlugins = makeUniqueLock( m_mutexLoadedPlugins );
					m_loadedPlugins[size_t( type )].insert( std::make_pair( pathFile, result ) );
				}
				{
					auto lockLibraries = makeUniqueLock( m_mutexLibraries );
					m_libraries[size_t( type )].insert( std::make_pair( pathFile, library ) );
				}
				Logger::logInfo( castor::makeStringStream() << cuT( "Plug-in [" ) << result->getName() << cuT( "] - Required engine version : " ) << toCheck << cuT( ", loaded" ) );
			}
			else
			{
				CASTOR_VERSION_EXCEPTION( toCheck, version );
			}
		}
		else
		{
			PluginType type = it->second;
			auto lock = makeUniqueLock( m_mutexLoadedPlugins );
			result = m_loadedPlugins[size_t( type )].find( pathFile )->second;
		}

		return result;
	}
}
