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
#	define CU_SharedLibExt cuT( "dll")
#	define dlerror() ::getLastError()
#	ifndef NOMINMAX
#		define NOMINMAX
#	endif
#elif defined( CU_PlatformApple )
#	define CU_SharedLibExt cuT( "dylib")
#	define CU_API
#else
#	define CU_SharedLibExt cuT( "so")
#	define CU_API
#endif

#if !defined( CU_PlatformAndroid ) && !defined( CU_PlatformLinux ) && !defined( CU_PlatformWindows ) && !defined( CU_PlatformApple )
#	error "Yet unsupported OS"
#endif

#endif
