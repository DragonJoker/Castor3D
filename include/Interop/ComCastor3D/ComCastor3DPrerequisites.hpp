/* See LICENSE file in root folder */
#ifndef ___C3DCOM_PREREQUISITES_H___
#define ___C3DCOM_PREREQUISITES_H___

#if defined( __linux__ )
#	define COMC3D_API
#elif defined( _WIN32 )
#	if defined( ComCastor3D_EXPORTS )
#		define COMC3D_API __declspec( dllexport )
#	else
#		define COMC3D_API __declspec( dllimport )
#	endif
#endif

#include <Castor3D/Castor3DPrerequisites.hpp>

// Include all the standard header *after* all the configuration
// settings have been made
#define _ATL_DLL

#ifndef STRICT
#	define STRICT
#endif

#ifndef _WIN32_WINNT
#	define _WIN32_WINNT 0x0501
#endif

#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit
#define ATL_NO_ASSERT_ON_DESTROY_NONEXISTENT_WINDOW

#include <atlbase.h>
#include <atlcom.h>
#include <atlwin.h>
#include <atltypes.h>
#include <atlctl.h>
#include <atlhost.h>
#include <initguid.h> // Realizes CLSID definitions

#ifndef TEXT
#	define TEXT( x ) _T( x )
#endif

namespace CastorCom
{
	class ComPosition;
	using tstring = std::basic_string< TCHAR >;

	static tstring const ERROR_UNINITIALISED_INSTANCE = _T( "The instance must be initialised" );
	static tstring const ERROR_UNSUPPORTED_C3D_TYPE = _T( "The type is unsupported" );

	template< typename T, typename FuncT >
	HRESULT retrieveValue( T * pVal, FuncT p_func )
	{
		HRESULT hr = E_POINTER;

		if ( *pVal )
		{
			*pVal = p_func();
			hr = S_OK;
		}

		return hr;
	}
}

#include "ComCastor3D/ComCastorUtils.hpp"
#include "ComCastor3D/Castor3D_h.h"
#include "ComCastor3D/Win32/resource.h"

#endif
