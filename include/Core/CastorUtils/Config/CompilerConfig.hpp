/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_COMPILER_CONFIG_H___
#define ___CASTOR_COMPILER_CONFIG_H___

#undef CASTOR_HAS_ALIGNAS

#if defined( _MSC_VER )

#	define CU_CompilerMSVC
#	define CU_CompilerVersion _MSC_VER
#	if !defined( VLD_AVAILABLE )
#		define _CRTDBG_MAP_ALLOC
#		include <stdlib.h>
#		include <crtdbg.h>
#	endif
#	if CU_CompilerVersion < 1900
#		error "Your compiler is too old, consider an upgrade."
#	endif

#elif defined( __clang__ )

#	define CU_CompilerCLANG
#	define CU_CompilerGNUC
#	if ( ! __has_feature( cxx_alignas ) || !__has_feature( cxx_nullptr ) || !__has_feature( cxx_defaulted_functions ) || !__has_feature( cxx_deleted_functions ) || !__has_feature( cxx_variadic_templates ) )
#		error "Your compiler is too old, consider an upgrade."
#	endif

#elif defined( __GNUG__ )

#	define CU_CompilerGNUC
#	define CU_CompilerVersion (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#	if CU_CompilerVersion < 40900
#		error "Your compiler is too old, consider an upgrade."
#	endif

#elif defined( __BORLANDC__ )

#	define CU_CompilerBORLAND
#	define CU_CompilerVersion __BORLANDC__
#	warning "Theoretically supported compiler, but untested yet"
#	if CU_CompilerVersion <= 0x621
#		error "Your compiler is too old, consider an upgrade."
#	endif

#else
#	error "Yet unsupported compiler"
#endif

#if defined( CU_CompilerMSVC )
#	define CU_SharedLibPrefix cuT( "")
#else
#	define CU_SharedLibPrefix cuT( "lib")
#endif

#endif
