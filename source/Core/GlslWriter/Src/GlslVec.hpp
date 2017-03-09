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
#ifndef ___GLSL_Vec_H___
#define ___GLSL_Vec_H___

#include "GlslSwizzle.hpp"

namespace GLSL
{
	template< typename ValueT >
	struct Vec2T
		: public Type
	{
		using my_type = ValueT;
		using my_vec2 = Vec2T< ValueT >;

		inline Vec2T();
		inline Vec2T( GlslWriter * p_writer, Castor::String const & p_name = Castor::String() );
		inline my_vec2 & operator=( my_vec2 const & p_rhs );
		template< typename RhsT > inline my_vec2 & operator=( RhsT const & p_rhs );
		template< typename IndexT > inline my_type operator[]( IndexT const & p_rhs );
		inline my_type operator[]( int const & p_rhs );

		GLSL_FIRST_SWIZZLE( my_vec2, my_type, x );
		GLSL_SWIZZLE( my_vec2, my_type, y );
		GLSL_SWIZZLE( my_vec2, my_type, r );
		GLSL_LAST_SWIZZLE( my_vec2, my_type, g );
		GLSL_FIRST_SWIZZLE( my_vec2, my_vec2, xy );
		GLSL_SWIZZLE( my_vec2, my_vec2, yx );
		GLSL_SWIZZLE( my_vec2, my_vec2, rg );
		GLSL_LAST_SWIZZLE( my_vec2, my_vec2, gr );
	};

	template< typename ValueT >
	struct Vec3T
		: public Type
	{
		using my_type = ValueT;
		using my_vec2 = Vec2T< ValueT >;
		using my_vec3 = Vec3T< ValueT >;

		inline Vec3T();
		inline Vec3T( GlslWriter * p_writer, Castor::String const & p_name = Castor::String() );
		inline my_vec3 & operator=( my_vec3 const & p_rhs );
		template< typename RhsT > inline my_vec3 & operator=( RhsT const & p_rhs );
		template< typename IndexT > inline my_type operator[]( IndexT const & p_rhs );
		inline my_type operator[]( int const & p_rhs );

		GLSL_FIRST_SWIZZLE( my_vec3, my_type, x );
		GLSL_SWIZZLE( my_vec3, my_type, y );
		GLSL_SWIZZLE( my_vec3, my_type, z );
		GLSL_SWIZZLE( my_vec3, my_type, r );
		GLSL_SWIZZLE( my_vec3, my_type, g );
		GLSL_LAST_SWIZZLE( my_vec3, my_type, b );
		GLSL_FIRST_SWIZZLE( my_vec3, my_vec2, xy );
		GLSL_SWIZZLE( my_vec3, my_vec2, xz );
		GLSL_SWIZZLE( my_vec3, my_vec2, yx );
		GLSL_SWIZZLE( my_vec3, my_vec2, yz );
		GLSL_SWIZZLE( my_vec3, my_vec2, zx );
		GLSL_SWIZZLE( my_vec3, my_vec2, zy );
		GLSL_SWIZZLE( my_vec3, my_vec2, rg );
		GLSL_SWIZZLE( my_vec3, my_vec2, rb );
		GLSL_SWIZZLE( my_vec3, my_vec2, gr );
		GLSL_SWIZZLE( my_vec3, my_vec2, gb );
		GLSL_SWIZZLE( my_vec3, my_vec2, br );
		GLSL_LAST_SWIZZLE( my_vec3, my_vec2, bg );
		GLSL_FIRST_SWIZZLE( my_vec3, my_vec3, xyz );
		GLSL_SWIZZLE( my_vec3, my_vec3, xzy );
		GLSL_SWIZZLE( my_vec3, my_vec3, yxz );
		GLSL_SWIZZLE( my_vec3, my_vec3, yzx );
		GLSL_SWIZZLE( my_vec3, my_vec3, zxy );
		GLSL_SWIZZLE( my_vec3, my_vec3, zyx );
		GLSL_SWIZZLE( my_vec3, my_vec3, rgb );
		GLSL_SWIZZLE( my_vec3, my_vec3, rbg );
		GLSL_SWIZZLE( my_vec3, my_vec3, grb );
		GLSL_SWIZZLE( my_vec3, my_vec3, gbr );
		GLSL_SWIZZLE( my_vec3, my_vec3, brg );
		GLSL_LAST_SWIZZLE( my_vec3, my_vec3, bgr );
	};

