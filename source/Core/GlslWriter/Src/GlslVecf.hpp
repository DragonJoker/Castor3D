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
#ifndef ___GLSL_VECF_H___
#define ___GLSL_VECF_H___

#include "GlslSwizzle.hpp"

namespace GLSL
{
	struct Vec2
		: public Type
	{
		inline Vec2();
		inline Vec2( GlslWriter * p_writer, Castor::String const & p_name = Castor::String() );
		inline ~Vec2();
		inline Vec2 & operator=( Vec2 const & p_rhs );
		template< typename T > inline Vec2 & operator=( T const & p_rhs );
		template< typename T > inline Vec2 & operator[]( T const & p_rhs );

		GLSL_FIRST_SWIZZLE( Vec2, Float, x );
		GLSL_SWIZZLE( Vec2, Float, y );
		GLSL_SWIZZLE( Vec2, Float, r );
		GLSL_LAST_SWIZZLE( Vec2, Float, g );
		GLSL_FIRST_SWIZZLE( Vec2, Vec2, xy );
		GLSL_SWIZZLE( Vec2, Vec2, yx );
		GLSL_SWIZZLE( Vec2, Vec2, rg );
		GLSL_LAST_SWIZZLE( Vec2, Vec2, gr );
	};

	struct Vec3
		: public Type
	{
		inline Vec3();
		inline Vec3( GlslWriter * p_writer, Castor::String const & p_name = Castor::String() );
		inline ~Vec3();
		inline Vec3 & operator=( Vec3 const & p_rhs );
		template< typename T > inline Vec3 & operator=( T const & p_rhs );
		template< typename T > inline Vec3 & operator[]( T const & p_rhs );

		GLSL_FIRST_SWIZZLE( Vec3, Float, x );
		GLSL_SWIZZLE( Vec3, Float, y );
		GLSL_SWIZZLE( Vec3, Float, z );
		GLSL_SWIZZLE( Vec3, Float, r );
		GLSL_SWIZZLE( Vec3, Float, g );
		GLSL_LAST_SWIZZLE( Vec3, Float, b );
		GLSL_FIRST_SWIZZLE( Vec3, Vec2, xy );
		GLSL_SWIZZLE( Vec3, Vec2, xz );
		GLSL_SWIZZLE( Vec3, Vec2, yx );
		GLSL_SWIZZLE( Vec3, Vec2, yz );
		GLSL_SWIZZLE( Vec3, Vec2, zx );
		GLSL_SWIZZLE( Vec3, Vec2, zy );
		GLSL_SWIZZLE( Vec3, Vec2, rg );
		GLSL_SWIZZLE( Vec3, Vec2, rb );
		GLSL_SWIZZLE( Vec3, Vec2, gr );
		GLSL_SWIZZLE( Vec3, Vec2, gb );
		GLSL_SWIZZLE( Vec3, Vec2, br );
		GLSL_LAST_SWIZZLE( Vec3, Vec2, bg );
		GLSL_FIRST_SWIZZLE( Vec3, Vec3, xyz );
		GLSL_SWIZZLE( Vec3, Vec3, xzy );
		GLSL_SWIZZLE( Vec3, Vec3, yxz );
		GLSL_SWIZZLE( Vec3, Vec3, yzx );
		GLSL_SWIZZLE( Vec3, Vec3, zxy );
		GLSL_SWIZZLE( Vec3, Vec3, zyx );
		GLSL_SWIZZLE( Vec3, Vec3, rgb );
		GLSL_SWIZZLE( Vec3, Vec3, rbg );
		GLSL_SWIZZLE( Vec3, Vec3, grb );
		GLSL_SWIZZLE( Vec3, Vec3, gbr );
		GLSL_SWIZZLE( Vec3, Vec3, brg );
		GLSL_LAST_SWIZZLE( Vec3, Vec3, bgr );
	};

