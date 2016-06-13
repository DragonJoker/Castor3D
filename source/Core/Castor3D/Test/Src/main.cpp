#include "Castor3DTestPrerequisites.hpp"

#include <Logger.hpp>
#include <File.hpp>

#include <Engine.hpp>
#include <PluginManager.hpp>

#include <BenchManager.hpp>

#include "BinaryExportTest.hpp"
#include "SceneExportTest.hpp"

using namespace Castor;
using namespace Castor3D;

namespace
{
	void DoLoadPlugins( Engine & p_engine )
	{
		PathArray l_arrayFiles;
		File::ListDirectoryFiles( Engine::GetPluginsDirectory(), l_arrayFiles );
		PathArray l_arrayKept;

		// Exclude debug plug-in in release builds, and release plug-ins in debug builds
		for ( auto l_file : l_arrayFiles )
		{
#if defined( NDEBUG )

			if ( l_file.find( String( cuT( "d." ) ) + CASTOR_DLL_EXT ) == String::npos )
#else

			if ( l_file.find( String( cuT( "d." ) ) + CASTOR_DLL_EXT ) != String::npos )

#endif
			{
				l_arrayKept.push_back( l_file );
			}
		}

		if ( !l_arrayKept.empty() )
		{
			PathArray l_arrayFailed;
			PathArray l_otherPlugins;

			for ( auto l_file : l_arrayKept )
			{
				if ( l_file.GetExtension() == CASTOR_DLL_EXT )
				{
					// Since techniques depend on renderers, we load these first
					if ( l_file.find( cuT( "RenderSystem" ) ) != String::npos )
					{
						if ( !p_engine.GetPluginManager().LoadPlugin( l_file ) )
						{
							l_arrayFailed.push_back( l_file );
						}
					}
					else
					{
						l_otherPlugins.push_back( l_file );
					}
				}
			}

			// Then we load other plug-ins
			for ( auto l_file : l_otherPlugins )
			{
				if ( !p_engine.GetPluginManager().LoadPlugin( l_file ) )
				{
					l_arrayFailed.push_back( l_file );
				}
			}

			if ( !l_arrayFailed.empty() )
			{
				Logger::LogWarning( cuT( "Some plug-ins couldn't be loaded :" ) );

				for ( auto l_file : l_arrayFailed )
				{
					Logger::LogWarning( Path( l_file ).GetFileName() );
				}

				l_arrayFailed.clear();
			}
		}

		Logger::LogInfo( cuT( "Plugins loaded" ) );
	}

	std::unique_ptr< Engine > DoInitialiseCastor()
	{
		if ( !File::DirectoryExists( Engine::GetEngineDirectory() ) )
		{
			File::DirectoryCreate( Engine::GetEngineDirectory() );
		}

		std::unique_ptr< Engine > l_return = std::make_unique< Engine >();
		DoLoadPlugins( *l_return );

		auto l_renderers = l_return->GetPluginManager().GetPlugins( ePLUGIN_TYPE_RENDERER );

		if ( l_renderers.empty() )
		{
			CASTOR_EXCEPTION( "No renderer plug-ins" );
		}

		if ( l_return->LoadRenderer( TestRender::TestRenderSystem::Name ) )
		{
			l_return->Initialise( 1, false );
		}
		else
		{
			CASTOR_EXCEPTION( "Couldn't load renderer." );
		}
		return l_return;
	}
}

int main( int argc, char const * argv[] )
{
	int l_return = EXIT_SUCCESS;
	int l_count = 1;

	if ( argc == 2 )
	{
		l_count = std::max< int >( 1, atoi( argv[2] ) );
	}

#if defined( NDEBUG )
	Castor::Logger::Initialise( Castor::ELogType_INFO );
#else
	Logger::Initialise( Castor::ELogType_DEBUG );
#endif

	Logger::SetFileName( Castor::File::GetExecutableDirectory() / cuT( "Castor3DTests.log" ) );
	{
		std::unique_ptr< Engine > l_engine = DoInitialiseCastor();

		// Test cases.
		//Testing::Register( std::make_shared< Testing::BinaryExportTest >( *l_engine ) );
		Testing::Register( std::make_shared< Testing::SceneExportTest >( *l_engine ) );

		// Tests loop.
		BENCHLOOP( l_count, l_return );

		l_engine->Cleanup();
	}
	Logger::Cleanup();
	return l_return;
}
