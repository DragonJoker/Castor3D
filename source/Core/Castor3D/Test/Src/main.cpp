#include "Castor3DTestPrerequisites.hpp"

#include <Logger.hpp>
#include <File.hpp>

#include <BenchManager.hpp>

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
	Castor::Logger::Initialise( Castor::ELogType_DEBUG );
#endif

	Castor::Logger::SetFileName( Castor::File::GetExecutableDirectory() / cuT( "Tests.log" ) );
	BENCHLOOP( l_count, l_return );
	Castor::Logger::Cleanup();
	return l_return;
}