	template< typename ValueT >
	struct Vec4T
		: public Type
	{
		using my_type = ValueT;
		using my_vec2 = Vec2T< ValueT >;
		using my_vec3 = Vec3T< ValueT >;
		using my_vec4 = Vec4T< ValueT >;

		inline Vec4T();
		inline Vec4T( GlslWriter * p_writer, Castor::String const & p_name = Castor::String() );
		inline my_vec4 & operator=( my_vec4 const & p_rhs );
		template< typename RhsT > inline my_vec4 & operator=( RhsT const & p_rhs );
		template< typename IndexT > inline my_type operator[]( IndexT const & p_rhs );
		inline my_type operator[]( int const & p_rhs );

		GLSL_FIRST_SWIZZLE( my_vec4, my_type, x );
		GLSL_SWIZZLE( my_vec4, my_type, y );
		GLSL_SWIZZLE( my_vec4, my_type, z );
		GLSL_SWIZZLE( my_vec4, my_type, w );
		GLSL_SWIZZLE( my_vec4, my_type, r );
		GLSL_SWIZZLE( my_vec4, my_type, g );
		GLSL_SWIZZLE( my_vec4, my_type, b );
		GLSL_LAST_SWIZZLE( my_vec4, my_type, a );
		GLSL_FIRST_SWIZZLE( my_vec4, my_vec2, xy );
		GLSL_SWIZZLE( my_vec4, my_vec2, xz );
		GLSL_SWIZZLE( my_vec4, my_vec2, xw );
		GLSL_SWIZZLE( my_vec4, my_vec2, yx );
		GLSL_SWIZZLE( my_vec4, my_vec2, yz );
		GLSL_SWIZZLE( my_vec4, my_vec2, yw );
		GLSL_SWIZZLE( my_vec4, my_vec2, zx );
		GLSL_SWIZZLE( my_vec4, my_vec2, zy );
		GLSL_SWIZZLE( my_vec4, my_vec2, zw );
		GLSL_SWIZZLE( my_vec4, my_vec2, wx );
		GLSL_SWIZZLE( my_vec4, my_vec2, wy );
		GLSL_SWIZZLE( my_vec4, my_vec2, wz );
		GLSL_SWIZZLE( my_vec4, my_vec2, rg );
		GLSL_SWIZZLE( my_vec4, my_vec2, rb );
		GLSL_SWIZZLE( my_vec4, my_vec2, ra );
		GLSL_SWIZZLE( my_vec4, my_vec2, gr );
		GLSL_SWIZZLE( my_vec4, my_vec2, gb );
		GLSL_SWIZZLE( my_vec4, my_vec2, ga );
		GLSL_SWIZZLE( my_vec4, my_vec2, br );
		GLSL_SWIZZLE( my_vec4, my_vec2, bg );
		GLSL_SWIZZLE( my_vec4, my_vec2, ba );
		GLSL_SWIZZLE( my_vec4, my_vec2, ar );
		GLSL_SWIZZLE( my_vec4, my_vec2, ag );
		GLSL_LAST_SWIZZLE( my_vec4, my_vec2, ab );
		GLSL_FIRST_SWIZZLE( my_vec4, my_vec3, xyz );
		GLSL_SWIZZLE( my_vec4, my_vec3, xyw );
		GLSL_SWIZZLE( my_vec4, my_vec3, xzy );
		GLSL_SWIZZLE( my_vec4, my_vec3, xzw );
		GLSL_SWIZZLE( my_vec4, my_vec3, xwy );
		GLSL_SWIZZLE( my_vec4, my_vec3, xwz );
		GLSL_SWIZZLE( my_vec4, my_vec3, yxz );
		GLSL_SWIZZLE( my_vec4, my_vec3, yxw );
		GLSL_SWIZZLE( my_vec4, my_vec3, yzx );
		GLSL_SWIZZLE( my_vec4, my_vec3, yzw );
		GLSL_SWIZZLE( my_vec4, my_vec3, ywx );
		GLSL_SWIZZLE( my_vec4, my_vec3, ywz );
		GLSL_SWIZZLE( my_vec4, my_vec3, zxy );
		GLSL_SWIZZLE( my_vec4, my_vec3, zxw );
		GLSL_SWIZZLE( my_vec4, my_vec3, zyx );
		GLSL_SWIZZLE( my_vec4, my_vec3, zyw );
		GLSL_SWIZZLE( my_vec4, my_vec3, zwx );
		GLSL_SWIZZLE( my_vec4, my_vec3, zwy );
		GLSL_SWIZZLE( my_vec4, my_vec3, wxy );
		GLSL_SWIZZLE( my_vec4, my_vec3, wxz );
		GLSL_SWIZZLE( my_vec4, my_vec3, wyx );
		GLSL_SWIZZLE( my_vec4, my_vec3, wyz );
		GLSL_SWIZZLE( my_vec4, my_vec3, wzx );
		GLSL_SWIZZLE( my_vec4, my_vec3, wzy );
		GLSL_SWIZZLE( my_vec4, my_vec3, rgb );
		GLSL_SWIZZLE( my_vec4, my_vec3, rga );
		GLSL_SWIZZLE( my_vec4, my_vec3, rbg );
		GLSL_SWIZZLE( my_vec4, my_vec3, rba );
		GLSL_SWIZZLE( my_vec4, my_vec3, rag );
		GLSL_SWIZZLE( my_vec4, my_vec3, rab );
		GLSL_SWIZZLE( my_vec4, my_vec3, grb );
		GLSL_SWIZZLE( my_vec4, my_vec3, gra );
		GLSL_SWIZZLE( my_vec4, my_vec3, gbr );
		GLSL_SWIZZLE( my_vec4, my_vec3, gba );
		GLSL_SWIZZLE( my_vec4, my_vec3, gar );
		GLSL_SWIZZLE( my_vec4, my_vec3, gab );
		GLSL_SWIZZLE( my_vec4, my_vec3, brg );
		GLSL_SWIZZLE( my_vec4, my_vec3, bra );
		GLSL_SWIZZLE( my_vec4, my_vec3, bgr );
		GLSL_SWIZZLE( my_vec4, my_vec3, bga );
		GLSL_SWIZZLE( my_vec4, my_vec3, bar );
		GLSL_SWIZZLE( my_vec4, my_vec3, bag );
		GLSL_SWIZZLE( my_vec4, my_vec3, arg );
		GLSL_SWIZZLE( my_vec4, my_vec3, arb );
		GLSL_SWIZZLE( my_vec4, my_vec3, agr );
		GLSL_SWIZZLE( my_vec4, my_vec3, agb );
		GLSL_SWIZZLE( my_vec4, my_vec3, abr );
		GLSL_LAST_SWIZZLE( my_vec4, my_vec3, abg );
		GLSL_FIRST_SWIZZLE( my_vec4, my_vec4, xyzw );
		GLSL_SWIZZLE( my_vec4, my_vec4, xyww );
		GLSL_SWIZZLE( my_vec4, my_vec4, xywz );
		GLSL_SWIZZLE( my_vec4, my_vec4, xzyw );
		GLSL_SWIZZLE( my_vec4, my_vec4, xzwy );
		GLSL_SWIZZLE( my_vec4, my_vec4, xwyz );
		GLSL_SWIZZLE( my_vec4, my_vec4, xwzy );
		GLSL_SWIZZLE( my_vec4, my_vec4, yxzw );
		GLSL_SWIZZLE( my_vec4, my_vec4, yxwz );
		GLSL_SWIZZLE( my_vec4, my_vec4, yzxw );
		GLSL_SWIZZLE( my_vec4, my_vec4, yzwx );
		GLSL_SWIZZLE( my_vec4, my_vec4, ywxz );
		GLSL_SWIZZLE( my_vec4, my_vec4, ywzx );
		GLSL_SWIZZLE( my_vec4, my_vec4, zxyw );
		GLSL_SWIZZLE( my_vec4, my_vec4, zxwy );
		GLSL_SWIZZLE( my_vec4, my_vec4, zyxw );
		GLSL_SWIZZLE( my_vec4, my_vec4, zywx );
		GLSL_SWIZZLE( my_vec4, my_vec4, zwxy );
		GLSL_SWIZZLE( my_vec4, my_vec4, zwyx );
		GLSL_SWIZZLE( my_vec4, my_vec4, wxyz );
		GLSL_SWIZZLE( my_vec4, my_vec4, wxzy );
		GLSL_SWIZZLE( my_vec4, my_vec4, wyxz );
		GLSL_SWIZZLE( my_vec4, my_vec4, wyzx );
		GLSL_SWIZZLE( my_vec4, my_vec4, wzxy );
		GLSL_SWIZZLE( my_vec4, my_vec4, wzyx );
		GLSL_SWIZZLE( my_vec4, my_vec4, rgba );
		GLSL_SWIZZLE( my_vec4, my_vec4, rgab );
		GLSL_SWIZZLE( my_vec4, my_vec4, rbga );
		GLSL_SWIZZLE( my_vec4, my_vec4, rbag );
		GLSL_SWIZZLE( my_vec4, my_vec4, ragb );
		GLSL_SWIZZLE( my_vec4, my_vec4, rabg );
		GLSL_SWIZZLE( my_vec4, my_vec4, grba );
		GLSL_SWIZZLE( my_vec4, my_vec4, grab );
		GLSL_SWIZZLE( my_vec4, my_vec4, gbra );
		GLSL_SWIZZLE( my_vec4, my_vec4, gbar );
		GLSL_SWIZZLE( my_vec4, my_vec4, garb );
		GLSL_SWIZZLE( my_vec4, my_vec4, gabr );
		GLSL_SWIZZLE( my_vec4, my_vec4, brga );
		GLSL_SWIZZLE( my_vec4, my_vec4, brag );
		GLSL_SWIZZLE( my_vec4, my_vec4, bgra );
		GLSL_SWIZZLE( my_vec4, my_vec4, bgar );
		GLSL_SWIZZLE( my_vec4, my_vec4, barg );
		GLSL_SWIZZLE( my_vec4, my_vec4, bagr );
		GLSL_SWIZZLE( my_vec4, my_vec4, argb );
		GLSL_SWIZZLE( my_vec4, my_vec4, arbg );
		GLSL_SWIZZLE( my_vec4, my_vec4, agrb );
		GLSL_SWIZZLE( my_vec4, my_vec4, agbr );
		GLSL_SWIZZLE( my_vec4, my_vec4, abrg );
		GLSL_LAST_SWIZZLE( my_vec4, my_vec4, abgr );
	};
}

#include "GlslVec.inl"

#endif
