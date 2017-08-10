#include "ComError.hpp"

#include <Log/Logger.hpp>

namespace CastorCom
{
	HRESULT CComError::DispatchWin32Error(
		DWORD dwError,
		REFCLSID clsid,
		LPCTSTR szSource,
		DWORD dwHelpContext,
		LPCTSTR szHelpFileName )
	{
		// Dispatch the requested error message
		return DispatchError(
				   HRESULT_FROM_WIN32( dwError ),
				   clsid, szSource, NULL, dwHelpContext,
				   szHelpFileName );
	}

	HRESULT CComError::DispatchError(
		HRESULT hError,
		REFCLSID clsid,
		LPCTSTR szSource,
		LPCTSTR szDescription,
		DWORD dwHelpContext,
		LPCTSTR szHelpFileName )
	{
		castor::Logger::logError( std::string( szSource ) + " - " + std::string( szDescription ) );
		// This function uses ATL conversion macros
		// (Hence we must use this MACRO provided by ATL)
		USES_CONVERSION;
		// Convert the description to OLE string
		LPOLESTR wszError = NULL;

		if ( szDescription != NULL )
		{
			// Convert to wide char
			wszError = T2OLE( ( LPTSTR )szDescription );
		}
		else
		{
			// If the code is a Win32 error code
			if ( HRESULT_FACILITY( hError ) == FACILITY_WIN32 )
			{
				// get the error from the system
				LPTSTR szError = NULL;

				if ( !::FormatMessage(
							FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
							NULL,
							HRESULT_CODE( hError ),
							MAKELANGID( LANG_USER_DEFAULT, SUBLANG_DEFAULT ),
							( LPTSTR )&szError,
							0,
							NULL ) )
				{
					return HRESULT_FROM_WIN32( getLastError() );
				}

				// Convert the Error multibyte string to OLE string
				if ( szError != NULL )
				{
					// Convert to wide char
					wszError = T2OLE( szError );
					// deallocate the multibyte string
					LocalFree( szError );
				}
			}
		}

		// Convert the source string to OLE string
		LPOLESTR wszSource = NULL;

		if ( szSource != NULL )
		{
			wszSource = T2OLE( ( LPTSTR )szSource );
		}

		// Convert the help filename to OLE string
		LPOLESTR wszHelpFile = NULL;

		if ( szHelpFileName != NULL )
		{
			wszHelpFile = T2OLE( ( LPTSTR )szHelpFileName );
		}

		// get the ICreateErrorInfo Interface
		ICreateErrorInfo * pCreateErrorInfo = NULL;
		HRESULT hSuccess = CreateErrorInfo( &pCreateErrorInfo );
		ATLASSERT( SUCCEEDED( hSuccess ) );
		// Fill the error information into it
		pCreateErrorInfo->setGUID( clsid );

		if ( wszError != NULL )
		{
			pCreateErrorInfo->setDescription( wszError );
		}

		if ( wszSource != NULL )
		{
			pCreateErrorInfo->setSource( wszSource );
		}

		if ( wszHelpFile != NULL )
		{
			pCreateErrorInfo->setHelpFile( wszHelpFile );
		}

		pCreateErrorInfo->setHelpContext( dwHelpContext );
		// get the IErrorInfo interface
		IErrorInfo * pErrorInfo = NULL;
		hSuccess = pCreateErrorInfo->QueryInterface( IID_IErrorInfo, ( LPVOID * )&pErrorInfo );

		if ( FAILED( hSuccess ) )
		{
			pCreateErrorInfo->Release();
			return hSuccess;
		}

		// set this error information in the current thread
		hSuccess = setErrorInfo( 0, pErrorInfo );

		if ( FAILED( hSuccess ) )
		{
			pCreateErrorInfo->Release();
			pErrorInfo->Release();
			return hSuccess;
		}

		// Finally release the interfaces
		pCreateErrorInfo->Release();
		pErrorInfo->Release();
		// And, Return the error code that was asked
		// to be dispatched
		return hError;
	}
}
