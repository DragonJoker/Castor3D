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
						, Producer && p_produce
						, Initialiser && p_initialise
						, Cleaner && p_clean
						, Merger && p_merge )
		: MyCacheType( engine
					   , std::move( p_produce )
					   , std::move( p_initialise )
					   , std::move( p_clean )
					   , std::move( p_merge ) )
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

	PluginSPtr PluginCache::loadPlugin( String const & p_pluginName, Path const & p_pathFolder )throw( )
	{
		Path strFilePath{ CASTOR_DLL_PREFIX + p_pluginName + cuT( "." ) + CASTOR_DLL_EXT };
		PluginSPtr result;

		try
		{
			result = doloadPlugin( strFilePath );
		}
		catch ( VersionException & p_exc )
		{
			Logger::logWarning( "loadPlugin - Fail - " + p_exc.getFullDescription() );
		}
		catch ( PluginException & p_exc )
		{
			if ( !p_pathFolder.empty() )
			{
				result = loadPlugin( p_pathFolder / strFilePath );
			}
			else
			{
				Logger::logWarning( "loadPlugin - Fail - " + p_exc.getFullDescription() );
			}
		}
		catch ( std::exception & p_exc )
		{
			Logger::logWarning( cuT( "loadPlugin - Fail - " ) + string::stringCast< xchar >( p_exc.what() ) );
		}
		catch ( ... )
		{
			Logger::logWarning( cuT( "loadPlugin - Fail - Unknown error" ) );
		}

		return result;
	}

	PluginSPtr PluginCache::loadPlugin( Path const & p_fileFullPath )throw( )
	{
		PluginSPtr result;

		try
		{
			result = doloadPlugin( p_fileFullPath );
		}
		catch ( VersionException & p_exc )
		{
			Logger::logWarning( "loadPlugin - Fail - " + p_exc.getFullDescription() );
		}
		catch ( PluginException & p_exc )
		{
			Logger::logWarning( "loadPlugin - Fail - " + p_exc.getFullDescription() );
		}
		catch ( std::exception & p_exc )
		{
			Logger::logWarning( cuT( "loadPlugin - Fail - " ) + string::stringCast< xchar >( p_exc.what() ) );
		}
		catch ( ... )
		{
			Logger::logWarning( cuT( "loadPlugin - Fail - Unknown error" ) );
		}

		return result;
	}

	PluginStrMap PluginCache::getPlugins( PluginType p_type )
	{
		auto lock = makeUniqueLock( m_mutexLoadedPlugins );
		return m_loadedPlugins[size_t( p_type )];
	}

	void PluginCache::loadAllPlugins( Path const & p_folder )
	{
		PathArray files;
		File::listDirectoryFiles( p_folder, files );

		if ( files.size() > 0 )
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
						Logger::logInfo( cuT( "Can't load plug-in : " ) + file );
					}
				}
			}
		}
	}

	PluginSPtr PluginCache::doloadPlugin( Path const & p_pathFile )
	{
		PluginSPtr result;
		auto lockTypes = makeUniqueLock( m_mutexLoadedPluginTypes );
		auto it = m_loadedPluginTypes.find( p_pathFile );

		if ( it == m_loadedPluginTypes.end() )
		{
			if ( !File::fileExists( p_pathFile ) )
			{
				CASTOR_EXCEPTION( string::stringCast< char >( cuT( "File [" ) + p_pathFile + cuT( "] does not exist" ) ) );
			}

			DynamicLibrarySPtr library = std::make_shared< DynamicLibrary >();

			if ( !library->open( p_pathFile ) )
			{
				CASTOR_PLUGIN_EXCEPTION( string::stringCast< char >( cuT( "Error encountered while loading file [" ) + p_pathFile + cuT( "]" ) ), true );
			}

			Plugin::PGetTypeFunction pfnGetType;

			if ( !library->getFunction( pfnGetType, getTypeFunctionABIName ) )
			{
				String strError = cuT( "Error encountered while loading file [" ) + p_pathFile.getFileName( true ) + cuT( "] getType plug-in function => Not a Castor3D plug-in" );
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
					String strError = cuT( "Error encountered while loading plug-in [" ) + p_pathFile.getFileName() + cuT( "] Unknown plug-in type" );
					CASTOR_PLUGIN_EXCEPTION( string::stringCast< char >( strError ), true );
				}
				break;
			}

			Version toCheck( 0, 0 );
			result->getRequiredVersion( toCheck );
			String strToLog( cuT( "Plug-in [" ) );
			Logger::logInfo( StringStream() << strToLog << result->getName() << cuT( "] - Required engine version : " ) << toCheck );
			Version version = getEngine()->getVersion();

			if ( toCheck <= version )
			{
				m_loadedPluginTypes.insert( std::make_pair( p_pathFile, type ) );
				{
					auto lockPlugins = makeUniqueLock( m_mutexLoadedPlugins );
					m_loadedPlugins[size_t( type )].insert( std::make_pair( p_pathFile, result ) );
				}
				{
					auto lockLibraries = makeUniqueLock( m_mutexLibraries );
					m_libraries[size_t( type )].insert( std::make_pair( p_pathFile, library ) );
				}
				strToLog = cuT( "Plug-in [" );
				Logger::logInfo( strToLog + result->getName() + cuT( "] loaded" ) );
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
			result = m_loadedPlugins[size_t( type )].find( p_pathFile )->second;
		}

		return result;
	}
}
