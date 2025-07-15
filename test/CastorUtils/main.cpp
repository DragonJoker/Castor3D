#include "OpenClBench.hpp"
#include "CastorUtilsArrayViewTest.hpp"
#include "CastorUtilsBuddyAllocatorTest.hpp"
#include "CastorUtilsDynamicBitsetTest.hpp"
#include "CastorUtilsMatrixTest.hpp"
#include "CastorUtilsPixelBufferExtractTest.hpp"
#include "CastorUtilsPixelFormatTest.hpp"
#include "CastorUtilsQuaternionTest.hpp"
#include "CastorUtilsSignalTest.hpp"
#include "CastorUtilsSpeedTest.hpp"
#include "CastorUtilsStringTest.hpp"
#include "CastorUtilsTextWriterTest.hpp"
#include "CastorUtilsThreadPoolTest.hpp"
#include "CastorUtilsUniqueTest.hpp"
#include "CastorUtilsWorkerThreadTest.hpp"
#include "CastorUtilsZipTest.hpp"

#include <CastorTest/Benchmark.hpp>
#include <CastorTest/BenchManager.hpp>

#include <CastorUtils/Log/Logger.hpp>
#include <CastorUtils/Data/File.hpp>

int main( int argc, char const * argv[] )
{
	uint32_t iReturn = EXIT_SUCCESS;
	uint32_t iCount = 1;

	if ( argc == 2 )
	{
		iCount = uint32_t( std::max< int >( 1, atoi( argv[2] ) ) );
	}

#if defined( NDEBUG )
	c3d::Logger::initialise( c3d::LogType::eInfo );
#else
	c3d::Logger::initialise( c3d::LogType::eDebug );
#endif

	c3d::Logger::setFileName( c3d::File::getExecutableDirectory() / cuT( "Tests.log" ) );
#if defined( CASTOR_USE_OCL )
	Testing::registerType( c3d::makeRawUnique< Testing::OpenCLBench >() );
#endif
	Testing::registerType( c3d::makeRawUnique< Testing::CastorUtilsDynamicBitsetTest >() );
	Testing::registerType( c3d::makeRawUnique< Testing::CastorUtilsBuddyAllocatorTest >() );
	Testing::registerType( c3d::makeRawUnique< Testing::CastorUtilsSignalTest >() );
	Testing::registerType( c3d::makeRawUnique< Testing::CastorUtilsWorkerThreadTest >() );
	Testing::registerType( c3d::makeRawUnique< Testing::CastorUtilsThreadPoolTest >() );
	Testing::registerType( c3d::makeRawUnique< Testing::CastorUtilsArrayViewTest >() );
	Testing::registerType( c3d::makeRawUnique< Testing::CastorUtilsUniqueTest >() );
	Testing::registerType( c3d::makeRawUnique< Testing::CastorUtilsMatrixTest >() );
	Testing::registerType( c3d::makeRawUnique< Testing::CastorUtilsMatrixBench >() );
	Testing::registerType( c3d::makeRawUnique< Testing::CastorUtilsPixelFormatTest >() );
	Testing::registerType( c3d::makeRawUnique< Testing::CastorUtilsStringTest >() );
	Testing::registerType( c3d::makeRawUnique< Testing::CastorUtilsZipTest >() );
	Testing::registerType( c3d::makeRawUnique< Testing::CastorUtilsQuaternionTest >() );
	Testing::registerType( c3d::makeRawUnique< Testing::CastorUtilsSpeedTest >() );
	Testing::registerType( c3d::makeRawUnique< Testing::CastorUtilsTextWriterTest >() );
	Testing::registerType( c3d::makeRawUnique< Testing::CastorUtilsPixelBufferExtractTest >() );
	BENCHLOOP( iCount, iReturn );
	c3d::Logger::cleanup();
	return int( iReturn );
}
