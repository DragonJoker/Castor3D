/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_PLATFORM_CONFIG_H___
#define ___CASTOR_PLATFORM_CONFIG_H___

#if defined( ANDROID )
#	define CU_PlatformAndroid
#elif defined( __linux__ )
#	define CU_PlatformLinux
#elif defined( _WIN32 )
#	define CU_PlatformWindows
#elif defined( __APPLE__ )
#	define CU_PlatformApple
#endif

#if defined( CU_PlatformWindows )
#	if defined( CastorUtils_EXPORTS )
#		define CU_API __declspec(dllexport)
#	else
#		define CU_API __declspec(dllimport)
#	endif
#	define CU_SharedLibExt cuT( "dll" )
#   define CU_LibPrefix cuT( "" )
#	define dlerror() ::getLastError()
#	define CU_stdcall __stdcall
#	ifndef NOMINMAX
#		define NOMINMAX
#	endif
#elif defined( CU_PlatformApple )
#	define CU_SharedLibExt cuT( "dylib" )
#   define CU_LibPrefix cuT( "lib" )
#	define CU_API
#	define CU_stdcall
#else
#	define CU_SharedLibExt cuT( "so" )
#   define CU_LibPrefix cuT( "lib" )
#	define CU_API
#	define CU_stdcall
#endif

#if !defined( CU_PlatformAndroid ) && !defined( CU_PlatformLinux ) && !defined( CU_PlatformWindows ) && !defined( CU_PlatformApple )
#	error "Yet unsupported OS"
#endif

#endif
