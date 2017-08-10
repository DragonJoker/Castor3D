#include "ComCastor3DTestPrerequisites.hpp"

#include <tchar.h>
#include <iostream>

template< typename T >
void  SafeRelease( T & x )
{
	if ( x )
	{
		x->Release();
		x = NULL;
	}
}

int _tmain( int argc, TCHAR * argv[] )
{
	HRESULT hr = ::CoInitialize( NULL );
	int iReturn = EXIT_SUCCESS;

	if ( hr == S_OK )
	{
		try
		{
			castor3d::ILoggerPtr logger = castor3d::ILoggerPtr( __uuidof( castor3d::Logger ) );
			logger->initialise( castor3d::eLOG_TYPE_DEBUG );
			logger->setFileName( _T( "ComCastor3DTest.log" ), castor3d::eLOG_TYPE_COUNT );

			try
			{
				logger->logInfo( _T( "ComCastor3DTest - Start" ) );
				castor3d::IEnginePtr engine = castor3d::IEnginePtr( __uuidof( castor3d::engine ) );
				engine->create();
				engine->destroy();
				logger->logInfo( _T( "ComCastor3DTest - Finish" ) );
			}
			catch ( _com_error & exc )
			{
				logger->logError( exc.ErrorMessage() );
				iReturn = EXIT_FAILURE;
			}

			logger->cleanup();
		}
		catch ( _com_error & exc )
		{
			std::wcout << exc.ErrorMessage() << std::endl;
			iReturn = EXIT_FAILURE;
		}
	}
	else
	{
		iReturn = EXIT_FAILURE;
	}

	return iReturn;
}
