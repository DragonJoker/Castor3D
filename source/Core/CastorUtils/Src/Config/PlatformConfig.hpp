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
#	define dlerror() ::GetLastError()
#else
#	define CASTOR_DLL_EXT cuT( "so")
#	define CU_API
#endif

#if !defined( CASTOR_PLATFORM_ANDROID ) && !defined( CASTOR_PLATFORM_LINUX ) && !defined( CASTOR_PLATFORM_WINDOWS )
#	error "Yet unsupported OS"
#endif

#endif
