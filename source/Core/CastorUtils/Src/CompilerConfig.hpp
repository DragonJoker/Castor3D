/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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

#undef CASTOR_HAS_ALIGNAS

#if defined( _MSC_VER)

#	if !defined( VLD_AVAILABLE )
#		define _CRTDBG_MAP_ALLOC
#		include <stdlib.h>
#		include <crtdbg.h>
#	endif

#	if _MSC_VER < 1900
#		error "Your compiler is too old, consider an upgrade."
#	endif
#elif defined( __clang__)
#	if ( ! __has_feature(cxx_alignas)
		|| !__has_feature( cxx_nullptr )
		|| !__has_feature( cxx_defaulted_functions )
		|| !__has_feature( cxx_deleted_functions )
		|| !__has_feature( cxx_variadic_templates ) )
#		error "Your compiler is too old, consider an upgrade."
#	endif
#elif defined( __GNUG__)
#	define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#	if GCC_VERSION < 40900
#		error "Your compiler is too old, consider an upgrade."
#	endif
#elif defined( __BORLANDC__ )
#	warning "Theoretically supported compiler, but untested yet"
#	if __BORLANDC__ <= 0x621
#		error "Your compiler is too old, consider an upgrade."
#	endif
#else
#	error "Yet unsupported compiler"
#endif

#endif
