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
#ifndef ___GLSL_VECI_H___
#define ___GLSL_VECI_H___

#include "GlslVecf.hpp"

namespace GLSL
{
	struct IVec2
		: public Type
	{
		inline IVec2();
		inline IVec2( GlslWriter * p_writer, Castor::String const & p_name = Castor::String() );
		inline ~IVec2();
		inline IVec2 & operator=( IVec2 const & p_rhs );
		template< typename T > inline IVec2 & operator=( T const & p_rhs );
		template< typename T > inline Int operator[]( T const & p_rhs );
		inline Int operator[]( int const & p_rhs );

		GLSL_FIRST_SWIZZLE( IVec2, Int, x );
		GLSL_SWIZZLE( IVec2, Int, y );
		GLSL_SWIZZLE( IVec2, Int, r );
		GLSL_LAST_SWIZZLE( IVec2, Int, g );
		GLSL_FIRST_SWIZZLE( IVec2, IVec2, xy );
		GLSL_SWIZZLE( IVec2, IVec2, yx );
		GLSL_SWIZZLE( IVec2, IVec2, rg );
		GLSL_LAST_SWIZZLE( IVec2, IVec2, gr );
	};

	struct IVec3
		: public Type
	{
		inline IVec3();
		inline IVec3( GlslWriter * p_writer, Castor::String const & p_name = Castor::String() );
		inline ~IVec3();
		inline IVec3 & operator=( IVec3 const & p_rhs );
		template< typename T > inline IVec3 & operator=( T const & p_rhs );
		template< typename T > inline Int operator[]( T const & p_rhs );
		inline Int operator[]( int const & p_rhs );

		GLSL_FIRST_SWIZZLE( IVec3, Int, x );
		GLSL_SWIZZLE( IVec3, Int, y );
		GLSL_SWIZZLE( IVec3, Int, z );
		GLSL_SWIZZLE( IVec3, Int, r );
		GLSL_SWIZZLE( IVec3, Int, g );
		GLSL_LAST_SWIZZLE( IVec3, Int, b );
		GLSL_FIRST_SWIZZLE( IVec3, IVec2, xy );
		GLSL_SWIZZLE( IVec3, IVec2, xz );
		GLSL_SWIZZLE( IVec3, IVec2, yx );
		GLSL_SWIZZLE( IVec3, IVec2, yz );
		GLSL_SWIZZLE( IVec3, IVec2, zx );
		GLSL_SWIZZLE( IVec3, IVec2, zy );
		GLSL_SWIZZLE( IVec3, IVec2, rg );
		GLSL_SWIZZLE( IVec3, IVec2, rb );
		GLSL_SWIZZLE( IVec3, IVec2, gr );
		GLSL_SWIZZLE( IVec3, IVec2, gb );
		GLSL_SWIZZLE( IVec3, IVec2, br );
		GLSL_LAST_SWIZZLE( IVec3, IVec2, bg );
		GLSL_FIRST_SWIZZLE( IVec3, IVec3, xyz );
		GLSL_SWIZZLE( IVec3, IVec3, xzy );
		GLSL_SWIZZLE( IVec3, IVec3, yxz );
		GLSL_SWIZZLE( IVec3, IVec3, yzx );
		GLSL_SWIZZLE( IVec3, IVec3, zxy );
		GLSL_SWIZZLE( IVec3, IVec3, zyx );
		GLSL_SWIZZLE( IVec3, IVec3, rgb );
		GLSL_SWIZZLE( IVec3, IVec3, rbg );
		GLSL_SWIZZLE( IVec3, IVec3, grb );
		GLSL_SWIZZLE( IVec3, IVec3, gbr );
		GLSL_SWIZZLE( IVec3, IVec3, brg );
		GLSL_LAST_SWIZZLE( IVec3, IVec3, bgr );
	};

