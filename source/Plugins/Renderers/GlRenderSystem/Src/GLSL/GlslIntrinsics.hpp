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
#ifndef ___GLSL_INTRINSICS_H___
#define ___GLSL_INTRINSICS_H___

#include "GlslFunctionParam.hpp"

namespace GlRender
{
	namespace GLSL
	{
		template< typename T > struct is_arithmetic_type : public std::false_type {};
		template<> struct is_arithmetic_type< Int > : public std::true_type {};
		template<> struct is_arithmetic_type< Float > : public std::true_type {};
		template<> struct is_arithmetic_type< Vec2 > : public std::true_type {};
		template<> struct is_arithmetic_type< Vec3 > : public std::true_type {};
		template<> struct is_arithmetic_type< Vec4 > : public std::true_type {};
		template<> struct is_arithmetic_type< IVec2 > : public std::true_type {};
		template<> struct is_arithmetic_type< IVec3 > : public std::true_type {};
		template<> struct is_arithmetic_type< IVec4 > : public std::true_type {};
		template<> struct is_arithmetic_type< Mat3 > : public std::true_type {};
		template<> struct is_arithmetic_type< Mat4 > : public std::true_type {};

		template< typename TypeA, typename TypeB, typename Enable = typename std::enable_if< is_arithmetic_type< TypeA >::value >::type >TypeA operator+( TypeA const & p_a, TypeB const & p_b );
		template< typename TypeA, typename TypeB, typename Enable = typename std::enable_if< is_arithmetic_type< TypeA >::value >::type >TypeA operator-( TypeA const & p_a, TypeB const & p_b );
		template< typename TypeA, typename TypeB, typename Enable = typename std::enable_if< is_arithmetic_type< TypeA >::value >::type >TypeA operator*( TypeA const & p_a, TypeB const & p_b );
		template< typename TypeA, typename TypeB, typename Enable = typename std::enable_if< is_arithmetic_type< TypeA >::value >::type >TypeA operator/( TypeA const & p_a, TypeB const & p_b );
		template< typename TypeA, typename TypeB, typename Enable = typename std::enable_if< is_arithmetic_type< TypeA >::value >::type >GlslBool operator==( TypeA const & p_a, TypeB const & p_b );
		template< typename TypeA, typename TypeB, typename Enable = typename std::enable_if< is_arithmetic_type< TypeA >::value >::type >GlslBool operator!=( TypeA const & p_a, TypeB const & p_b );
		template< typename TypeA, typename TypeB, typename Enable = typename std::enable_if< is_arithmetic_type< TypeA >::value >::type >GlslBool operator<( TypeA const & p_a, TypeB const & p_b );
		template< typename TypeA, typename TypeB, typename Enable = typename std::enable_if< is_arithmetic_type< TypeA >::value >::type >GlslBool operator<=( TypeA const & p_a, TypeB const & p_b );
		template< typename TypeA, typename TypeB, typename Enable = typename std::enable_if< is_arithmetic_type< TypeA >::value >::type >GlslBool operator>( TypeA const & p_a, TypeB const & p_b );
		template< typename TypeA, typename TypeB, typename Enable = typename std::enable_if< is_arithmetic_type< TypeA >::value >::type >GlslBool operator>=( TypeA const & p_a, TypeB const & p_b );

		C3D_Gl_API Vec4 operator*( Mat4 const & p_a, Vec4 const & p_b );
		C3D_Gl_API Vec3 operator*( Mat3 const & p_a, Vec3 const & p_b );
		C3D_Gl_API Float operator+( float p_a, Float const & p_b );
		C3D_Gl_API Float operator-( float p_a, Float const & p_b );
		C3D_Gl_API Float operator*( float p_a, Float const & p_b );
		C3D_Gl_API Float operator/( float p_a, Float const & p_b );
		C3D_Gl_API Int operator+( int p_a, Int const & p_b );
		C3D_Gl_API Int operator-( int p_a, Int const & p_b );
		C3D_Gl_API Int operator*( int p_a, Int const & p_b );
		C3D_Gl_API Int operator/( int p_a, Int const & p_b );

