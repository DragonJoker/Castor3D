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
					if ( !engine.getPluginCache().loadPlugin( file ) )
					{
						arrayFailed.push_back( file );
					}
				}
			}

			if ( !arrayFailed.empty() )
			{
				Logger::logWarning( cuT( "Some plug-ins couldn't be loaded :" ) );

				for ( auto file : arrayFailed )
				{
					Logger::logWarning( file.getFileName() );
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
			, true
			, true );
		doloadPlugins( *result );

		auto & renderers = result->getRenderersList();

		if ( renderers.empty() )
		{
			CU_Exception( "No renderer plug-ins" );
		}

		if ( !result->loadRenderer( "test" ) )
		{
			CU_Exception( "Couldn't load renderer." );
		}

		return result;
	}
}

int main( int argc, char const * argv[] )
{
	uint32_t result = EXIT_SUCCESS;
	uint32_t count = 1;

	if ( argc == 2 )
	{
		count = uint32_t( std::max< int >( 1, atoi( argv[2] ) ) );
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
	}
	Logger::cleanup();
	return int( result );
}
