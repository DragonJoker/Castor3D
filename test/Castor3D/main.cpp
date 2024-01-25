#include "Castor3DTestPrerequisites.hpp"

#include "BinaryExportTest.hpp"
#include "SceneExportTest.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/PluginCache.hpp>

#include <BenchManager.hpp>

#include <CastorUtils/Log/Logger.hpp>
#include <CastorUtils/Data/File.hpp>

namespace
{
	castor::PathArray listPluginsFiles( castor::Path const & folder )
	{
		static castor::String castor3DLibPrefix{ CU_LibPrefix + castor::String{ cuT( "castor3d" ) } };
		castor::PathArray files;
		castor::File::listDirectoryFiles( folder, files );
		castor::PathArray result;
		castor::String endRel = cuT( "." ) + castor::String{ CU_SharedLibExt };
		castor::String endDbg = cuT( "d" ) + endRel;

		// Exclude debug plug-in in release builds, and release plug-ins in debug builds
		for ( auto const & file : files )
		{
			auto fileName = file.getFileName( true );
			bool res = castor::string::endsWith( fileName, endDbg );
#if defined( NDEBUG )
			res = castor::string::endsWith( fileName, endRel ) && !res;
#endif
			if ( res && fileName.find( castor3DLibPrefix ) == 0u )
			{
				result.emplace_back( file );
			}
		}

		return result;
	}

	void loadPlugins( castor3d::Engine & engine )
	{
		auto arrayKept = listPluginsFiles( castor3d::Engine::getPluginsDirectory() );

#if !defined( NDEBUG )

		// When debug is installed, plugins are installed in lib/Debug/Castor3D
		if ( arrayKept.empty() )
		{
			auto pathBin = castor::File::getExecutableDirectory();

			while ( pathBin.getFileName() != cuT( "bin" ) )
			{
				pathBin = pathBin.getPath();
			}

			auto pathUsr = pathBin.getPath();
			arrayKept = listPluginsFiles( pathUsr / cuT( "lib" ) / cuT( "Castor3D" ) );
		}

#endif

		if ( !arrayKept.empty() )
		{
			castor::PathArray arrayFailed;

			for ( auto const & file : arrayKept )
			{
				if ( !engine.getPluginCache().loadPlugin( file ) )
				{
					arrayFailed.emplace_back( file );
				}
			}

			if ( !arrayFailed.empty() )
			{
				castor::Logger::logWarning( cuT( "Some plug-ins couldn't be loaded :" ) );

				for ( auto const & file : arrayFailed )
				{
					castor::Logger::logWarning( file.getFileName() );
				}

				arrayFailed.clear();
			}
		}

		castor::Logger::logInfo( cuT( "Plugins loaded" ) );
	}

	castor::RawUniquePtr< castor3d::Engine > initialiseCastor()
	{
		if ( !castor::File::directoryExists( castor3d::Engine::getEngineDirectory() ) )
		{
			castor::File::directoryCreate( castor3d::Engine::getEngineDirectory() );
		}

		castor3d::EngineConfig config{ cuT( "Castor3DTest" )
			, castor3d::Version{ Castor3DTest_VERSION_MAJOR, Castor3DTest_VERSION_MINOR, Castor3DTest_VERSION_BUILD } };
		auto result = castor::make_unique< castor3d::Engine >( castor::move( config ) );
		loadPlugins( *result );

		if ( auto & renderers = result->getRenderersList();
			renderers.empty() )
		{
			CU_Exception( "No renderer plug-ins" );
		}

		if ( !result->loadRenderer( cuT( "test" ) ) )
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
	castor::Logger::initialise( castor::LogType::eDebug );
#endif

	castor::Logger::setFileName( castor::File::getExecutableDirectory() / cuT( "Castor3DTests.log" ) );
	{
		castor::RawUniquePtr< castor3d::Engine > engine = initialiseCastor();

		// Test cases.
		Testing::registerType( castor::make_unique< Testing::BinaryExportTest >( *engine ) );
		Testing::registerType( castor::make_unique< Testing::SceneExportTest >( *engine ) );

		// Tests loop.
		BENCHLOOP( count, result )
	}
	castor::Logger::cleanup();
	return int( result );
}
