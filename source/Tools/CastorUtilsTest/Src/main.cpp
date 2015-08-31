#include "Benchmark.hpp"

#include <Logger.hpp>
#include <File.hpp>

#include "BenchManager.hpp"
#include "OpenClBench.hpp"
#include "CastorUtilsBench.hpp"
#include "CastorUtilsTest.hpp"

int main( int argc, char const * argv[] )
{
	int l_iReturn = EXIT_SUCCESS;
	int l_iCount = 1;

	if ( argc == 2 )
	{
		l_iCount = std::max< int >( 1, atoi( argv[2] ) );
	}

	Castor::Logger::Initialise( Castor::ELogType_DEBUG );
	Castor::Logger::SetFileName( Castor::File::DirectoryGetCurrent() / cuT( "Tests.log" ) );
#if defined( CASTOR_USE_OCL )
	Testing::Register( std::make_shared< Testing::OpenCLBench >() );
#endif
	Testing::Register( std::make_shared< Testing::CastorUtilsBench >() );
	Testing::Register( std::make_shared< Testing::CastorUtilsTest >() );
	BENCHLOOP( l_iCount, l_iReturn );
	Castor::Logger::Cleanup();
	return l_iReturn;
}
