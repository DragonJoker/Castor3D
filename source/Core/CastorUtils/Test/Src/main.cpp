#include "Benchmark.hpp"

#include <Log/Logger.hpp>
#include <Data/File.hpp>

#include "BenchManager.hpp"
#include "OpenClBench.hpp"
#include "CastorUtilsArrayViewTest.hpp"
#include "CastorUtilsMatrixTest.hpp"
#include "CastorUtilsPixelFormatTest.hpp"
#include "CastorUtilsStringTest.hpp"
#include "CastorUtilsZipTest.hpp"
#include "CastorUtilsUniqueTest.hpp"
#include "CastorUtilsObjectsPoolTest.hpp"
#include "CastorUtilsQuaternionTest.hpp"
#include "CastorUtilsThreadPoolTest.hpp"
#include "CastorUtilsWorkerThreadTest.hpp"

int main( int argc, char const * argv[] )
{
	int iReturn = EXIT_SUCCESS;
	int iCount = 1;

	if ( argc == 2 )
	{
		iCount = std::max< int >( 1, atoi( argv[2] ) );
	}

#if defined( NDEBUG )
	Castor::Logger::Initialise( Castor::LogType::eInfo );
#else
	Castor::Logger::Initialise( Castor::LogType::eDebug );
#endif

	Castor::Logger::SetFileName( Castor::File::GetExecutableDirectory() / cuT( "Tests.log" ) );
#if defined( CASTOR_USE_OCL )
	Testing::Register( std::make_unique< Testing::OpenCLBench >() );
#endif
	Testing::Register( std::make_unique< Testing::CastorUtilsWorkerThreadTest >() );
	Testing::Register( std::make_unique< Testing::CastorUtilsThreadPoolTest >() );
	Testing::Register( std::make_unique< Testing::CastorUtilsMatrixBench >() );
	Testing::Register( std::make_unique< Testing::CastorUtilsArrayViewTest >() );
	Testing::Register( std::make_unique< Testing::CastorUtilsUniqueTest >() );
	Testing::Register( std::make_unique< Testing::CastorUtilsMatrixTest >() );
	Testing::Register( std::make_unique< Testing::CastorUtilsPixelFormatTest >() );
	//Testing::Register( std::make_unique< Testing::CastorUtilsStringTest >() );
	Testing::Register( std::make_unique< Testing::CastorUtilsZipTest >() );
	Testing::Register( std::make_unique< Testing::CastorUtilsObjectsPoolTest >() );
	Testing::Register( std::make_unique< Testing::CastorUtilsQuaternionTest >() );
	BENCHLOOP( iCount, iReturn );
	Castor::Logger::Cleanup();
	return iReturn;
}
