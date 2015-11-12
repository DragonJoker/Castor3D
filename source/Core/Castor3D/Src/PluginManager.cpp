#include "PluginManager.hpp"

#include "DividerPlugin.hpp"
#include "GenericPlugin.hpp"
#include "ImporterPlugin.hpp"
#include "PostFxPlugin.hpp"
#include "RendererPlugin.hpp"
#include "TechniquePlugin.hpp"
#include "VersionException.hpp"

#include <DynamicLibrary.hpp>

using namespace Castor;

namespace Castor3D
{
#if defined( _MSC_VER)
	static const String GetTypeFunctionABIName = cuT( "?GetType@@YA?AW4ePLUGIN_TYPE@Castor3D@@XZ" );
#elif defined( __GNUG__)
	static const String GetTypeFunctionABIName = cuT( "_Z7GetTypev" );
#endif

	PluginManager::PluginManager( Engine & p_engine )
		: Manager< Path, PluginBase >( p_engine )
	{
	}

	PluginManager::~PluginManager()
	{
	}

	void PluginManager::Cleanup()
	{
	}

	void PluginManager::Clear()
	{
		m_mutexLoadedPlugins.lock();
		m_mutexLibraries.lock();
		m_loadedPluginTypes.clear();

		for ( auto & l_it : m_renderers )
		{
			l_it.reset();
		}

		for ( auto & l_it : m_loadedPlugins )
		{
			l_it.clear();
		}

		for ( auto & l_it : m_libraries )
		{
			l_it.clear();
		}

		m_mutexLibraries.unlock();
		m_mutexLoadedPlugins.unlock();
	}

