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

using namespace Castor;

namespace Castor3D
{
	template<> const String CacheTraits< Plugin, String >::Name = cuT( "Plugin" );
	static const String GetTypeFunctionABIName = cuT( "GetType" );

	PluginCache::Cache( Engine & p_engine
						, Producer && p_produce
						, Initialiser && p_initialise
						, Cleaner && p_clean
						, Merger && p_merge )
		: MyCacheType( p_engine
					   , std::move( p_produce )
					   , std::move( p_initialise )
					   , std::move( p_clean )
					   , std::move( p_merge ) )
	{
	}

	PluginCache::~Cache()
	{
	}

	void PluginCache::Clear()
	{
		{
			auto lock = make_unique_lock( m_mutexLoadedPluginTypes );
			m_loadedPluginTypes.clear();
		}

		{
			auto lock = make_unique_lock( m_mutexLoadedPlugins );

			for ( auto & it : m_loadedPlugins )
			{
				it.clear();
			}
		}

		{
			auto lock = make_unique_lock( m_mutexLibraries );

			for ( auto & it : m_libraries )
			{
				it.clear();
			}
		}
	}

	PluginSPtr PluginCache::LoadPlugin( String const & p_pluginName, Path const & p_pathFolder )throw( )
	{
		Path strFilePath{ CASTOR_DLL_PREFIX + p_pluginName + cuT( "." ) + CASTOR_DLL_EXT };
		PluginSPtr result;

		try
		{
			result = DoLoadPlugin( strFilePath );
		}
		catch ( VersionException & p_exc )
		{
			Logger::LogWarning( "LoadPlugin - Fail - " + p_exc.GetFullDescription() );
		}
		catch ( PluginException & p_exc )
		{
			if ( !p_pathFolder.empty() )
			{
				result = LoadPlugin( p_pathFolder / strFilePath );
			}
			else
			{
				Logger::LogWarning( "LoadPlugin - Fail - " + p_exc.GetFullDescription() );
			}
		}
		catch ( std::exception & p_exc )
		{
			Logger::LogWarning( cuT( "LoadPlugin - Fail - " ) + string::string_cast< xchar >( p_exc.what() ) );
		}
		catch ( ... )
		{
			Logger::LogWarning( cuT( "LoadPlugin - Fail - Unknown error" ) );
		}

		return result;
	}

	PluginSPtr PluginCache::LoadPlugin( Path const & p_fileFullPath )throw( )
	{
		PluginSPtr result;

		try
		{
			result = DoLoadPlugin( p_fileFullPath );
		}
		catch ( VersionException & p_exc )
		{
			Logger::LogWarning( "LoadPlugin - Fail - " + p_exc.GetFullDescription() );
		}
		catch ( PluginException & p_exc )
		{
			Logger::LogWarning( "LoadPlugin - Fail - " + p_exc.GetFullDescription() );
		}
		catch ( std::exception & p_exc )
		{
			Logger::LogWarning( cuT( "LoadPlugin - Fail - " ) + string::string_cast< xchar >( p_exc.what() ) );
		}
		catch ( ... )
		{
			Logger::LogWarning( cuT( "LoadPlugin - Fail - Unknown error" ) );
		}

		return result;
	}

	PluginStrMap PluginCache::GetPlugins( PluginType p_type )
	{
		auto lock = make_unique_lock( m_mutexLoadedPlugins );
		return m_loadedPlugins[size_t( p_type )];
	}

	void PluginCache::LoadAllPlugins( Path const & p_folder )
	{
		PathArray files;
		File::ListDirectoryFiles( p_folder, files );

		if ( files.size() > 0 )
		{
			for ( auto file : files )
			{
				if ( file.GetExtension() == CASTOR_DLL_EXT )
				{
					try
					{
						DoLoadPlugin( file );
					}
					catch ( ... )
					{
						Logger::LogInfo( cuT( "Can't load plug-in : " ) + file );
					}
				}
			}
		}
	}

