/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_COMPILER_CONFIG_H___
#define ___CASTOR_COMPILER_CONFIG_H___

#undef CASTOR_HAS_ALIGNAS

#if defined( _MSC_VER )

#	define CASTOR_COMPILER_MSVC
#	define CASTOR_COMPILER_VERSION _MSC_VER
#	if !defined( VLD_AVAILABLE )
#		define _CRTDBG_MAP_ALLOC
#		include <stdlib.h>
#		include <crtdbg.h>
#	endif
#	if CASTOR_COMPILER_VERSION < 1900
#		error "Your compiler is too old, consider an upgrade."
#	endif

#elif defined( __clang__ )

#	define CASTOR_COMPILER_CLANG
#	define CASTOR_COMPILER_GNUC
#	if ( ! __has_feature( cxx_alignas ) || !__has_feature( cxx_nullptr ) || !__has_feature( cxx_defaulted_functions ) || !__has_feature( cxx_deleted_functions ) || !__has_feature( cxx_variadic_templates ) )
#		error "Your compiler is too old, consider an upgrade."
#	endif

#elif defined( __GNUG__ )

#	define CASTOR_COMPILER_GNUC
#	define CASTOR_COMPILER_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#	if CASTOR_COMPILER_VERSION < 40900
#		error "Your compiler is too old, consider an upgrade."
#	endif

#elif defined( __BORLANDC__ )

#	define CASTOR_COMPILER_BORLAND
#	define CASTOR_COMPILER_VERSION __BORLANDC__
#	warning "Theoretically supported compiler, but untested yet"
#	if CASTOR_COMPILER_VERSION <= 0x621
#		error "Your compiler is too old, consider an upgrade."
#	endif

#else
#	error "Yet unsupported compiler"
#endif

#if defined( CASTOR_COMPILER_MSVC )
#	define CASTOR_DLL_PREFIX cuT( "")
#else
#	define CASTOR_DLL_PREFIX cuT( "lib")
#endif

#endif