	PluginBaseSPtr PluginManager::LoadPlugin( String const & p_strPluginName, Path const & p_pathFolder )throw( )
	{
		Path l_strFilePath = CASTOR_DLL_PREFIX + p_strPluginName + cuT( "." ) + CASTOR_DLL_EXT;
		PluginBaseSPtr l_return;

		try
		{
			l_return = InternalLoadPlugin( l_strFilePath );
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

	PluginBaseSPtr PluginManager::LoadPlugin( Path const & p_fileFullPath )throw( )
	{
		PluginBaseSPtr l_return;

		try
		{
			l_return = InternalLoadPlugin( p_fileFullPath );
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

	PluginStrMap PluginManager::GetPlugins( ePLUGIN_TYPE p_type )
	{
		auto l_lock = Castor::make_unique_lock( m_mutexLoadedPlugins );
		return m_loadedPlugins[p_type];
	}

	RenderSystem * PluginManager::LoadRenderer( eRENDERER_TYPE p_type )
	{
		bool l_return = false;
		m_mutexRenderers.lock();
		RendererPluginSPtr l_plugin = m_renderers[p_type];
		m_mutexRenderers.unlock();

		if ( l_plugin )
		{
			m_renderSystem = l_plugin->CreateRenderSystem( GetOwner() );
		}

		return m_renderSystem;
	}

	void PluginManager::LoadAllPlugins( Path const & p_strFolder )
	{
		PathArray l_arrayFiles;
		File::ListDirectoryFiles( p_strFolder, l_arrayFiles );

		if ( l_arrayFiles.size() > 0 )
		{
			for ( uint32_t i = 0; i < l_arrayFiles.size(); i++ )
			{
				Path l_file = l_arrayFiles[i];

				if ( l_file.GetExtension() == CASTOR_DLL_EXT )
				{
					try
					{
						InternalLoadPlugin( l_file );
					}
					catch ( ... )
					{
						Logger::LogInfo( cuT( "Can't load plugin : " ) + l_file );
					}
				}
			}
		}
	}

	PluginBaseSPtr PluginManager::LoadRendererPlugin( DynamicLibrarySPtr p_pLibrary )
	{
		RendererPluginSPtr l_pRenderer = std::make_shared< RendererPlugin >( p_pLibrary, GetOwner() );
		PluginBaseSPtr l_return = std::static_pointer_cast<PluginBase, RendererPlugin>( l_pRenderer );
		eRENDERER_TYPE l_eRendererType = l_pRenderer->GetRendererType();

		if ( l_eRendererType == eRENDERER_TYPE_UNDEFINED )
		{
			l_return.reset();
		}
		else
		{
			m_mutexRenderers.lock();
			m_renderers[l_eRendererType] = l_pRenderer;
			m_mutexRenderers.unlock();
		}

		return l_return;
	}

	PluginBaseSPtr PluginManager::LoadTechniquePlugin( DynamicLibrarySPtr p_pLibrary )
	{
		return std::make_shared< TechniquePlugin >( p_pLibrary, GetOwner() );
	}

	PluginBaseSPtr PluginManager::InternalLoadPlugin( Path const & p_pathFile )
	{
		PluginBaseSPtr l_return;
		m_mutexLoadedPluginTypes.lock();
		auto l_it = m_loadedPluginTypes.find( p_pathFile );

		if ( l_it == m_loadedPluginTypes.end() )
		{
			m_mutexLoadedPluginTypes.unlock();

			if ( File::FileExists( p_pathFile ) )
			{
				DynamicLibrarySPtr l_pLibrary = std::make_shared< DynamicLibrary >();

				if ( !l_pLibrary->Open( p_pathFile ) )
				{
					CASTOR_PLUGIN_EXCEPTION( string::string_cast< char >( cuT( "Error encountered while loading file [" ) + p_pathFile + cuT( "]" ) ), true );
				}

				PluginBase::PGetTypeFunction l_pfnGetType;

				if ( !l_pLibrary->GetFunction( l_pfnGetType, GetTypeFunctionABIName ) )
				{
					String l_strError = cuT( "Error encountered while loading file [" ) + p_pathFile.GetFileName() + cuT( "] GetType plugin function => Not a Castor3D plugin" );
					CASTOR_PLUGIN_EXCEPTION( string::string_cast< char >( l_strError ), true );
				}

				ePLUGIN_TYPE l_type = l_pfnGetType();

				switch ( l_type )
				{
				case ePLUGIN_TYPE_DIVIDER:
					l_return = std::make_shared< DividerPlugin >( l_pLibrary, GetOwner() );
					break;

				case ePLUGIN_TYPE_IMPORTER:
					l_return = std::make_shared< ImporterPlugin >( l_pLibrary, GetOwner() );
					break;

				case ePLUGIN_TYPE_RENDERER:
					l_return = LoadRendererPlugin( l_pLibrary );
					break;

				case ePLUGIN_TYPE_GENERIC:
					l_return = std::make_shared< GenericPlugin >( l_pLibrary, GetOwner() );
					break;

				case ePLUGIN_TYPE_TECHNIQUE:
					l_return = LoadTechniquePlugin( l_pLibrary );
					break;

				case ePLUGIN_TYPE_POSTFX:
					l_return = std::make_shared< PostFxPlugin >( l_pLibrary, GetOwner() );
					break;

				default:
					break;
				}

				if ( l_return )
				{
					Version l_toCheck( 0, 0 );
					l_return->GetRequiredVersion( l_toCheck );
					String l_strToLog( cuT( "LoadPlugin - Plugin [" ) );
					Logger::LogInfo( StringStream() << l_strToLog << l_return->GetName() << cuT( "] - Required engine version : " ) << l_toCheck );
					Version l_version = GetOwner()->GetVersion();

					if ( l_toCheck <= l_version )
					{
						m_mutexLoadedPluginTypes.lock();
						m_loadedPluginTypes.insert( std::make_pair( p_pathFile, l_type ) );
						m_mutexLoadedPluginTypes.unlock();
						m_mutexLoadedPlugins.lock();
						m_loadedPlugins[l_type].insert( std::make_pair( p_pathFile, l_return ) );
						m_mutexLoadedPlugins.unlock();
						m_mutexLibraries.lock();
						m_libraries[l_type].insert( std::make_pair( p_pathFile, l_pLibrary ) );
						m_mutexLibraries.unlock();
						l_strToLog = cuT( "LoadPlugin - Plugin [" );
						Logger::LogInfo( l_strToLog + l_return->GetName() + cuT( "] loaded" ) );
					}
					else
					{
						CASTOR_VERSION_EXCEPTION( l_toCheck, l_version );
					}
				}
				else
				{
					// Plugin not loaded, due to an error
				}
			}
			else
			{
				// File doesn't exist
			}
		}
		else
		{
			ePLUGIN_TYPE l_type = l_it->second;
			m_mutexLoadedPluginTypes.unlock();
			m_mutexLoadedPlugins.lock();
			l_return = m_loadedPlugins[l_type].find( p_pathFile )->second;
			m_mutexLoadedPlugins.unlock();
		}

		return l_return;
	}
}
