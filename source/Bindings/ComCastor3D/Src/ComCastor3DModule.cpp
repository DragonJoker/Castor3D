#include "ComCastor3DModule.hpp"
#include "ComPosition.hpp"
#include "ComSize.hpp"

#include <objbase.h>

#undef RegisterClass

namespace CastorCom
{
	TCHAR * CComCastor3DModule::m_appId = _T( "{AE7400FF-58CB-40F5-9D8B-3373BCC45E54}" );
	CComCastor3DModule _AtlModule;
}


// Used to determine whether the DLL can be unloaded by OLE.
STDAPI DllCanUnloadNow()
{
	return CastorCom::_AtlModule.DllCanUnloadNow();
}

// Returns a class factory to create an object of the requested type.
STDAPI DllGetClassObject( REFCLSID rclsid, REFIID riid, LPVOID * ppv )
{
	return CastorCom::_AtlModule.DllGetClassObject( rclsid, riid, ppv );
}

// DllRegisterServer - Adds entries to the system registry.
STDAPI DllRegisterServer()
{
	// registers object, typelib and all interfaces in typelib
	HRESULT hr = CastorCom::_AtlModule.DllRegisterServer( TRUE );
	return hr;
}

// DllUnregisterServer - Removes entries from the system registry.
STDAPI DllUnregisterServer()
{
	HRESULT hr = CastorCom::_AtlModule.DllUnregisterServer();
	return hr;
}

// DllInstall - Adds/Removes entries to the system registry per user per machine.
STDAPI DllInstall( BOOL bInstall, LPCWSTR pszCmdLine )
{
	HRESULT hr = E_FAIL;
	static const wchar_t szUserSwitch[] = L"user";

	if ( pszCmdLine )
	{
		if ( _wcsnicmp( pszCmdLine, szUserSwitch, _countof( szUserSwitch ) ) == 0 )
		{
			ATL::AtlSetPerUserRegistration( true );
		}
	}

	if ( bInstall )
	{
		hr = DllRegisterServer();

		if ( FAILED( hr ) )
		{
			DllUnregisterServer();
		}
	}
	else
	{
		hr = DllUnregisterServer();
	}

	return hr;
}

// DLL Entry Point
extern "C" BOOL WINAPI DllMain( HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved )
{
	hInstance;
	return CastorCom::_AtlModule.DllMain( dwReason, lpReserved );
}
