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
#ifndef ___CASTOR_COMPILER_CONFIG_H___
#define ___CASTOR_COMPILER_CONFIG_H___

#undef CASTOR_USE_UNICODE
#undef CASTOR_HAS_NULLPTR
#undef CASTOR_HAS_STDSMARTPTR
#undef CASTOR_HAS_TR1SMARTPTR
#undef CASTOR_HAS_STDARRAY
#undef CASTOR_HAS_TR1ARRAY
#undef CASTOR_HAS_DEFAULTED_FUNC
#undef CASTOR_HAS_DELETED_FUNC
#undef CASTOR_HAS_STDFUNCTIONAL
#undef CASTOR_HAS_TR1FUNCTIONAL
#undef CASTOR_HAS_STDTHREAD
#undef CASTOR_HAS_VARIADIC_TEMPLATES
#undef CASTOR_HAS_MAKE_UNIQUE

#if defined( _MSC_VER)
#	if _MSC_VER >= 1900
#		define CASTOR_HAS_ALIGNAS					1
#		define CASTOR_HAS_STDTHREAD					1
#		define CASTOR_TYPE( x )						: x
#		define CASTOR_HAS_NULLPTR					1
#		define CASTOR_HAS_STDSMARTPTR				1
#		define CASTOR_HAS_STDFUNCTIONAL				1
#		define CASTOR_HAS_TR1FUNCTIONAL				1
#		define CASTOR_HAS_STDARRAY					1
#		define CASTOR_HAS_DEFAULTED_FUNC			1
#		define CASTOR_HAS_DELETED_FUNC				1
#		define CASTOR_HAS_TR1SMARTPTR				1
#		define CASTOR_HAS_TR1ARRAY					1
#		define CASTOR_HAS_VARIADIC_TEMPLATES		1
#		define CASTOR_HAS_MAKE_UNIQUE				1
#	elif _MSC_VER >= 1800
#		define CASTOR_HAS_ALIGNAS					0
#		define CASTOR_HAS_STDTHREAD					1
#		define CASTOR_TYPE( x )						: x
#		define CASTOR_HAS_NULLPTR					1
#		define CASTOR_HAS_STDSMARTPTR				1
#		define CASTOR_HAS_STDFUNCTIONAL				1
#		define CASTOR_HAS_TR1FUNCTIONAL				1
#		define CASTOR_HAS_STDARRAY					1
#		define CASTOR_HAS_DEFAULTED_FUNC			1
#		define CASTOR_HAS_DELETED_FUNC				1
#		define CASTOR_HAS_TR1SMARTPTR				1
#		define CASTOR_HAS_TR1ARRAY					1
#		define CASTOR_HAS_VARIADIC_TEMPLATES		1
#		define CASTOR_HAS_MAKE_UNIQUE				1
#	elif _MSC_VER >= 1700
#		define CASTOR_HAS_ALIGNAS					0
#		define CASTOR_HAS_STDTHREAD					1
#		define CASTOR_TYPE( x )						: x
#		define CASTOR_HAS_NULLPTR					1
#		define CASTOR_HAS_STDSMARTPTR				1
#		define CASTOR_HAS_STDFUNCTIONAL				1
#		define CASTOR_HAS_TR1FUNCTIONAL				1
#		define CASTOR_HAS_STDARRAY					1
#		define CASTOR_HAS_DEFAULTED_FUNC			1
#		define CASTOR_HAS_DELETED_FUNC				1
#		define CASTOR_HAS_TR1SMARTPTR				1
#		define CASTOR_HAS_TR1ARRAY					1
#		define CASTOR_HAS_VARIADIC_TEMPLATES		0
#		define CASTOR_HAS_MAKE_UNIQUE				0
#	elif _MSC_VER >= 1600
#		define CASTOR_HAS_ALIGNAS					0
#		define CASTOR_HAS_STDTHREAD					0
#		define CASTOR_TYPE( x )
#		define CASTOR_HAS_NULLPTR					1
#		define CASTOR_HAS_STDSMARTPTR				1
#		define CASTOR_HAS_STDFUNCTIONAL				0
#		define CASTOR_HAS_TR1FUNCTIONAL				1
#		define CASTOR_HAS_STDARRAY					1
#		define CASTOR_HAS_DEFAULTED_FUNC			1
#		define CASTOR_HAS_DELETED_FUNC				1
#		define CASTOR_HAS_TR1SMARTPTR				1
#		define CASTOR_HAS_TR1ARRAY					1
#		define CASTOR_HAS_VARIADIC_TEMPLATES		0
#		define CASTOR_HAS_MAKE_UNIQUE				0
#	elif _MSC_VER > 1500
#		define CASTOR_HAS_ALIGNAS					0
#		define CASTOR_HAS_STDTHREAD					0
#		define CASTOR_TYPE( x )
#		define CASTOR_HAS_NULLPTR					0
#		define CASTOR_HAS_STDSMARTPTR				0
#		define CASTOR_HAS_STDFUNCTIONAL				0
#		define CASTOR_HAS_TR1FUNCTIONAL				0
#		define CASTOR_HAS_STDARRAY					0
#		define CASTOR_HAS_DEFAULTED_FUNC			0
#		define CASTOR_HAS_DELETED_FUNC				0
#		define CASTOR_HAS_TR1SMARTPTR				1
#		define CASTOR_HAS_TR1ARRAY					1
#		define CASTOR_HAS_VARIADIC_TEMPLATES		0
#		define CASTOR_HAS_MAKE_UNIQUE				0
#	else
#		define CASTOR_HAS_ALIGNAS					0
#		define CASTOR_HAS_STDTHREAD					0
#		define CASTOR_TYPE( x )
#		define CASTOR_HAS_NULLPTR					0
#		define CASTOR_HAS_STDSMARTPTR				0
#		define CASTOR_HAS_STDFUNCTIONAL				0
#		define CASTOR_HAS_TR1FUNCTIONAL				0
#		define CASTOR_HAS_STDARRAY					0
#		define CASTOR_HAS_DEFAULTED_FUNC			0
#		define CASTOR_HAS_DELETED_FUNC				0
#		define CASTOR_HAS_TR1SMARTPTR				0
#		define CASTOR_HAS_TR1ARRAY					0
#		define CASTOR_HAS_VARIADIC_TEMPLATES		0
#		define CASTOR_HAS_MAKE_UNIQUE				0
#	endif
#elif defined( __clang__)
#	define CASTOR_HAS_ALIGNAS						__has_feature(cxx_alignas)
#	define CASTOR_HAS_STDTHREAD						1
#	define CASTOR_TYPE( x )							: x
#	define CASTOR_HAS_NULLPTR						__has_feature(cxx_nullptr)
#	define CASTOR_HAS_STDSMARTPTR					1
#	define CASTOR_HAS_STDARRAY						1
#	define CASTOR_HAS_DEFAULTED_FUNC				__has_feature(cxx_defaulted_functions)
#	define CASTOR_HAS_DELETED_FUNC					__has_feature(cxx_deleted_functions)
#	define CASTOR_HAS_STDFUNCTIONAL					1
#	define CASTOR_HAS_TR1FUNCTIONAL					0
#	define CASTOR_HAS_TR1SMARTPTR					0
#	define CASTOR_HAS_TR1ARRAY						0
#	define CASTOR_HAS_VARIADIC_TEMPLATES			__has_feature(cxx_variadic_templates)
#	define CASTOR_HAS_MAKE_UNIQUE					__has_feature(cxx_variadic_templates)
#elif defined( __GNUG__)
#	define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#	if defined( _WIN32 )
#		if GCC_VERSION >= 40900
#			define CASTOR_HAS_ALIGNAS				1
#			define CASTOR_TYPE( x )					: x
#			define CASTOR_HAS_STDTHREAD				0
#			define CASTOR_HAS_NULLPTR				1
#			define CASTOR_HAS_STDSMARTPTR			1
#			define CASTOR_HAS_STDARRAY				1
#			define CASTOR_HAS_DEFAULTED_FUNC		1
#			define CASTOR_HAS_DELETED_FUNC			1
#			define CASTOR_HAS_TR1SMARTPTR			1
#			define CASTOR_HAS_TR1ARRAY				1
#			define CASTOR_HAS_STDFUNCTIONAL			1
#			define CASTOR_HAS_TR1FUNCTIONAL			1
#			define CASTOR_HAS_VARIADIC_TEMPLATES	1
#			define CASTOR_HAS_MAKE_UNIQUE			1
#		elif GCC_VERSION >= 40800
#			define CASTOR_HAS_ALIGNAS				1
#			define CASTOR_TYPE( x )					: x
#			define CASTOR_HAS_STDTHREAD				1
#			define CASTOR_HAS_NULLPTR				1
#			define CASTOR_HAS_STDSMARTPTR			1
#			define CASTOR_HAS_STDARRAY				1
#			define CASTOR_HAS_DEFAULTED_FUNC		1
#			define CASTOR_HAS_DELETED_FUNC			1
#			define CASTOR_HAS_TR1SMARTPTR			1
#			define CASTOR_HAS_TR1ARRAY				1
#			define CASTOR_HAS_STDFUNCTIONAL			1
#			define CASTOR_HAS_TR1FUNCTIONAL			1
#			define CASTOR_HAS_VARIADIC_TEMPLATES	1
#			define CASTOR_HAS_MAKE_UNIQUE			1
#		elif GCC_VERSION >= 40600
#			define CASTOR_HAS_ALIGNAS				0
#			define CASTOR_TYPE( x )					: x
#			define CASTOR_HAS_STDTHREAD				0
#			define CASTOR_HAS_NULLPTR				1
#			define CASTOR_HAS_STDSMARTPTR			1
#			define CASTOR_HAS_STDARRAY				1
#			define CASTOR_HAS_DEFAULTED_FUNC		1
#			define CASTOR_HAS_DELETED_FUNC			1
#			define CASTOR_HAS_TR1SMARTPTR			1
#			define CASTOR_HAS_TR1ARRAY				1
#			define CASTOR_HAS_STDFUNCTIONAL			1
#			define CASTOR_HAS_TR1FUNCTIONAL			1
#			define CASTOR_HAS_VARIADIC_TEMPLATES	1
#			define CASTOR_HAS_MAKE_UNIQUE			0
#		elif GCC_VERSION >= 40500
#			define CASTOR_HAS_ALIGNAS				0
#			define CASTOR_TYPE( x )					: x
#			define CASTOR_HAS_STDTHREAD				1
#			define CASTOR_HAS_NULLPTR				1
#			define CASTOR_HAS_STDSMARTPTR			1
#			define CASTOR_HAS_STDARRAY				1
#			define CASTOR_HAS_DEFAULTED_FUNC		1
#			define CASTOR_HAS_DELETED_FUNC			1
#			define CASTOR_HAS_TR1SMARTPTR			1
#			define CASTOR_HAS_TR1ARRAY				1
#			define CASTOR_HAS_STDFUNCTIONAL			1
#			define CASTOR_HAS_TR1FUNCTIONAL			1
#			define CASTOR_HAS_VARIADIC_TEMPLATES	1
#			define CASTOR_HAS_MAKE_UNIQUE			1
#		else
#			define CASTOR_HAS_ALIGNAS				0
#			define CASTOR_TYPE( x )
#			define CASTOR_HAS_STDTHREAD				1
#			define CASTOR_HAS_NULLPTR				1
#			define CASTOR_HAS_STDSMARTPTR			1
#			define CASTOR_HAS_STDARRAY				1
#			define CASTOR_HAS_DEFAULTED_FUNC		1
#			define CASTOR_HAS_DELETED_FUNC			1
#			define CASTOR_HAS_TR1SMARTPTR			1
#			define CASTOR_HAS_TR1ARRAY				1
#			define CASTOR_HAS_STDFUNCTIONAL			1
#			define CASTOR_HAS_TR1FUNCTIONAL			1
#			define CASTOR_HAS_VARIADIC_TEMPLATES	1
#			define CASTOR_HAS_MAKE_UNIQUE			1
#		endif
#	else
#		if GCC_VERSION >= 40800
#			define CASTOR_HAS_ALIGNAS				1
#			define CASTOR_TYPE( x )					: x
#			define CASTOR_HAS_STDTHREAD				1
#			define CASTOR_HAS_NULLPTR				1
#			define CASTOR_HAS_STDSMARTPTR			1
#			define CASTOR_HAS_STDARRAY				1
#			define CASTOR_HAS_DEFAULTED_FUNC		1
#			define CASTOR_HAS_DELETED_FUNC			1
#			define CASTOR_HAS_TR1SMARTPTR			1
#			define CASTOR_HAS_TR1ARRAY				1
#			define CASTOR_HAS_STDFUNCTIONAL			1
#			define CASTOR_HAS_TR1FUNCTIONAL			1
#			define CASTOR_HAS_VARIADIC_TEMPLATES	1
#			define CASTOR_HAS_MAKE_UNIQUE			1
#		elif GCC_VERSION >= 40600
#			define CASTOR_TYPE( x )					: x
#			define CASTOR_HAS_STDTHREAD				1
#			define CASTOR_HAS_NULLPTR				1
#			define CASTOR_HAS_STDSMARTPTR			1
#			define CASTOR_HAS_STDARRAY				1
#			define CASTOR_HAS_DEFAULTED_FUNC		1
#			define CASTOR_HAS_DELETED_FUNC			1
#			define CASTOR_HAS_TR1SMARTPTR			1
#			define CASTOR_HAS_TR1ARRAY				1
#			define CASTOR_HAS_STDFUNCTIONAL			1
#			define CASTOR_HAS_TR1FUNCTIONAL			1
#			define CASTOR_HAS_VARIADIC_TEMPLATES	1
#			define CASTOR_HAS_MAKE_UNIQUE			0
#		elif GCC_VERSION >= 40400
#			define CASTOR_HAS_STDTHREAD				0
#			define CASTOR_HAS_NULLPTR				0
#			define CASTOR_HAS_STDFUNCTIONAL			0
#			define CASTOR_HAS_STDSMARTPTR			1
#			define CASTOR_HAS_STDARRAY				1
#			define CASTOR_HAS_DEFAULTED_FUNC		1
#			define CASTOR_HAS_DELETED_FUNC			1
#			define CASTOR_HAS_TR1FUNCTIONAL			1
#			define CASTOR_HAS_TR1SMARTPTR			1
#			define CASTOR_HAS_TR1ARRAY				1
#			define CASTOR_HAS_VARIADIC_TEMPLATES	1
#			define CASTOR_HAS_MAKE_UNIQUE			0
#		elif GCC_VERSION >= 40300
#			define CASTOR_HAS_STDTHREAD				0
#			define CASTOR_HAS_NULLPTR				0
#			define CASTOR_HAS_STDFUNCTIONAL			0
#			define CASTOR_HAS_STDSMARTPTR			0
#			define CASTOR_HAS_STDARRAY				0
#			define CASTOR_HAS_DEFAULTED_FUNC		0
#			define CASTOR_HAS_DELETED_FUNC			0
#			define CASTOR_HAS_TR1FUNCTIONAL			1
#			define CASTOR_HAS_TR1SMARTPTR			1
#			define CASTOR_HAS_TR1ARRAY				1
#			define CASTOR_HAS_VARIADIC_TEMPLATES	1
#			define CASTOR_HAS_MAKE_UNIQUE			0
#		else
#			define CASTOR_HAS_STDTHREAD				0
#			define CASTOR_HAS_NULLPTR				0
#			define CASTOR_HAS_STDFUNCTIONAL			0
#			define CASTOR_HAS_STDSMARTPTR			0
#			define CASTOR_HAS_STDARRAY				0
#			define CASTOR_HAS_DEFAULTED_FUNC		0
#			define CASTOR_HAS_DELETED_FUNC			0
#			define CASTOR_HAS_TR1SMARTPTR			0
#			define CASTOR_HAS_TR1ARRAY				0
#			define CASTOR_HAS_TR1FUNCTIONAL			0
#			define CASTOR_HAS_VARIADIC_TEMPLATES	0
#			define CASTOR_HAS_MAKE_UNIQUE			0
#		endif
#	endif
#elif defined( __BORLANDC__ )
#	warning "Theoretically supported compiler, but untested yet"
#	if __BORLANDC__ >= 0x621
#		define CASTOR_HAS_STDTHREAD					0
#		define CASTOR_HAS_NULLPTR					0
#		define CASTOR_HAS_STDFUNCTIONAL				0
#		define CASTOR_HAS_STDSMARTPTR				0
#		define CASTOR_HAS_STDARRAY					0
#		define CASTOR_HAS_DEFAULTED_FUNC			0
#		define CASTOR_HAS_DELETED_FUNC				0
#		define CASTOR_HAS_TR1SMARTPTR				0
#		define CASTOR_HAS_TR1ARRAY					0
#		define CASTOR_HAS_TR1FUNCTIONAL				0
#		define CASTOR_HAS_VARIADIC_TEMPLATES		0
#		define CASTOR_HAS_MAKE_UNIQUE				0
#	endif
#else
#	error "Yet unsupported compiler"
#endif

#endif
