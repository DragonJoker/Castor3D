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

#if defined( __x86_64__ ) || defined( _M_X64 )
#	define CU_ArchX86_64
#elif defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86)
#	define CU_ArchX86_32
#elif defined(__ARM_ARCH_2__)
#	define CU_ArchARM2
#elif defined(__ARM_ACH_3__) || defined(__ARM_ARCH_3M__)
#	define CU_ArchARMv3
#elif defined(__ARM_ARCH_4T__) || defined(__TARGET_ARM_4T)
#	define CU_ArchARM4T
#elif defined(__ARM_ARCH_5_) || defined(__ARM_ARCH_5E_)
#	define CU_ArchARM5
#elif defined(__ARM_ARCH_5T_) || defined(__ARM_ARCH_5TE_) || defined(__ARM_ARCH_5TEJ_)
#	define CU_ArchARM5T
#elif defined(__ARM_ARCH_6T2_)
#	define CU_ArchARM6T2
#elif defined(__ARM_ARCH_6__) || defined(__ARM_ARCH_6J__) || defined(__ARM_ARCH_6K__) || defined(__ARM_ARCH_6Z__) || defined(__ARM_ARCH_6ZK__)
#	define CU_ArchARM6
#elif defined(__ARM_ARCH_7__) || defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7S__)
#	define CU_ArchARM7
#elif defined(__aarch64__) || defined(_M_ARM64)
#	define CU_ArchARM64
#else
#	error "Yet unsupported architecture"
#endif

#if defined( CU_PlatformWindows )
#	if defined( CastorUtils_EXPORTS )
#		define CU_API __declspec(dllexport)
#	else
#		define CU_API __declspec(dllimport)
#	endif
#	define CU_SharedLibExt cuT( "dll" )
#	define CU_SharedLibPrefix cuT( "")
#   define CU_LibPrefix cuT( "" )
#	define CU_stdcall __stdcall
#	ifndef NOMINMAX
#		define NOMINMAX
#	endif
#elif defined( CU_PlatformApple )
#	define CU_SharedLibExt cuT( "dylib" )
#	define CU_SharedLibPrefix cuT( "lib")
#   define CU_LibPrefix cuT( "lib" )
#	define CU_API
#	define CU_stdcall
#else
#	define CU_SharedLibExt cuT( "so" )
#	define CU_SharedLibPrefix cuT( "lib")
#   define CU_LibPrefix cuT( "lib" )
#	define CU_API
#	define CU_stdcall
#endif

#if !defined( CU_PlatformAndroid ) && !defined( CU_PlatformLinux ) && !defined( CU_PlatformWindows ) && !defined( CU_PlatformApple )
#	error "Yet unsupported OS"
#endif

#endif