	PluginSPtr PluginCache::DoLoadPlugin( Path const & p_pathFile )
	{
		PluginSPtr result;
		auto lockTypes = make_unique_lock( m_mutexLoadedPluginTypes );
		auto it = m_loadedPluginTypes.find( p_pathFile );

		if ( it == m_loadedPluginTypes.end() )
		{
			if ( !File::FileExists( p_pathFile ) )
			{
				CASTOR_EXCEPTION( string::string_cast< char >( cuT( "File [" ) + p_pathFile + cuT( "] does not exist" ) ) );
			}

			DynamicLibrarySPtr library = std::make_shared< DynamicLibrary >();

			if ( !library->Open( p_pathFile ) )
			{
				CASTOR_PLUGIN_EXCEPTION( string::string_cast< char >( cuT( "Error encountered while loading file [" ) + p_pathFile + cuT( "]" ) ), true );
			}

			Plugin::PGetTypeFunction pfnGetType;

			if ( !library->GetFunction( pfnGetType, GetTypeFunctionABIName ) )
			{
				String strError = cuT( "Error encountered while loading file [" ) + p_pathFile.GetFileName( true ) + cuT( "] GetType plug-in function => Not a Castor3D plug-in" );
				CASTOR_PLUGIN_EXCEPTION( string::string_cast< char >( strError ), true );
			}

			PluginType type = PluginType::eCount;
			pfnGetType( &type );

			switch ( type )
			{
			case PluginType::eDivider:
				result = std::make_shared< DividerPlugin >( library, GetEngine() );
				break;

			case PluginType::eImporter:
				result = std::make_shared< ImporterPlugin >( library, GetEngine() );
				break;

			case PluginType::eRenderer:
				result = std::make_shared< RendererPlugin >( library, GetEngine() );
				break;

			case PluginType::eGeneric:
				result = std::make_shared< GenericPlugin >( library, GetEngine() );
				break;

			case PluginType::eTechnique:
				result = std::make_shared< TechniquePlugin >( library, GetEngine() );
				break;

			case PluginType::eToneMapping:
				result = std::make_shared< ToneMappingPlugin >( library, GetEngine() );
				break;

			case PluginType::ePostEffect:
				result = std::make_shared< PostFxPlugin >( library, GetEngine() );
				break;

			case PluginType::eParticle:
				result = std::make_shared< ParticlePlugin >( library, GetEngine() );
				break;

			case PluginType::eGenerator:
				result = std::make_shared< GeneratorPlugin >( library, GetEngine() );
				break;

			default:
				FAILURE( "Unknown plug-in type" );
				{
					String strError = cuT( "Error encountered while loading plug-in [" ) + p_pathFile.GetFileName() + cuT( "] Unknown plug-in type" );
					CASTOR_PLUGIN_EXCEPTION( string::string_cast< char >( strError ), true );
				}
				break;
			}

			Version toCheck( 0, 0 );
			result->GetRequiredVersion( toCheck );
			String strToLog( cuT( "Plug-in [" ) );
			Logger::LogInfo( StringStream() << strToLog << result->GetName() << cuT( "] - Required engine version : " ) << toCheck );
			Version version = GetEngine()->GetVersion();

			if ( toCheck <= version )
			{
				m_loadedPluginTypes.insert( std::make_pair( p_pathFile, type ) );
				{
					auto lockPlugins = make_unique_lock( m_mutexLoadedPlugins );
					m_loadedPlugins[size_t( type )].insert( std::make_pair( p_pathFile, result ) );
				}
				{
					auto lockLibraries = make_unique_lock( m_mutexLibraries );
					m_libraries[size_t( type )].insert( std::make_pair( p_pathFile, library ) );
				}
				strToLog = cuT( "Plug-in [" );
				Logger::LogInfo( strToLog + result->GetName() + cuT( "] loaded" ) );
			}
			else
			{
				CASTOR_VERSION_EXCEPTION( toCheck, version );
			}
		}
		else
		{
			PluginType type = it->second;
			auto lock = make_unique_lock( m_mutexLoadedPlugins );
			result = m_loadedPlugins[size_t( type )].find( p_pathFile )->second;
		}

		return result;
	}
}
