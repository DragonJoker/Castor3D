/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
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
