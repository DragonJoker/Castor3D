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
#ifndef ___CASTOR_PLATFORM_CONFIG_H___
#define ___CASTOR_PLATFORM_CONFIG_H___

#if defined( __linux__ )
#	define CU_API
#elif defined( _WIN32 )
#	if defined( CastorUtils_EXPORTS )
#		define CU_API __declspec(dllexport)
#	else
#		define CU_API __declspec(dllimport)
#	endif
#endif

#if defined( _WIN32 )
#	if defined( _MSC_VER )
#		define CASTOR_DLL_PREFIX	cuT( "")
#	else
#		define CASTOR_DLL_PREFIX	cuT( "lib")
#	endif
#	define CASTOR_DLL_EXT			cuT( "dll")
#	define dlerror()				::GetLastError()
#else
#	define CASTOR_DLL_EXT			cuT( "so")
#	define CASTOR_DLL_PREFIX		cuT( "lib")
#endif

#if !defined( __linux__ ) && !defined( _WIN32 )
#	error "Yet unsupported OS"
#endif

#endif