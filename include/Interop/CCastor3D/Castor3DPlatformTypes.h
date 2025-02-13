/*
See LICENSE file in root folder
*/
#ifndef ___CC3D_PlatformTypes_H___
#define ___CC3D_PlatformTypes_H___

#ifdef __cplusplus
extern "C"
{
#endif

#if defined( _WIN32 )
	struct C3DWin32WindowHandle_
	{
		void * hWnd;
	};
#elif defined( __linux__ )
	struct C3DX11WindowHandle_
	{
		Window drawable;
		Display * display;
	};
#elif defined( __APPLE__ )
	struct C3DMacOSWindowHandle_
	{
		void * view;
	};
#endif

#ifdef __cplusplus
}
#endif

#endif
