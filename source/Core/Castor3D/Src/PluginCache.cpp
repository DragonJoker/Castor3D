#include "PluginCache.hpp"

#include "Engine.hpp"

#include "Plugin/DividerPlugin.hpp"
#include "Plugin/GenericPlugin.hpp"
#include "Plugin/ImporterPlugin.hpp"
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

#if defined( _MSC_VER)
	static const String GetTypeFunctionABIName = cuT( "?GetType@@YA?AW4ePLUGIN_TYPE@Castor3D@@XZ" );
#elif defined( __GNUG__)
	static const String GetTypeFunctionABIName = cuT( "_Z7GetTypev" );
#endif
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
			auto l_lock = make_unique_lock( m_mutexLoadedPluginTypes );
			m_loadedPluginTypes.clear();
		}

		{
			auto l_lock = make_unique_lock( m_mutexLoadedPlugins );

			for ( auto & l_it : m_loadedPlugins )
			{
				l_it.clear();
			}
		}

		{
			auto l_lock = make_unique_lock( m_mutexLibraries );

			for ( auto & l_it : m_libraries )
			{
				l_it.clear();
			}
		}
	}

	PluginSPtr PluginCache::LoadPlugin( String const & p_pluginName, Path const & p_pathFolder )throw( )
	{
		Path l_strFilePath{ CASTOR_DLL_PREFIX + p_pluginName + cuT( "." ) + CASTOR_DLL_EXT };
		PluginSPtr l_return;

		try
		{
			l_return = DoLoadPlugin( l_strFilePath );
		}
		catch ( VersionException & p_exc )
		{
			Logger::LogWarning( "LoadPlugin - Fail - " + p_exc.GetFullDescription() );
		}
		catch ( PluginException & p_exc )
		{
			if ( !p_pathFolder.empty() )
			{
				l_return = LoadPlugin( p_pathFolder / l_strFilePath );
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

		return l_return;
	}

	PluginSPtr PluginCache::LoadPlugin( Path const & p_fileFullPath )throw( )
	{
		PluginSPtr l_return;

		try
		{
			l_return = DoLoadPlugin( p_fileFullPath );
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

		return l_return;
	}

	PluginStrMap PluginCache::GetPlugins( ePLUGIN_TYPE p_type )
	{
		auto l_lock = make_unique_lock( m_mutexLoadedPlugins );
		return m_loadedPlugins[p_type];
	}

	void PluginCache::LoadAllPlugins( Path const & p_folder )
	{
		PathArray l_files;
		File::ListDirectoryFiles( p_folder, l_files );

		if ( l_files.size() > 0 )
		{
			for ( auto l_file : l_files )
			{
				if ( l_file.GetExtension() == CASTOR_DLL_EXT )
				{
					try
					{
						DoLoadPlugin( l_file );
					}
					catch ( ... )
					{
						Logger::LogInfo( cuT( "Can't load plug-in : " ) + l_file );
					}
				}
			}
		}
	}

	PluginSPtr PluginCache::DoLoadPlugin( Path const & p_pathFile )
	{
		PluginSPtr l_return;
		auto l_lockTypes = make_unique_lock( m_mutexLoadedPluginTypes );
		auto l_it = m_loadedPluginTypes.find( p_pathFile );

		if ( l_it == m_loadedPluginTypes.end() )
		{
			if ( !File::FileExists( p_pathFile ) )
			{
				CASTOR_EXCEPTION( string::string_cast< char >( cuT( "File [" ) + p_pathFile + cuT( "] does not exist" ) ) );
			}

			DynamicLibrarySPtr l_library = std::make_shared< DynamicLibrary >();

			if ( !l_library->Open( p_pathFile ) )
			{
				CASTOR_PLUGIN_EXCEPTION( string::string_cast< char >( cuT( "Error encountered while loading file [" ) + p_pathFile + cuT( "]" ) ), true );
			}

			Plugin::PGetTypeFunction l_pfnGetType;

			if ( !l_library->GetFunction( l_pfnGetType, GetTypeFunctionABIName ) )
			{
				String l_strError = cuT( "Error encountered while loading file [" ) + p_pathFile.GetFileName( true ) + cuT( "] GetType plug-in function => Not a Castor3D plug-in" );
				CASTOR_PLUGIN_EXCEPTION( string::string_cast< char >( l_strError ), true );
			}

			ePLUGIN_TYPE l_type = l_pfnGetType();

			switch ( l_type )
			{
			case ePLUGIN_TYPE_DIVIDER:
				l_return = std::make_shared< DividerPlugin >( l_library, GetEngine() );
				break;

			case ePLUGIN_TYPE_IMPORTER:
				l_return = std::make_shared< ImporterPlugin >( l_library, GetEngine() );
				break;

			case ePLUGIN_TYPE_RENDERER:
				l_return = std::make_shared< RendererPlugin >( l_library, GetEngine() );
				break;

			case ePLUGIN_TYPE_GENERIC:
				l_return = std::make_shared< GenericPlugin >( l_library, GetEngine() );
				break;

			case ePLUGIN_TYPE_TECHNIQUE:
				l_return = std::make_shared< TechniquePlugin >( l_library, GetEngine() );
				break;

			case ePLUGIN_TYPE_TONEMAPPING:
				l_return = std::make_shared< ToneMappingPlugin >( l_library, GetEngine() );
				break;

			case ePLUGIN_TYPE_POSTFX:
				l_return = std::make_shared< PostFxPlugin >( l_library, GetEngine() );
				break;

			default:
				FAILURE( "Unknown plug-in type" );
				{
					String l_strError = cuT( "Error encountered while loading plug-in [" ) + p_pathFile.GetFileName() + cuT( "] Unknown plug-in type" );
					CASTOR_PLUGIN_EXCEPTION( string::string_cast< char >( l_strError ), true );
				}
				break;
			}

			Version l_toCheck( 0, 0 );
			l_return->GetRequiredVersion( l_toCheck );
			String l_strToLog( cuT( "Plug-in [" ) );
			Logger::LogInfo( StringStream() << l_strToLog << l_return->GetName() << cuT( "] - Required engine version : " ) << l_toCheck );
			Version l_version = GetEngine()->GetVersion();

			if ( l_toCheck <= l_version )
			{
				m_loadedPluginTypes.insert( std::make_pair( p_pathFile, l_type ) );
				{
					auto l_lockPlugins = make_unique_lock( m_mutexLoadedPlugins );
					m_loadedPlugins[l_type].insert( std::make_pair( p_pathFile, l_return ) );
				}
				{
					auto l_lockLibraries = make_unique_lock( m_mutexLibraries );
					m_libraries[l_type].insert( std::make_pair( p_pathFile, l_library ) );
				}
				l_strToLog = cuT( "Plug-in [" );
				Logger::LogInfo( l_strToLog + l_return->GetName() + cuT( "] loaded" ) );
			}
			else
			{
				CASTOR_VERSION_EXCEPTION( l_toCheck, l_version );
			}
		}
		else
		{
			ePLUGIN_TYPE l_type = l_it->second;
			auto l_lock = make_unique_lock( m_mutexLoadedPlugins );
			l_return = m_loadedPlugins[l_type].find( p_pathFile )->second;
		}

		return l_return;
	}
}
