#include "CastorUtils/PrecompiledHeader.hpp"

#include "CastorUtils/Utils.hpp"

#if defined( _WIN32 )
#	if defined( _MSC_VER )
#		pragma warning( push )
#		pragma warning( disable:4311 )
#		pragma warning( disable:4312 )
#	endif
#	include <windows.h>
#	if defined( _MSC_VER )
#		pragma warning( pop )
#	endif
#endif

#if defined( __GNUG__ )
#	include <sys/time.h>
#	include <errno.h>
#	include <iostream>
#	include <unistd.h>
#endif

uint8_t Castor::GetCPUCount()
{
#if !defined( _WIN32 )
#	if defined( __GNUG__ )
	char res[128];
	FILE * fp = popen( "/bin/cat /proc/cpuinfo |grep -c '^processor'", "r" );
	fread( res, 1, sizeof( res ) - 1, fp );
	pclose( fp );
	uint8_t l_byReturn = res[0];
#	else
#		error "Unsupported OS"
#	endif
#else
	SYSTEM_INFO sysinfo = { 0 };
	::GetSystemInfo( &sysinfo );
	uint8_t l_byReturn = uint8_t( sysinfo.dwNumberOfProcessors );
#endif

	return l_byReturn;
}

void Castor::Sleep( uint32_t p_uiTime )
{
#if defined( __GNUG__ )
	usleep( p_uiTime * 1000 );
#elif defined( _MSC_VER )
	::Sleep( p_uiTime );
#endif
}
