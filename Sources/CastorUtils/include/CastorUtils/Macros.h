/*
This source file is part of Castor3D (http://dragonjoker.co.cc

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___MACROS_H___
#define ___MACROS_H___

/*
	Usefull macros :

	d_debug_only : [debug helper] this code will only exists in the debug build
	d_debug_assert : [debug helper] this assert only exists in debug build

	d_no_default : [debug helper] use when all the cases possibles for a switch have been used.
					Produces an error if somehow the code reaches the default

	d_no_throw : [compile/speed/security] indicates that a functino will no throw an exception ( void func () d_no_throw {code}
					functions called within the destructor of a class need d_no_throw

	d_single_inheritance: [compile] msvc only, use on pointers to predeclared classes.Compilation optimisation.
	d_multiple_inheritance : [compile] msvc only, use on pointers to predeclared classes.Compilation optimisation.
	d_virtual_inheritance : [compile] msvc only, use on pointers to predeclared classes.Compilation optimisation.

	d_abstract : [readability / compile] use on an abstract class. Can also be used on a class that will never use virtuals functions.Compilation optimisation.
											Warning : no vtable is not checked by the compiler. If you instanciate an instance of a novtable class, it's probablly gonna crash.
	d_no_globals : [execute/compile] complex, tells a function that it will not use any global aliases.
	d_no_inline : [compile] tells the compiler not to inline this function
	d_no_return : [compile] tells the compiler that the function preceding this does not return ever, thus prevents a warning.
	d_unique_ptr : [compile] the function will return a new pointer, that will never be aliases by anything else at that point.

	d_interface : [security] msvc only, usage : "d_interface blah{private:whatever}"
								d_interface tells the compiler that this class is a pure interface : public pure virtual functions only.

	d_hint : [compile] d_hint(0) tells the compiler that the code following is to be removed, since it won't be reached ever.
	d_aligned : [memory] used to memory-align a class or struct :  class blah d_align(8){};

	d_unique_global : [linker] with this, a global variable can be defined in multiple files.The linker will only keep one and not generate an error.

	d_std_call , d_fast_call , d_cdecl_call : [execute] the 3 different calling conventions for functions.

	d_forceinline : [compile] forces a function to be inlined.

	d_naked : [compile] the compiler will not create the epilog / prolog for this function.Use only for inline asm functions.

*/


#ifndef ___MUSE_DEBUG___
#	define d_debug_only
#	define d_debug_assert( X)
#else
#	define d_debug_only if( 0)
#	define d_debug_assert( X) Assert( X)
#endif

#ifndef __FUNCTION__
#	define __FUNCTION__ ""
#endif

#define d_unreachable_code d_debug_assert( 0); d_hint( 0)
#define d_no_default default : d_unreachable_code
#define d_no_throw throw()
#define d_coucou std::cout "Coucou" << __COUNTER__ << " : " << __FUNCTION__ << " @ line " << __LINE__ << std::endl

#ifdef _MSC_VER
#	define d_single_inheritance			__single_inheritance
#	define d_multiple_inheritance		__multiple_inheritance
#	define d_virtual_inheritance		__virtual_inheritance
#	define d_abstract					__declspec( novtable)
#	define d_no_globals					__declspec( noalias)
#	define d_no_inline					__declspec( noinline)
#	define d_no_return					__declspec( noreturn)
#	define d_unique_ptr					__restrict
#	define d_interface					__interface
#	define d_hint( p_hint)				__assume( ( p_hint))
#	define d_aligned( p_size)			__declspec( align( p_size))
#	define d_unique_global				__declspec( selectany)
#	define d_std_call					__stdcall
#	define d_fast_call					__fastcall
#	define d_cdecl_call					__cdecl
#	define d_forceinline				__forceinline
#	define d_naked						__declspec( naked)
#	define d_no_side_effect				__attribute__( ( const))
#	define d_no_local_side_effect		__attribute__( ( pure))
#	define d_nonnull( X)				__attribute__( ( nonnull( X)))
#	define d_critical					__attribute__( ( hot))
#	define d_unlikely					__attribute__( ( cold))
#elif __GNUG__
#	define d_single_inheritance
#	define d_multiple_inheritance
#	define d_virtual_inheritance
#	define d_abstract
#	define d_no_globals
#	define d_no_inline					__attribute__( ( noinline))
#	define d_no_return					__attribute__( ( noreturn))
#	define d_unique_ptr
#	define d_interface					class
#	define d_hint( p_hint)
#	define d_aligned( p_size)			__attribute__( ( aligned( p_size)))
#	define d_unique_global				__attribute__( ( weak))
#	define d_std_call					__attribute__( ( stdcall))
#	define d_fast_call					__attribute__( ( fastcall))
#	define d_cdecl_call					__attribute__( ( cdecl))
#	define d_forceinline				__attribute__( ( always_inline))
#	define d_naked						__attribute__( ( naked))
#	define d_no_side_effect				__attribute__( ( const))
#	define d_no_local_side_effect		__attribute__( ( pure))
#	define d_nonnull( X)				__attribute__( ( nonnull( X)))
#	define d_critical					__attribute__( ( hot))
#	define d_unlikely					__attribute__( ( cold))
#else
#	define d_single_inheritance
#	define d_multiple_inheritance
#	define d_virtual_inheritance
#	define d_abstract
#	define d_no_globals
#	define d_no_inline
#	define d_no_return
#	define d_unique_ptr
#	define d_interface
#	define d_hint( p_hint)
#	define d_aligned( p_size)
#	define d_unique_global
#	define d_std_call
#	define d_fast_call
#	define d_cdecl_call
#	define d_forceinline
#	define d_naked
#endif

#endif
