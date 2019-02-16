#include "ComCastor3D/ComError.hpp"

#include <CastorUtils/Log/Logger.hpp>

namespace CastorCom
{
	HRESULT CComError::dispatchWin32Error(
		DWORD dwError,
		REFCLSID clsid,
		LPCTSTR szSource,
		DWORD dwHelpContext,
		LPCTSTR szHelpFileName )
	{
		// Dispatch the requested error message
		return dispatchError(
				   HRESULT_FROM_WIN32( dwError ),
				   clsid, szSource, nullptr, dwHelpContext,
				   szHelpFileName );
	}

	HRESULT CComError::dispatchError(
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
		LPOLESTR wszError = nullptr;

		if ( szDescription != nullptr )
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
				LPTSTR szError = nullptr;

				if ( !::FormatMessage(
							FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
							nullptr,
							HRESULT_CODE( hError ),
							MAKELANGID( LANG_USER_DEFAULT, SUBLANG_DEFAULT ),
							( LPTSTR )&szError,
							0,
							nullptr ) )
				{
					return HRESULT_FROM_WIN32( ::GetLastError() );
				}

				// Convert the Error multibyte string to OLE string
				if ( szError != nullptr )
				{
					// Convert to wide char
					wszError = T2OLE( szError );
					// deallocate the multibyte string
					LocalFree( szError );
				}
			}
		}

		// Convert the source string to OLE string
		LPOLESTR wszSource = nullptr;

		if ( szSource != nullptr )
		{
			wszSource = T2OLE( ( LPTSTR )szSource );
		}

		// Convert the help filename to OLE string
		LPOLESTR wszHelpFile = nullptr;

		if ( szHelpFileName != nullptr )
		{
			wszHelpFile = T2OLE( ( LPTSTR )szHelpFileName );
		}

		// get the ICreateErrorInfo Interface
		ICreateErrorInfo * pCreateErrorInfo = nullptr;
		HRESULT hSuccess = CreateErrorInfo( &pCreateErrorInfo );
		ATLASSERT( SUCCEEDED( hSuccess ) );
		// Fill the error information into it
		pCreateErrorInfo->SetGUID( clsid );

		if ( wszError != nullptr )
		{
			pCreateErrorInfo->SetDescription( wszError );
		}

		if ( wszSource != nullptr )
		{
			pCreateErrorInfo->SetSource( wszSource );
		}

		if ( wszHelpFile != nullptr )
		{
			pCreateErrorInfo->SetHelpFile( wszHelpFile );
		}

		pCreateErrorInfo->SetHelpContext( dwHelpContext );
		// get the IErrorInfo interface
		IErrorInfo * pErrorInfo = nullptr;
		hSuccess = pCreateErrorInfo->QueryInterface( IID_IErrorInfo, ( LPVOID * )&pErrorInfo );

		if ( FAILED( hSuccess ) )
		{
			pCreateErrorInfo->Release();
			return hSuccess;
		}

		// set this error information in the current thread
		hSuccess = SetErrorInfo( 0, pErrorInfo );

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
