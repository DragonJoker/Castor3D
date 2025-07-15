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
	c3d::PathArray listPluginsFiles( c3d::Path const & folder )
	{
		static c3d::String castor3DLibPrefix{ CU_LibPrefix + c3d::String{ cuT( "castor3d" ) } };
		c3d::PathArray files;
		c3d::File::listDirectoryFiles( folder, files );
		c3d::PathArray result;
		c3d::String endRel = cuT( "." ) + c3d::String{ CU_SharedLibExt };
		c3d::String endDbg = cuT( "d" ) + endRel;

		// Exclude debug plug-in in release builds, and release plug-ins in debug builds
		for ( auto const & file : files )
		{
			auto fileName = file.getFileName( true );
			bool res = c3d::string::endsWith( fileName, endDbg );
#if defined( NDEBUG )
			res = c3d::string::endsWith( fileName, endRel ) && !res;
#endif
			if ( res && fileName.find( castor3DLibPrefix ) == 0u )
			{
				result.emplace_back( file );
			}
		}

		return result;
	}

	void loadPlugins( c3d::Engine & engine )
	{
		auto arrayKept = listPluginsFiles( c3d::Engine::getPluginsDirectory() );

#if !defined( NDEBUG )

		// When debug is installed, plugins are installed in lib/Debug/Castor3D
		if ( arrayKept.empty() )
		{
			auto pathBin = c3d::File::getExecutableDirectory();

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
			c3d::PathArray arrayFailed;

			for ( auto const & file : arrayKept )
			{
				if ( !engine.getPluginCache().loadPlugin( file ) )
				{
					arrayFailed.emplace_back( file );
				}
			}

			if ( !arrayFailed.empty() )
			{
				c3d::Logger::logWarning( cuT( "Some plug-ins couldn't be loaded :" ) );

				for ( auto const & file : arrayFailed )
				{
					c3d::Logger::logWarning( file.getFileName() );
				}

				arrayFailed.clear();
			}
		}

		c3d::Logger::logInfo( cuT( "Plugins loaded" ) );
	}

	c3d::RawUniquePtr< c3d::Engine > initialiseCastor()
	{
		if ( !c3d::File::directoryExists( c3d::Engine::getEngineDirectory() ) )
		{
			c3d::File::directoryCreate( c3d::Engine::getEngineDirectory() );
		}

		c3d::EngineConfig config{ cuT( "Castor3DTest" )
			, c3d::Version{ Castor3DTest_VERSION_MAJOR, Castor3DTest_VERSION_MINOR, Castor3DTest_VERSION_BUILD } };
		auto result = c3d::makeRawUnique< c3d::Engine >( c3d::move( config ) );
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
	c3d::Logger::initialise( c3d::LogType::eInfo );
#else
	c3d::Logger::initialise( c3d::LogType::eDebug );
#endif

	c3d::Logger::setFileName( c3d::File::getExecutableDirectory() / cuT( "Castor3DTests.log" ) );
	{
		c3d::RawUniquePtr< c3d::Engine > engine = initialiseCastor();

		// Test cases.
		Testing::registerType( c3d::makeRawUnique< Testing::BinaryExportTest >( *engine ) );
		Testing::registerType( c3d::makeRawUnique< Testing::SceneExportTest >( *engine ) );

		// Tests loop.
		BENCHLOOP( count, result )
	}
	c3d::Logger::cleanup();
	return int( result );
}
