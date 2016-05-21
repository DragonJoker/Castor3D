#include "ComCastor3DTestPrerequisites.hpp"

#import <Castor3D.tlb>

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
			Castor3D::ILoggerPtr logger = Castor3D::ILoggerPtr( __uuidof( Castor3D::Logger ) );
			logger->Initialise( Castor3D::eLOG_TYPE_DEBUG );
			logger->SetFileName( _T( "ComCastor3DTest.log" ), Castor3D::eLOG_TYPE_COUNT );

			try
			{
				logger->LogInfo( _T( "ComCastor3DTest - Start" ) );
				Castor3D::IEnginePtr engine = Castor3D::IEnginePtr( __uuidof( Castor3D::engine ) );
				engine->Create();
				engine->Destroy();
				logger->LogInfo( _T( "ComCastor3DTest - Finish" ) );
			}
			catch ( _com_error & exc )
			{
				logger->LogError( exc.ErrorMessage() );
				iReturn = EXIT_FAILURE;
			}

			logger->Cleanup();
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
