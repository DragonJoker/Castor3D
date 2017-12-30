/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_PLATFORM_CONFIG_H___
#define ___CASTOR_PLATFORM_CONFIG_H___

#if defined( ANDROID )
#	define CASTOR_PLATFORM_ANDROID
#elif defined( __linux__ )
#	define CASTOR_PLATFORM_LINUX
#elif defined( _WIN32 )
#	define CASTOR_PLATFORM_WINDOWS
#endif

#if defined( CASTOR_PLATFORM_WINDOWS )
#	if defined( CastorUtils_EXPORTS )
#		define CU_API __declspec(dllexport)
#	else
#		define CU_API __declspec(dllimport)
#	endif
#	define CASTOR_DLL_EXT cuT( "dll")
#	define dlerror() ::getLastError()
#else
#	define CASTOR_DLL_EXT cuT( "so")
#	define CU_API
#endif

#if !defined( CASTOR_PLATFORM_ANDROID ) && !defined( CASTOR_PLATFORM_LINUX ) && !defined( CASTOR_PLATFORM_WINDOWS )
#	error "Yet unsupported OS"
#endif

#endif
