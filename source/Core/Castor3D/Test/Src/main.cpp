#include "Castor3DTestPrerequisites.hpp"

#include <Log/Logger.hpp>
#include <Data/File.hpp>

#include <Engine.hpp>
#include <Cache/PluginCache.hpp>

#include <BenchManager.hpp>

#include "BinaryExportTest.hpp"
#include "SceneExportTest.hpp"

using namespace Castor;
using namespace Castor3D;

namespace
{
	void DoLoadPlugins( Engine & p_engine )
	{
		PathArray arrayFiles;
		File::ListDirectoryFiles( Engine::GetPluginsDirectory(), arrayFiles );
		PathArray arrayKept;

		// Exclude debug plug-in in release builds, and release plug-ins in debug builds
		for ( auto file : arrayFiles )
		{
#if defined( NDEBUG )

			if ( file.find( String( cuT( "d." ) ) + CASTOR_DLL_EXT ) == String::npos )
#else

			if ( file.find( String( cuT( "d." ) ) + CASTOR_DLL_EXT ) != String::npos )

#endif
			{
				arrayKept.push_back( file );
			}
		}

		if ( !arrayKept.empty() )
		{
			PathArray arrayFailed;
			PathArray otherPlugins;

			for ( auto file : arrayKept )
			{
				if ( file.GetExtension() == CASTOR_DLL_EXT )
				{
					// Since techniques depend on renderers, we load these first
					if ( file.find( cuT( "RenderSystem" ) ) != String::npos )
					{
						if ( !p_engine.GetPluginCache().LoadPlugin( file ) )
						{
							arrayFailed.push_back( file );
						}
					}
					else
					{
						otherPlugins.push_back( file );
					}
				}
			}

			// Then we load other plug-ins
			for ( auto file : otherPlugins )
			{
				if ( !p_engine.GetPluginCache().LoadPlugin( file ) )
				{
					arrayFailed.push_back( file );
				}
			}

			if ( !arrayFailed.empty() )
			{
				Logger::LogWarning( cuT( "Some plug-ins couldn't be loaded :" ) );

				for ( auto file : arrayFailed )
				{
					Logger::LogWarning( Path( file ).GetFileName() );
				}

				arrayFailed.clear();
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

		std::unique_ptr< Engine > result = std::make_unique< Engine >();
		DoLoadPlugins( *result );

		auto renderers = result->GetPluginCache().GetPlugins( PluginType::eRenderer );

		if ( renderers.empty() )
		{
			CASTOR_EXCEPTION( "No renderer plug-ins" );
		}

		if ( result->LoadRenderer( TestRender::TestRenderSystem::Type ) )
		{
			result->Initialise( 1, false );
		}
		else
		{
			CASTOR_EXCEPTION( "Couldn't load renderer." );
		}

		return result;
	}
}

int main( int argc, char const * argv[] )
{
	int result = EXIT_SUCCESS;
	int count = 1;

	if ( argc == 2 )
	{
		count = std::max< int >( 1, atoi( argv[2] ) );
	}

#if defined( NDEBUG )
	Castor::Logger::Initialise( Castor::LogType::eInfo );
#else
	Logger::Initialise( Castor::LogType::eDebug );
#endif

	Logger::SetFileName( Castor::File::GetExecutableDirectory() / cuT( "Castor3DTests.log" ) );
	{
		std::unique_ptr< Engine > engine = DoInitialiseCastor();

		// Test cases.
		Testing::Register( std::make_unique< Testing::BinaryExportTest >( *engine ) );
		Testing::Register( std::make_unique< Testing::SceneExportTest >( *engine ) );

		// Tests loop.
		BENCHLOOP( count, result );

		engine->Cleanup();
	}
	Logger::Cleanup();
	return result;
}
