#include "ComCastor3DModule.hpp"
#include "ComPosition.hpp"
#include "ComSize.hpp"

#include <objbase.h>

#undef RegisterClass

namespace CastorCom
{
	TCHAR * CComCastor3DModule::m_appId = _T( "{AE7400FF-58CB-40F5-9D8B-3373BCC45E54}" );
	CComCastor3DModule g_module;
}

#ifdef __cplusplus
extern "C"
{
#endif
	
	// Used to determine whether the DLL can be unloaded by OLE.
	HRESULT STDAPICALLTYPE DllCanUnloadNow()
	{
		return CastorCom::g_module.DllCanUnloadNow();
	}

	// Returns a class factory to create an object of the requested type.
	HRESULT STDAPICALLTYPE DllGetClassObject( REFCLSID rclsid, REFIID riid, LPVOID * ppv )
	{
		return CastorCom::g_module.DllGetClassObject( rclsid, riid, ppv );
	}

	// DllRegisterServer - Adds entries to the system registry.
	HRESULT STDAPICALLTYPE DllRegisterServer()
	{
		// registers object, typelib and all interfaces in typelib
		HRESULT hr = CastorCom::g_module.DllRegisterServer( TRUE );
		return hr;
	}

	// DllUnregisterServer - Removes entries from the system registry.
	HRESULT STDAPICALLTYPE DllUnregisterServer()
	{
		HRESULT hr = CastorCom::g_module.DllUnregisterServer();
		return hr;
	}

	// DllInstall - Adds/Removes entries to the system registry per user per machine.
	HRESULT STDAPICALLTYPE DllInstall( BOOL bInstall, LPCWSTR pszCmdLine )
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
	BOOL STDAPICALLTYPE DllMain( HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved )
	{
		hInstance;
		return CastorCom::g_module.DllMain( dwReason, lpReserved );
	}

#ifdef __cplusplus
}
#endif
