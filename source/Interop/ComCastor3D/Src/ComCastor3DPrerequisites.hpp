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
#ifndef ___C3DCOM_PREREQUISITES_H___
#define ___C3DCOM_PREREQUISITES_H___

#if defined( __linux__ )
#	define COMC3D_API
#elif defined( _WIN32 )
#	if defined( ComCastor3D_EXPORTS )
#		define COMC3D_API __declspec( dllexport )
#	else
#		define COMC3D_API __declspec( dllimport )
#	endif
#endif

#include <Castor3DPrerequisites.hpp>

// Include all the standard header *after* all the configuration
// settings have been made
#define _ATL_DLL

#ifndef STRICT
#	define STRICT
#endif

#ifndef _WIN32_WINNT
#	define _WIN32_WINNT 0x0501
#endif

#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit
#define ATL_NO_ASSERT_ON_DESTROY_NONEXISTENT_WINDOW

#include <atlbase.h>
#include <atlcom.h>
#include <atlwin.h>
#include <atltypes.h>
#include <atlctl.h>
#include <atlhost.h>
#include <initguid.h> // Realizes CLSID definitions

#ifndef TEXT
#	define TEXT( x ) _T( x )
#endif

namespace CastorCom
{
	class ComPosition;
	typedef std::basic_string< TCHAR > tstring;

	static const Castor::String ERROR_UNINITIALISED_INSTANCE = cuT( "The instance must be initialised" );
}

#include "ComCastorUtils.hpp"
#include "Castor3D_h.h"
#include "resource.h"

#endif