		template< typename ... Values > inline Vec2 vec2( Type const & p_value, Values const & ... p_values );
		template< typename ... Values > inline Vec3 vec3( Type const & p_value, Values const & ... p_values );
		template< typename ... Values > inline Vec4 vec4( Type const & p_value, Values const & ... p_values );
		template< typename ... Values > inline IVec2 ivec2( Type const & p_value, Values const & ... p_values );
		template< typename ... Values > inline IVec3 ivec3( Type const & p_value, Values const & ... p_values );
		template< typename ... Values > inline IVec4 ivec4( Type const & p_value, Values const & ... p_values );
		template< typename ... Values > inline Mat3 mat3( Type const & p_value, Values const & ... p_values );
		template< typename ... Values > inline Mat4 mat4( Type const & p_value, Values const & ... p_values );
		template< typename ... Values > inline Float dot( Type const & p_value, Values const & ... p_values );
		template< typename ... Values > inline Float inversesqrt( Expr const & p_value, Values const & ... p_values );
		template< typename ... Values > inline Float sqrt( Expr const & p_value, Values const & ... p_values );
		template< typename ... Values > inline Float pow( Expr const & p_value, Values const & ... p_values );
		template< typename Value, typename ... Values > inline Value cross( Value const & p_value, Values const & ... p_values );
		template< typename Value, typename ... Values > inline Value clamp( Value const & p_value, Values const & ... p_values );
		template< typename Value, typename ... Values > inline Value min( Value const & p_value, Values const & ... p_values );
		template< typename Value, typename ... Values > inline Value max( Value const & p_value, Values const & ... p_values );
		template< typename Value, typename ... Values > inline Value reflect( Type const & p_value, Values const & ... p_values );
		template< typename Value > inline Value neg( Value const & p_value );
		template< typename Input, typename Output > inline Output neg( Swizzle< Input, Output > const & p_value );
		template< typename Value > inline Value normalize( Value const & p_value );
		template< typename Input, typename Output > inline Output normalize( Swizzle< Input, Output > const & p_value );
		template< typename Value > inline Value transpose( Value const & p_value );
		template< typename Value > inline Value inverse( Value const & p_value );
		C3D_Gl_API Vec4 texture1D( Sampler1D const & p_sampler, Type const & p_value );
		C3D_Gl_API Vec4 texture2D( Sampler2D const & p_sampler, Type const & p_value );
		C3D_Gl_API Vec4 texture3D( Sampler3D const & p_sampler, Type const & p_value );
		C3D_Gl_API Vec4 texelFetch( SamplerBuffer const & p_sampler, Type const & p_value );
		C3D_Gl_API Vec4 texelFetch( Sampler1D const & p_sampler, Type const & p_value, Int const & p_modif );
		C3D_Gl_API Vec4 texelFetch( Sampler2D const & p_sampler, Type const & p_value, Int const & p_modif );
		C3D_Gl_API Vec4 texelFetch( Sampler3D const & p_sampler, Type const & p_value, Int const & p_modif );
		C3D_Gl_API Vec2 reflect( Vec2 const & p_a, Type const & p_b );
		C3D_Gl_API Vec3 reflect( Vec3 const & p_a, Type const & p_b );
		C3D_Gl_API Vec4 reflect( Vec4 const & p_a, Type const & p_b );
		C3D_Gl_API Float length( Type const & p_value );
		C3D_Gl_API Float radians( Type const & p_value );
		C3D_Gl_API Float cos( Type const & p_value );
		C3D_Gl_API Float sin( Type const & p_value );
		C3D_Gl_API Float tan( Type const & p_value );
	}
}

#include "GlslIntrinsics.inl"

#endif
