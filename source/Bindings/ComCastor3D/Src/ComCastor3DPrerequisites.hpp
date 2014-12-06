/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
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
