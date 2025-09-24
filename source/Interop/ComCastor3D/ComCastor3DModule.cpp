#include "ComCastor3D/ComCastor3DModule.hpp"

#include <objbase.h>

#ifdef RegisterClass
#	undef RegisterClass
#endif

namespace CastorCom
{
	TCHAR const * CComCastor3DModule::m_tappId = _T( "{AE7400FF-58CB-40F5-9D8B-3373BCC45E54}" );
	OLECHAR const * CComCastor3DModule::m_oleappId = L"{AE7400FF-58CB-40F5-9D8B-3373BCC45E54}";
	CComCastor3DModule g_module;
}

#ifdef __cplusplus
extern "C"
{
#endif

// Used to determine whether the DLL can be unloaded by OLE.
__control_entrypoint( DllExport )
HRESULT STDAPICALLTYPE DllCanUnloadNow()
{
	return CastorCom::g_module.DllCanUnloadNow();
}

// Returns a class factory to create an object of the requested type.
_Check_return_
HRESULT STDAPICALLTYPE DllGetClassObject( _In_ REFCLSID rclsid, _In_ REFIID riid, _Outptr_ LPVOID FAR * ppv )
{
	return CastorCom::g_module.DllGetClassObject( rclsid, riid, ppv );
}

// DllRegisterServer - adds entries to the system registry.
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

// DllInstall - adds/Removes entries to the system registry per user per machine.
HRESULT STDAPICALLTYPE DllInstall( BOOL bInstall, _In_opt_ LPCWSTR pszCmdLine )
{
	auto hr = E_FAIL;

	if ( static const wchar_t szUserSwitch[] = L"user";
		pszCmdLine && ( _wcsnicmp( pszCmdLine, szUserSwitch, _countof( szUserSwitch ) ) == 0 ) )
		ATL::AtlSetPerUserRegistration( true );

	if ( bInstall )
	{
		hr = DllRegisterServer();
		if ( FAILED( hr ) )
			DllUnregisterServer();
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
	return CastorCom::g_module.DllMain( dwReason, lpReserved );
}

#ifdef __cplusplus
}
#endif