	struct Vec4
		: public Type
	{
		inline Vec4();
		inline Vec4( GlslWriter * p_writer, Castor::String const & p_name = Castor::String() );
		inline ~Vec4();
		inline Vec4 & operator=( Vec4 const & p_rhs );
		template< typename T > inline Vec4 & operator=( T const & p_rhs );
		template< typename T > inline Vec4 & operator[]( T const & p_rhs );

		GLSL_FIRST_SWIZZLE( Vec4, Float, x );
		GLSL_SWIZZLE( Vec4, Float, y );
		GLSL_SWIZZLE( Vec4, Float, z );
		GLSL_SWIZZLE( Vec4, Float, w );
		GLSL_SWIZZLE( Vec4, Float, r );
		GLSL_SWIZZLE( Vec4, Float, g );
		GLSL_SWIZZLE( Vec4, Float, b );
		GLSL_LAST_SWIZZLE( Vec4, Float, a );
		GLSL_FIRST_SWIZZLE( Vec4, Vec2, xy );
		GLSL_SWIZZLE( Vec4, Vec2, xz );
		GLSL_SWIZZLE( Vec4, Vec2, xw );
		GLSL_SWIZZLE( Vec4, Vec2, yx );
		GLSL_SWIZZLE( Vec4, Vec2, yz );
		GLSL_SWIZZLE( Vec4, Vec2, yw );
		GLSL_SWIZZLE( Vec4, Vec2, zx );
		GLSL_SWIZZLE( Vec4, Vec2, zy );
		GLSL_SWIZZLE( Vec4, Vec2, zw );
		GLSL_SWIZZLE( Vec4, Vec2, wx );
		GLSL_SWIZZLE( Vec4, Vec2, wy );
		GLSL_SWIZZLE( Vec4, Vec2, wz );
		GLSL_SWIZZLE( Vec4, Vec2, rg );
		GLSL_SWIZZLE( Vec4, Vec2, rb );
		GLSL_SWIZZLE( Vec4, Vec2, ra );
		GLSL_SWIZZLE( Vec4, Vec2, gr );
		GLSL_SWIZZLE( Vec4, Vec2, gb );
		GLSL_SWIZZLE( Vec4, Vec2, ga );
		GLSL_SWIZZLE( Vec4, Vec2, br );
		GLSL_SWIZZLE( Vec4, Vec2, bg );
		GLSL_SWIZZLE( Vec4, Vec2, ba );
		GLSL_SWIZZLE( Vec4, Vec2, ar );
		GLSL_SWIZZLE( Vec4, Vec2, ag );
		GLSL_LAST_SWIZZLE( Vec4, Vec2, ab );
		GLSL_FIRST_SWIZZLE( Vec4, Vec3, xyz );
		GLSL_SWIZZLE( Vec4, Vec3, xyw );
		GLSL_SWIZZLE( Vec4, Vec3, xzy );
		GLSL_SWIZZLE( Vec4, Vec3, xzw );
		GLSL_SWIZZLE( Vec4, Vec3, xwy );
		GLSL_SWIZZLE( Vec4, Vec3, xwz );
		GLSL_SWIZZLE( Vec4, Vec3, yxz );
		GLSL_SWIZZLE( Vec4, Vec3, yxw );
		GLSL_SWIZZLE( Vec4, Vec3, yzx );
		GLSL_SWIZZLE( Vec4, Vec3, yzw );
		GLSL_SWIZZLE( Vec4, Vec3, ywx );
		GLSL_SWIZZLE( Vec4, Vec3, ywz );
		GLSL_SWIZZLE( Vec4, Vec3, zxy );
		GLSL_SWIZZLE( Vec4, Vec3, zxw );
		GLSL_SWIZZLE( Vec4, Vec3, zyx );
		GLSL_SWIZZLE( Vec4, Vec3, zyw );
		GLSL_SWIZZLE( Vec4, Vec3, zwx );
		GLSL_SWIZZLE( Vec4, Vec3, zwy );
		GLSL_SWIZZLE( Vec4, Vec3, wxy );
		GLSL_SWIZZLE( Vec4, Vec3, wxz );
		GLSL_SWIZZLE( Vec4, Vec3, wyx );
		GLSL_SWIZZLE( Vec4, Vec3, wyz );
		GLSL_SWIZZLE( Vec4, Vec3, wzx );
		GLSL_SWIZZLE( Vec4, Vec3, wzy );
		GLSL_SWIZZLE( Vec4, Vec3, rgb );
		GLSL_SWIZZLE( Vec4, Vec3, rga );
		GLSL_SWIZZLE( Vec4, Vec3, rbg );
		GLSL_SWIZZLE( Vec4, Vec3, rba );
		GLSL_SWIZZLE( Vec4, Vec3, rag );
		GLSL_SWIZZLE( Vec4, Vec3, rab );
		GLSL_SWIZZLE( Vec4, Vec3, grb );
		GLSL_SWIZZLE( Vec4, Vec3, gra );
		GLSL_SWIZZLE( Vec4, Vec3, gbr );
		GLSL_SWIZZLE( Vec4, Vec3, gba );
		GLSL_SWIZZLE( Vec4, Vec3, gar );
		GLSL_SWIZZLE( Vec4, Vec3, gab );
		GLSL_SWIZZLE( Vec4, Vec3, brg );
		GLSL_SWIZZLE( Vec4, Vec3, bra );
		GLSL_SWIZZLE( Vec4, Vec3, bgr );
		GLSL_SWIZZLE( Vec4, Vec3, bga );
		GLSL_SWIZZLE( Vec4, Vec3, bar );
		GLSL_SWIZZLE( Vec4, Vec3, bag );
		GLSL_SWIZZLE( Vec4, Vec3, arg );
		GLSL_SWIZZLE( Vec4, Vec3, arb );
		GLSL_SWIZZLE( Vec4, Vec3, agr );
		GLSL_SWIZZLE( Vec4, Vec3, agb );
		GLSL_SWIZZLE( Vec4, Vec3, abr );
		GLSL_LAST_SWIZZLE( Vec4, Vec3, abg );
		GLSL_FIRST_SWIZZLE( Vec4, Vec4, xyzw );
		GLSL_SWIZZLE( Vec4, Vec4, xyww );
		GLSL_SWIZZLE( Vec4, Vec4, xywz );
		GLSL_SWIZZLE( Vec4, Vec4, xzyw );
		GLSL_SWIZZLE( Vec4, Vec4, xzwy );
		GLSL_SWIZZLE( Vec4, Vec4, xwyz );
		GLSL_SWIZZLE( Vec4, Vec4, xwzy );
		GLSL_SWIZZLE( Vec4, Vec4, yxzw );
		GLSL_SWIZZLE( Vec4, Vec4, yxwz );
		GLSL_SWIZZLE( Vec4, Vec4, yzxw );
		GLSL_SWIZZLE( Vec4, Vec4, yzwx );
		GLSL_SWIZZLE( Vec4, Vec4, ywxz );
		GLSL_SWIZZLE( Vec4, Vec4, ywzx );
		GLSL_SWIZZLE( Vec4, Vec4, zxyw );
		GLSL_SWIZZLE( Vec4, Vec4, zxwy );
		GLSL_SWIZZLE( Vec4, Vec4, zyxw );
		GLSL_SWIZZLE( Vec4, Vec4, zywx );
		GLSL_SWIZZLE( Vec4, Vec4, zwxy );
		GLSL_SWIZZLE( Vec4, Vec4, zwyx );
		GLSL_SWIZZLE( Vec4, Vec4, wxyz );
		GLSL_SWIZZLE( Vec4, Vec4, wxzy );
		GLSL_SWIZZLE( Vec4, Vec4, wyxz );
		GLSL_SWIZZLE( Vec4, Vec4, wyzx );
		GLSL_SWIZZLE( Vec4, Vec4, wzxy );
		GLSL_SWIZZLE( Vec4, Vec4, wzyx );
		GLSL_SWIZZLE( Vec4, Vec4, rgba );
		GLSL_SWIZZLE( Vec4, Vec4, rgab );
		GLSL_SWIZZLE( Vec4, Vec4, rbga );
		GLSL_SWIZZLE( Vec4, Vec4, rbag );
		GLSL_SWIZZLE( Vec4, Vec4, ragb );
		GLSL_SWIZZLE( Vec4, Vec4, rabg );
		GLSL_SWIZZLE( Vec4, Vec4, grba );
		GLSL_SWIZZLE( Vec4, Vec4, grab );
		GLSL_SWIZZLE( Vec4, Vec4, gbra );
		GLSL_SWIZZLE( Vec4, Vec4, gbar );
		GLSL_SWIZZLE( Vec4, Vec4, garb );
		GLSL_SWIZZLE( Vec4, Vec4, gabr );
		GLSL_SWIZZLE( Vec4, Vec4, brga );
		GLSL_SWIZZLE( Vec4, Vec4, brag );
		GLSL_SWIZZLE( Vec4, Vec4, bgra );
		GLSL_SWIZZLE( Vec4, Vec4, bgar );
		GLSL_SWIZZLE( Vec4, Vec4, barg );
		GLSL_SWIZZLE( Vec4, Vec4, bagr );
		GLSL_SWIZZLE( Vec4, Vec4, argb );
		GLSL_SWIZZLE( Vec4, Vec4, arbg );
		GLSL_SWIZZLE( Vec4, Vec4, agrb );
		GLSL_SWIZZLE( Vec4, Vec4, agbr );
		GLSL_SWIZZLE( Vec4, Vec4, abrg );
		GLSL_LAST_SWIZZLE( Vec4, Vec4, abgr );
	};
}

#include "GlslVecf.inl"

#endif