	struct IVec4
		: public Type
	{
		inline IVec4();
		inline IVec4( GlslWriter * p_writer, Castor::String const & p_name = Castor::String() );
		inline ~IVec4();
		inline IVec4 & operator=( IVec4 const & p_rhs );
		template< typename T > inline IVec4 & operator=( T const & p_rhs );
		template< typename T > inline Int operator[]( T const & p_rhs );
		inline Int operator[]( int const & p_rhs );

		GLSL_FIRST_SWIZZLE( IVec4, Int, x );
		GLSL_SWIZZLE( IVec4, Int, y );
		GLSL_SWIZZLE( IVec4, Int, z );
		GLSL_SWIZZLE( IVec4, Int, w );
		GLSL_SWIZZLE( IVec4, Int, r );
		GLSL_SWIZZLE( IVec4, Int, g );
		GLSL_SWIZZLE( IVec4, Int, b );
		GLSL_LAST_SWIZZLE( IVec4, Int, a );
		GLSL_FIRST_SWIZZLE( IVec4, IVec2, xy );
		GLSL_SWIZZLE( IVec4, IVec2, xz );
		GLSL_SWIZZLE( IVec4, IVec2, xw );
		GLSL_SWIZZLE( IVec4, IVec2, yx );
		GLSL_SWIZZLE( IVec4, IVec2, yz );
		GLSL_SWIZZLE( IVec4, IVec2, yw );
		GLSL_SWIZZLE( IVec4, IVec2, zx );
		GLSL_SWIZZLE( IVec4, IVec2, zy );
		GLSL_SWIZZLE( IVec4, IVec2, zw );
		GLSL_SWIZZLE( IVec4, IVec2, wx );
		GLSL_SWIZZLE( IVec4, IVec2, wy );
		GLSL_SWIZZLE( IVec4, IVec2, wz );
		GLSL_SWIZZLE( IVec4, IVec2, rg );
		GLSL_SWIZZLE( IVec4, IVec2, rb );
		GLSL_SWIZZLE( IVec4, IVec2, ra );
		GLSL_SWIZZLE( IVec4, IVec2, gr );
		GLSL_SWIZZLE( IVec4, IVec2, gb );
		GLSL_SWIZZLE( IVec4, IVec2, ga );
		GLSL_SWIZZLE( IVec4, IVec2, br );
		GLSL_SWIZZLE( IVec4, IVec2, bg );
		GLSL_SWIZZLE( IVec4, IVec2, ba );
		GLSL_SWIZZLE( IVec4, IVec2, ar );
		GLSL_SWIZZLE( IVec4, IVec2, ag );
		GLSL_LAST_SWIZZLE( IVec4, IVec2, ab );
		GLSL_FIRST_SWIZZLE( IVec4, IVec3, xyz );
		GLSL_SWIZZLE( IVec4, IVec3, xyw );
		GLSL_SWIZZLE( IVec4, IVec3, xzy );
		GLSL_SWIZZLE( IVec4, IVec3, xzw );
		GLSL_SWIZZLE( IVec4, IVec3, xwy );
		GLSL_SWIZZLE( IVec4, IVec3, xwz );
		GLSL_SWIZZLE( IVec4, IVec3, yxz );
		GLSL_SWIZZLE( IVec4, IVec3, yxw );
		GLSL_SWIZZLE( IVec4, IVec3, yzx );
		GLSL_SWIZZLE( IVec4, IVec3, yzw );
		GLSL_SWIZZLE( IVec4, IVec3, ywx );
		GLSL_SWIZZLE( IVec4, IVec3, ywz );
		GLSL_SWIZZLE( IVec4, IVec3, zxy );
		GLSL_SWIZZLE( IVec4, IVec3, zxw );
		GLSL_SWIZZLE( IVec4, IVec3, zyx );
		GLSL_SWIZZLE( IVec4, IVec3, zyw );
		GLSL_SWIZZLE( IVec4, IVec3, zwx );
		GLSL_SWIZZLE( IVec4, IVec3, zwy );
		GLSL_SWIZZLE( IVec4, IVec3, wxy );
		GLSL_SWIZZLE( IVec4, IVec3, wxz );
		GLSL_SWIZZLE( IVec4, IVec3, wyx );
		GLSL_SWIZZLE( IVec4, IVec3, wyz );
		GLSL_SWIZZLE( IVec4, IVec3, wzx );
		GLSL_SWIZZLE( IVec4, IVec3, wzy );
		GLSL_SWIZZLE( IVec4, IVec3, rgb );
		GLSL_SWIZZLE( IVec4, IVec3, rga );
		GLSL_SWIZZLE( IVec4, IVec3, rbg );
		GLSL_SWIZZLE( IVec4, IVec3, rba );
		GLSL_SWIZZLE( IVec4, IVec3, rag );
		GLSL_SWIZZLE( IVec4, IVec3, rab );
		GLSL_SWIZZLE( IVec4, IVec3, grb );
		GLSL_SWIZZLE( IVec4, IVec3, gra );
		GLSL_SWIZZLE( IVec4, IVec3, gbr );
		GLSL_SWIZZLE( IVec4, IVec3, gba );
		GLSL_SWIZZLE( IVec4, IVec3, gar );
		GLSL_SWIZZLE( IVec4, IVec3, gab );
		GLSL_SWIZZLE( IVec4, IVec3, brg );
		GLSL_SWIZZLE( IVec4, IVec3, bra );
		GLSL_SWIZZLE( IVec4, IVec3, bgr );
		GLSL_SWIZZLE( IVec4, IVec3, bga );
		GLSL_SWIZZLE( IVec4, IVec3, bar );
		GLSL_SWIZZLE( IVec4, IVec3, bag );
		GLSL_SWIZZLE( IVec4, IVec3, arg );
		GLSL_SWIZZLE( IVec4, IVec3, arb );
		GLSL_SWIZZLE( IVec4, IVec3, agr );
		GLSL_SWIZZLE( IVec4, IVec3, agb );
		GLSL_SWIZZLE( IVec4, IVec3, abr );
		GLSL_LAST_SWIZZLE( IVec4, IVec3, abg );
		GLSL_FIRST_SWIZZLE( IVec4, IVec4, xyzw );
		GLSL_SWIZZLE( IVec4, IVec4, xyww );
		GLSL_SWIZZLE( IVec4, IVec4, xywz );
		GLSL_SWIZZLE( IVec4, IVec4, xzyw );
		GLSL_SWIZZLE( IVec4, IVec4, xzwy );
		GLSL_SWIZZLE( IVec4, IVec4, xwyz );
		GLSL_SWIZZLE( IVec4, IVec4, xwzy );
		GLSL_SWIZZLE( IVec4, IVec4, yxzw );
		GLSL_SWIZZLE( IVec4, IVec4, yxwz );
		GLSL_SWIZZLE( IVec4, IVec4, yzxw );
		GLSL_SWIZZLE( IVec4, IVec4, yzwx );
		GLSL_SWIZZLE( IVec4, IVec4, ywxz );
		GLSL_SWIZZLE( IVec4, IVec4, ywzx );
		GLSL_SWIZZLE( IVec4, IVec4, zxyw );
		GLSL_SWIZZLE( IVec4, IVec4, zxwy );
		GLSL_SWIZZLE( IVec4, IVec4, zyxw );
		GLSL_SWIZZLE( IVec4, IVec4, zywx );
		GLSL_SWIZZLE( IVec4, IVec4, zwxy );
		GLSL_SWIZZLE( IVec4, IVec4, zwyx );
		GLSL_SWIZZLE( IVec4, IVec4, wxyz );
		GLSL_SWIZZLE( IVec4, IVec4, wxzy );
		GLSL_SWIZZLE( IVec4, IVec4, wyxz );
		GLSL_SWIZZLE( IVec4, IVec4, wyzx );
		GLSL_SWIZZLE( IVec4, IVec4, wzxy );
		GLSL_SWIZZLE( IVec4, IVec4, wzyx );
		GLSL_SWIZZLE( IVec4, IVec4, rgba );
		GLSL_SWIZZLE( IVec4, IVec4, rgab );
		GLSL_SWIZZLE( IVec4, IVec4, rbga );
		GLSL_SWIZZLE( IVec4, IVec4, rbag );
		GLSL_SWIZZLE( IVec4, IVec4, ragb );
		GLSL_SWIZZLE( IVec4, IVec4, rabg );
		GLSL_SWIZZLE( IVec4, IVec4, grba );
		GLSL_SWIZZLE( IVec4, IVec4, grab );
		GLSL_SWIZZLE( IVec4, IVec4, gbra );
		GLSL_SWIZZLE( IVec4, IVec4, gbar );
		GLSL_SWIZZLE( IVec4, IVec4, garb );
		GLSL_SWIZZLE( IVec4, IVec4, gabr );
		GLSL_SWIZZLE( IVec4, IVec4, brga );
		GLSL_SWIZZLE( IVec4, IVec4, brag );
		GLSL_SWIZZLE( IVec4, IVec4, bgra );
		GLSL_SWIZZLE( IVec4, IVec4, bgar );
		GLSL_SWIZZLE( IVec4, IVec4, barg );
		GLSL_SWIZZLE( IVec4, IVec4, bagr );
		GLSL_SWIZZLE( IVec4, IVec4, argb );
		GLSL_SWIZZLE( IVec4, IVec4, arbg );
		GLSL_SWIZZLE( IVec4, IVec4, agrb );
		GLSL_SWIZZLE( IVec4, IVec4, agbr );
		GLSL_SWIZZLE( IVec4, IVec4, abrg );
		GLSL_LAST_SWIZZLE( IVec4, IVec4, abgr );
	};
}

#include "GlslVeci.inl"

#endif
