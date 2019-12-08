#include "Castor3DTestPrerequisites.hpp"

#include "BinaryExportTest.hpp"
#include "SceneExportTest.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/PluginCache.hpp>

#include <BenchManager.hpp>

#include <CastorUtils/Log/Logger.hpp>
#include <CastorUtils/Data/File.hpp>


using namespace castor;
using namespace castor3d;

namespace
{
	void doloadPlugins( Engine & engine )
	{
		PathArray arrayFiles;
		File::listDirectoryFiles( Engine::getPluginsDirectory(), arrayFiles );
		PathArray arrayKept;

		// Exclude debug plug-in in release builds, and release plug-ins in debug builds
		for ( auto file : arrayFiles )
		{
#if defined( NDEBUG )

			if ( file.find( String( cuT( "d." ) ) + CU_SharedLibExt ) == String::npos )
#else

			if ( file.find( String( cuT( "d." ) ) + CU_SharedLibExt ) != String::npos )

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
				if ( file.getExtension() == CU_SharedLibExt )
				{
					// Since techniques depend on renderers, we load these first
					if ( file.find( cuT( "RenderSystem" ) ) != String::npos )
					{
						if ( !engine.getPluginCache().loadPlugin( file ) )
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
				if ( !engine.getPluginCache().loadPlugin( file ) )
				{
					arrayFailed.push_back( file );
				}
			}

			if ( !arrayFailed.empty() )
			{
				Logger::logWarning( cuT( "Some plug-ins couldn't be loaded :" ) );

				for ( auto file : arrayFailed )
				{
					Logger::logWarning( Path( file ).getFileName() );
				}

				arrayFailed.clear();
			}
		}

		Logger::logInfo( cuT( "Plugins loaded" ) );
	}

	std::unique_ptr< Engine > doInitialiseCastor()
	{
		if ( !File::directoryExists( Engine::getEngineDirectory() ) )
		{
			File::directoryCreate( Engine::getEngineDirectory() );
		}

		std::unique_ptr< Engine > result = std::make_unique< Engine >( cuT( "Castor3DTest" )
			, Version{ Castor3DTest_VERSION_MAJOR, Castor3DTest_VERSION_MINOR, Castor3DTest_VERSION_BUILD }
			, true );
		doloadPlugins( *result );

		auto renderers = result->getPluginCache().getPlugins( PluginType::eRenderer );

		if ( renderers.empty() )
		{
			CU_Exception( "No renderer plug-ins" );
		}

		if ( result->loadRenderer( "test" ) )
		{
			result->initialise( 1, false );
		}
		else
		{
			CU_Exception( "Couldn't load renderer." );
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
	castor::Logger::initialise( castor::LogType::eInfo );
#else
	Logger::initialise( castor::LogType::eDebug );
#endif

	Logger::setFileName( castor::File::getExecutableDirectory() / cuT( "Castor3DTests.log" ) );
	{
		std::unique_ptr< Engine > engine = doInitialiseCastor();

		// Test cases.
		Testing::registerType( std::make_unique< Testing::BinaryExportTest >( *engine ) );
		Testing::registerType( std::make_unique< Testing::SceneExportTest >( *engine ) );

		// Tests loop.
		BENCHLOOP( count, result );

		engine->cleanup();
	}
	Logger::cleanup();
	return result;
}
