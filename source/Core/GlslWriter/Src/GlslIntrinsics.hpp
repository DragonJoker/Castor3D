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
#ifndef ___GLSL_INTRINSICS_H___
#define ___GLSL_INTRINSICS_H___

#include "GlslFunctionParam.hpp"

namespace GLSL
{
	template< typename T > struct IsArithmeticType : public std::false_type {};
	template<> struct IsArithmeticType< Int > : public std::true_type {};
	template<> struct IsArithmeticType< UInt > : public std::true_type {};
	template<> struct IsArithmeticType< Float > : public std::true_type {};
	template<> struct IsArithmeticType< Vec2 > : public std::true_type {};
	template<> struct IsArithmeticType< Vec3 > : public std::true_type {};
	template<> struct IsArithmeticType< Vec4 > : public std::true_type {};
	template<> struct IsArithmeticType< IVec2 > : public std::true_type {};
	template<> struct IsArithmeticType< IVec3 > : public std::true_type {};
	template<> struct IsArithmeticType< IVec4 > : public std::true_type {};
	template<> struct IsArithmeticType< Mat2 > : public std::true_type {};
	template<> struct IsArithmeticType< Mat3 > : public std::true_type {};
	template<> struct IsArithmeticType< Mat4 > : public std::true_type{};

#	define GLSL_DECLARE_OPERATOR( RetType, LhsType, RhsType, Operator )\
	template< typename LhsType, typename RhsType, typename Enable = typename std::enable_if< IsArithmeticType< LhsType >::value >::type >\
	RetType operator Operator( LhsType const & p_lhs, RhsType const & p_rhs );\
	template< typename LhsType, typename RhsType, typename Enable = typename std::enable_if< IsArithmeticType< LhsType >::value >::type >\
	RetType operator Operator( LhsType const & p_lhs, InParam< RhsType > const & p_rhs );\
	template< typename LhsType, typename RhsType, typename Enable = typename std::enable_if< IsArithmeticType< LhsType >::value >::type >\
	RetType operator Operator( LhsType const & p_lhs, InOutParam< RhsType > const & p_rhs );\
	template< typename LhsType, typename RhsType, typename Enable = typename std::enable_if< IsArithmeticType< LhsType >::value >::type >\
	RetType operator Operator( LhsType const & p_lhs, OutParam< RhsType > const & p_rhs );\
	template< typename LhsType, typename RhsType, typename Enable = typename std::enable_if< IsArithmeticType< LhsType >::value >::type >\
	RetType operator Operator( LhsType const & p_lhs, Optional< RhsType > const & p_rhs );\
	template< typename LhsType, typename RhsType, typename Enable = typename std::enable_if< IsArithmeticType< LhsType >::value >::type >\
	RetType operator Operator( InParam< LhsType > const & p_lhs, RhsType const & p_rhs );\
	template< typename LhsType, typename RhsType, typename Enable = typename std::enable_if< IsArithmeticType< LhsType >::value >::type >\
	RetType operator Operator( InParam< LhsType > const & p_lhs, InParam< RhsType > const & p_rhs );\
	template< typename LhsType, typename RhsType, typename Enable = typename std::enable_if< IsArithmeticType< LhsType >::value >::type >\
	RetType operator Operator( InParam< LhsType > const & p_lhs, InOutParam< RhsType > const & p_rhs );\
	template< typename LhsType, typename RhsType, typename Enable = typename std::enable_if< IsArithmeticType< LhsType >::value >::type >\
	RetType operator Operator( InParam< LhsType > const & p_lhs, OutParam< RhsType > const & p_rhs );\
	template< typename LhsType, typename RhsType, typename Enable = typename std::enable_if< IsArithmeticType< LhsType >::value >::type >\
	RetType operator Operator( InParam< LhsType > const & p_lhs, Optional< RhsType > const & p_rhs );\
	template< typename LhsType, typename RhsType, typename Enable = typename std::enable_if< IsArithmeticType< LhsType >::value >::type >\
	RetType operator Operator( InOutParam< LhsType > const & p_lhs, RhsType const & p_rhs );\
	template< typename LhsType, typename RhsType, typename Enable = typename std::enable_if< IsArithmeticType< LhsType >::value >::type >\
	RetType operator Operator( InOutParam< LhsType > const & p_lhs, InParam< RhsType > const & p_rhs );\
	template< typename LhsType, typename RhsType, typename Enable = typename std::enable_if< IsArithmeticType< LhsType >::value >::type >\
	RetType operator Operator( InOutParam< LhsType > const & p_lhs, InOutParam< RhsType > const & p_rhs );\
	template< typename LhsType, typename RhsType, typename Enable = typename std::enable_if< IsArithmeticType< LhsType >::value >::type >\
	RetType operator Operator( InOutParam< LhsType > const & p_lhs, OutParam< RhsType > const & p_rhs );\
	template< typename LhsType, typename RhsType, typename Enable = typename std::enable_if< IsArithmeticType< LhsType >::value >::type >\
	RetType operator Operator( InOutParam< LhsType > const & p_lhs, Optional< RhsType > const & p_rhs );\
	template< typename LhsType, typename RhsType, typename Enable = typename std::enable_if< IsArithmeticType< LhsType >::value >::type >\
	RetType operator Operator( OutParam< LhsType > const & p_lhs, RhsType const & p_rhs );\
	template< typename LhsType, typename RhsType, typename Enable = typename std::enable_if< IsArithmeticType< LhsType >::value >::type >\
	RetType operator Operator( OutParam< LhsType > const & p_lhs, InParam< RhsType > const & p_rhs );\
	template< typename LhsType, typename RhsType, typename Enable = typename std::enable_if< IsArithmeticType< LhsType >::value >::type >\
	RetType operator Operator( OutParam< LhsType > const & p_lhs, InOutParam< RhsType > const & p_rhs );\
	template< typename LhsType, typename RhsType, typename Enable = typename std::enable_if< IsArithmeticType< LhsType >::value >::type >\
	RetType operator Operator( OutParam< LhsType > const & p_lhs, OutParam< RhsType > const & p_rhs );\
	template< typename LhsType, typename RhsType, typename Enable = typename std::enable_if< IsArithmeticType< LhsType >::value >::type >\
	RetType operator Operator( OutParam< LhsType > const & p_lhs, Optional< RhsType > const & p_rhs );\
	template< typename LhsType, typename RhsType, typename Enable = typename std::enable_if< IsArithmeticType< LhsType >::value >::type >\
	RetType operator Operator( Optional< LhsType > const & p_lhs, RhsType const & p_rhs );\
	template< typename LhsType, typename RhsType, typename Enable = typename std::enable_if< IsArithmeticType< LhsType >::value >::type >\
	RetType operator Operator( Optional< LhsType > const & p_lhs, InParam< RhsType > const & p_rhs );\
	template< typename LhsType, typename RhsType, typename Enable = typename std::enable_if< IsArithmeticType< LhsType >::value >::type >\
	RetType operator Operator( Optional< LhsType > const & p_lhs, InOutParam< RhsType > const & p_rhs );\
	template< typename LhsType, typename RhsType, typename Enable = typename std::enable_if< IsArithmeticType< LhsType >::value >::type >\
	RetType operator Operator( Optional< LhsType > const & p_lhs, OutParam< RhsType > const & p_rhs );\
	template< typename LhsType, typename RhsType, typename Enable = typename std::enable_if< IsArithmeticType< LhsType >::value >::type >\
	RetType operator Operator( Optional< LhsType > const & p_lhs, Optional< RhsType > const & p_rhs )

#	define GLSL_DECLARE_ARITHMETIC_OPERATOR( LhsType, RhsType, Operator )\
	GLSL_DECLARE_OPERATOR( LhsType, LhsType, RhsType, Operator )

#	define GLSL_DECLARE_BOOLEAN_OPERATOR( LhsType, RhsType, Operator )\
	GLSL_DECLARE_OPERATOR( Boolean, LhsType, RhsType, Operator )

	GLSL_DECLARE_ARITHMETIC_OPERATOR( TypeA, TypeB, + );
	GLSL_DECLARE_ARITHMETIC_OPERATOR( TypeA, TypeB, - );
	GLSL_DECLARE_ARITHMETIC_OPERATOR( TypeA, TypeB, * );
	GLSL_DECLARE_ARITHMETIC_OPERATOR( TypeA, TypeB, / );
	GLSL_DECLARE_BOOLEAN_OPERATOR( TypeA, TypeB, == );
	GLSL_DECLARE_BOOLEAN_OPERATOR( TypeA, TypeB, != );
	GLSL_DECLARE_BOOLEAN_OPERATOR( TypeA, TypeB, < );
	GLSL_DECLARE_BOOLEAN_OPERATOR( TypeA, TypeB, <= );
	GLSL_DECLARE_BOOLEAN_OPERATOR( TypeA, TypeB, > );
	GLSL_DECLARE_BOOLEAN_OPERATOR( TypeA, TypeB, >= );

	GlslWriter_API Vec4 operator*( Mat4 const & p_a, Vec4 const & p_b );
	GlslWriter_API Vec3 operator*( Mat3 const & p_a, Vec3 const & p_b );
	GlslWriter_API Float operator+( float p_a, Float const & p_b );
	GlslWriter_API Float operator-( float p_a, Float const & p_b );
	GlslWriter_API Float operator*( float p_a, Float const & p_b );
	GlslWriter_API Float operator/( float p_a, Float const & p_b );
	GlslWriter_API Int operator+( int p_a, Int const & p_b );
	GlslWriter_API Int operator-( int p_a, Int const & p_b );
	GlslWriter_API Int operator*( int p_a, Int const & p_b );
	GlslWriter_API Int operator/( int p_a, Int const & p_b );
	GlslWriter_API Int operator%( int p_a, Int const & p_b );
	GlslWriter_API Int operator%( Int const & p_a, int p_b );
	GlslWriter_API Int operator%( Int const & p_a, Int const & p_b );

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
	template< typename Value, typename ... Values > inline Value mix( Value const & p_value, Values const & ... p_values );
	template< typename Value, typename ... Values > inline Value reflect( Type const & p_value, Values const & ... p_values );
	template< typename Value > inline Value neg( Value const & p_value );
	template< typename Value > inline Value log( Value const & p_value );
	template< typename Value > inline Value exp( Value const & p_value );
	template< typename Value > inline Value log2( Value const & p_value );
	template< typename Value > inline Value exp2( Value const & p_value );
	template< typename Value > inline Value tan( Value const & p_value );
	template< typename Value > inline Value sin( Value const & p_value );
	template< typename Value > inline Value cos( Value const & p_value );
	template< typename Value > inline Value atan( Value const & p_y, Value const & p_x );
	template< typename Value > inline Value atan( Value const & p_value );
	template< typename Value > inline Value asin( Value const & p_value );
	template< typename Value > inline Value acos( Value const & p_value );
	template< typename Input, typename Output > inline Output neg( Swizzle< Input, Output > const & p_value );
	template< typename Value > inline Value normalize( Value const & p_value );
	template< typename Input, typename Output > inline Output normalize( Swizzle< Input, Output > const & p_value );
	template< typename Value > inline Value transpose( Value const & p_value );
	template< typename Value > inline Value inverse( Value const & p_value );
	template< typename Value > inline Value abs( Value const & p_value );
	template< typename Value > inline Value isinf( Value const & p_value );
	template< typename ValueA, typename ValueB > ValueB smoothstep( ValueA const & p_edge0, ValueA const & p_edge1, ValueB const & x );
	template< typename Value > inline Value dFdx( Value const & p_value );
	template< typename Value > inline Value dFdy( Value const & p_value );
	template< typename Value > inline Value dFdxCoarse( Value const & p_value );
	template< typename Value > inline Value dFdyCoarse( Value const & p_value );
	template< typename Value > inline Value dFdxFine( Value const & p_value );
	template< typename Value > inline Value dFdyFine( Value const & p_value );
	GlslWriter_API Int textureSize( Sampler1D const & p_sampler, Int const p_lod );
	GlslWriter_API IVec2 textureSize( Sampler2D const & p_sampler, Int const & p_lod );
	GlslWriter_API IVec3 textureSize( Sampler3D const & p_sampler, Int const & p_lod );
	GlslWriter_API IVec2 textureSize( SamplerCube const & p_sampler, Int const & p_lod );
	GlslWriter_API IVec2 textureSize( Sampler1DArray const & p_sampler, Int const p_lod );
	GlslWriter_API IVec3 textureSize( Sampler2DArray const & p_sampler, Int const & p_lod );
	GlslWriter_API IVec3 textureSize( SamplerCubeArray const & p_sampler, Int const & p_lod );
	GlslWriter_API Int textureSize( Sampler1DShadow const & p_sampler, Int const p_lod );
	GlslWriter_API IVec2 textureSize( Sampler2DShadow const & p_sampler, Int const & p_lod );
	GlslWriter_API IVec2 textureSize( SamplerCubeShadow const & p_sampler, Int const & p_lod );
	GlslWriter_API IVec2 textureSize( Sampler1DArrayShadow const & p_sampler, Int const p_lod );
	GlslWriter_API IVec3 textureSize( Sampler2DArrayShadow const & p_sampler, Int const & p_lod );
	GlslWriter_API IVec3 textureSize( SamplerCubeArrayShadow const & p_sampler, Int const & p_lod );
	GlslWriter_API Vec4 texture( Sampler1D const & p_sampler, Float const & p_value );
	GlslWriter_API Vec4 texture( Sampler1D const & p_sampler, Float const & p_value, Float const & p_lod );
	GlslWriter_API Vec4 texture( Sampler2D const & p_sampler, Vec2 const & p_value );
	GlslWriter_API Vec4 texture( Sampler2D const & p_sampler, Vec2 const & p_value, Float const & p_lod );
	GlslWriter_API Vec4 texture( Sampler3D const & p_sampler, Vec3 const & p_value );
	GlslWriter_API Vec4 texture( Sampler3D const & p_sampler, Vec3 const & p_value, Float const & p_lod );
	GlslWriter_API Vec4 texture( SamplerCube const & p_sampler, Vec3 const & p_value );
	GlslWriter_API Vec4 texture( SamplerCube const & p_sampler, Vec3 const & p_value, Float const & p_lod );
	GlslWriter_API Vec4 textureOffset( Sampler1D const & p_sampler, Vec2 const & p_value, Int const p_offset );
	GlslWriter_API Vec4 textureOffset( Sampler1D const & p_sampler, Vec2 const & p_value, Int const p_offset, Float const & p_lod );
	GlslWriter_API Vec4 textureOffset( Sampler2D const & p_sampler, Vec2 const & p_value, IVec2 const p_offset );
	GlslWriter_API Vec4 textureOffset( Sampler2D const & p_sampler, Vec2 const & p_value, IVec2 const p_offset, Float const & p_lod );
	GlslWriter_API Vec4 textureOffset( Sampler3D const & p_sampler, Vec3 const & p_value, IVec3 const p_offset );
	GlslWriter_API Vec4 textureOffset( Sampler3D const & p_sampler, Vec3 const & p_value, IVec3 const p_offset, Float const & p_lod );
	GlslWriter_API Vec4 textureLodOffset( Sampler1D const & p_sampler, Vec2 const & p_value, Float const & p_lod, Int const p_offset );
	GlslWriter_API Vec4 textureLodOffset( Sampler2D const & p_sampler, Vec2 const & p_value, Float const & p_lod, IVec2 const p_offset );
	GlslWriter_API Vec4 textureLodOffset( Sampler3D const & p_sampler, Vec3 const & p_value, Float const & p_lod, IVec3 const p_offset );
	GlslWriter_API Vec4 texelFetch( SamplerBuffer const & p_sampler, Type const & p_value );
	GlslWriter_API Vec4 texelFetch( Sampler1D const & p_sampler, Type const & p_value, Int const & p_modif );
	GlslWriter_API Vec4 texelFetch( Sampler2D const & p_sampler, Type const & p_value, Int const & p_modif );
	GlslWriter_API Vec4 texelFetch( Sampler3D const & p_sampler, Type const & p_value, Int const & p_modif );
	GlslWriter_API Vec4 texture( Sampler1DArray const & p_sampler, Vec2 const & p_value );
	GlslWriter_API Vec4 texture( Sampler1DArray const & p_sampler, Vec2 const & p_value, Float const & p_lod );
	GlslWriter_API Vec4 texture( Sampler2DArray const & p_sampler, Vec3 const & p_value );
	GlslWriter_API Vec4 texture( Sampler2DArray const & p_sampler, Vec3 const & p_value, Float const & p_lod );
	GlslWriter_API Vec4 texture( SamplerCubeArray const & p_sampler, Vec4 const & p_value );
	GlslWriter_API Vec4 texture( SamplerCubeArray const & p_sampler, Vec4 const & p_value, Float const & p_lod );
	GlslWriter_API Vec4 textureOffset( Sampler1DArray const & p_sampler, Vec2 const & p_value, Int const p_offset );
	GlslWriter_API Vec4 textureOffset( Sampler1DArray const & p_sampler, Vec2 const & p_value, Int const p_offset, Float const & p_lod );
	GlslWriter_API Vec4 textureOffset( Sampler2DArray const & p_sampler, Vec3 const & p_value, IVec2 const p_offset );
	GlslWriter_API Vec4 textureOffset( Sampler2DArray const & p_sampler, Vec3 const & p_value, IVec2 const p_offset, Float const & p_lod );
	GlslWriter_API Vec4 textureLodOffset( Sampler1DArray const & p_sampler, Vec2 const & p_value, Float const & p_lod, Int const p_offset );
	GlslWriter_API Vec4 textureLodOffset( Sampler2DArray const & p_sampler, Vec3 const & p_value, Float const & p_lod, IVec2 const p_offset );
	GlslWriter_API Float texture( Sampler1DShadow const & p_sampler, Vec2 const & p_value );
	GlslWriter_API Float texture( Sampler1DShadow const & p_sampler, Vec2 const & p_value, Float const & p_lod );
	GlslWriter_API Float texture( Sampler2DShadow const & p_sampler, Vec3 const & p_value );
	GlslWriter_API Float texture( Sampler2DShadow const & p_sampler, Vec3 const & p_value, Float const & p_lod );
	GlslWriter_API Float texture( SamplerCubeShadow const & p_sampler, Vec4 const & p_value );
	GlslWriter_API Float texture( SamplerCubeShadow const & p_sampler, Vec4 const & p_value, Float const & p_lod );
	GlslWriter_API Float textureOffset( Sampler1DShadow const & p_sampler, Vec2 const & p_value, Int const p_offset );
	GlslWriter_API Float textureOffset( Sampler1DShadow const & p_sampler, Vec2 const & p_value, Int const p_offset, Float const & p_lod );
	GlslWriter_API Float textureOffset( Sampler2DShadow const & p_sampler, Vec3 const & p_value, IVec2 const p_offset );
	GlslWriter_API Float textureOffset( Sampler2DShadow const & p_sampler, Vec3 const & p_value, IVec2 const p_offset, Float const & p_lod );
	GlslWriter_API Float textureLodOffset( Sampler1DShadow const & p_sampler, Vec2 const & p_value, Float const & p_lod, Int const p_offset );
	GlslWriter_API Float textureLodOffset( Sampler2DShadow const & p_sampler, Vec3 const & p_value, Float const & p_lod, IVec2 const p_offset );
	GlslWriter_API Float texture( Sampler1DArrayShadow const & p_sampler, Vec3 const & p_value );
	GlslWriter_API Float texture( Sampler1DArrayShadow const & p_sampler, Vec3 const & p_value, Float const & p_lod );
	GlslWriter_API Float texture( Sampler2DArrayShadow const & p_sampler, Vec4 const & p_value );
	GlslWriter_API Float texture( Sampler2DArrayShadow const & p_sampler, Vec4 const & p_value, Float const & p_lod );
	GlslWriter_API Float texture( SamplerCubeArrayShadow const & p_sampler, Vec4 const & p_value, Float const & p_layer );
	GlslWriter_API Float texture( SamplerCubeArrayShadow const & p_sampler, Vec4 const & p_value, Float const & p_layer, Float const & p_lod );
	GlslWriter_API Float textureOffset( Sampler1DArrayShadow const & p_sampler, Vec3 const & p_value, Int const p_offset );
	GlslWriter_API Float textureOffset( Sampler1DArrayShadow const & p_sampler, Vec3 const & p_value, Int const p_offset, Float const & p_lod );
	GlslWriter_API Float textureOffset( Sampler2DArrayShadow const & p_sampler, Vec4 const & p_value, IVec2 const p_offset );
	GlslWriter_API Float textureOffset( Sampler2DArrayShadow const & p_sampler, Vec4 const & p_value, IVec2 const p_offset, Float const & p_lod );
	GlslWriter_API Float textureLodOffset( Sampler1DArrayShadow const & p_sampler, Vec3 const & p_value, Float const & p_lod, Int const p_offset );
	GlslWriter_API Float textureLodOffset( Sampler2DArrayShadow const & p_sampler, Vec4 const & p_value, Float const & p_lod, IVec2 const p_offset );
	GlslWriter_API Vec2 reflect( Vec2 const & p_a, Vec2 const & p_b );
	GlslWriter_API Vec3 reflect( Vec3 const & p_a, Vec3 const & p_b );
	GlslWriter_API Vec4 reflect( Vec4 const & p_a, Vec4 const & p_b );
	GlslWriter_API Vec2 refract( Vec2 const & p_a, Vec2 const & p_b, Float const & p_r );
	GlslWriter_API Vec3 refract( Vec3 const & p_a, Vec3 const & p_b, Float const & p_r );
	GlslWriter_API Vec4 refract( Vec4 const & p_a, Vec4 const & p_b, Float const & p_r );
	GlslWriter_API Float length( Type const & p_value );
	GlslWriter_API Float distance( Type const & p_a, Type const & p_b );
	GlslWriter_API Float radians( Type const & p_value );
	GlslWriter_API Float cos( Type const & p_value );
	GlslWriter_API Float sin( Type const & p_value );
	GlslWriter_API Float tan( Type const & p_value );
	GlslWriter_API Float fract( Type const & p_value );
	GlslWriter_API Optional< Int > textureSize( Optional< Sampler1D > const & p_sampler, Int const p_lod );
	GlslWriter_API Optional< IVec2 > textureSize( Optional< Sampler2D > const & p_sampler, Int const & p_lod );
	GlslWriter_API Optional< IVec3 > textureSize( Optional< Sampler3D > const & p_sampler, Int const & p_lod );
	GlslWriter_API Optional< IVec2 > textureSize( Optional< SamplerCube > const & p_sampler, Int const & p_lod );
	GlslWriter_API Optional< IVec2 > textureSize( Optional< Sampler1DArray > const & p_sampler, Int const p_lod );
	GlslWriter_API Optional< IVec3 > textureSize( Optional< Sampler2DArray > const & p_sampler, Int const & p_lod );
	GlslWriter_API Optional< IVec3 > textureSize( Optional< SamplerCubeArray > const & p_sampler, Int const & p_lod );
	GlslWriter_API Optional< Int > textureSize( Optional< Sampler1DShadow > const & p_sampler, Int const p_lod );
	GlslWriter_API Optional< IVec2 > textureSize( Optional< Sampler2DShadow > const & p_sampler, Int const & p_lod );
	GlslWriter_API Optional< IVec2 > textureSize( Optional< SamplerCubeShadow > const & p_sampler, Int const & p_lod );
	GlslWriter_API Optional< IVec2 > textureSize( Optional< Sampler1DArrayShadow > const & p_sampler, Int const p_lod );
	GlslWriter_API Optional< IVec3 > textureSize( Optional< Sampler2DArrayShadow > const & p_sampler, Int const & p_lod );
	GlslWriter_API Optional< IVec3 > textureSize( Optional< SamplerCubeArrayShadow > const & p_sampler, Int const & p_lod );
	GlslWriter_API Optional< Vec4 > texture( Optional< Sampler1D > const & p_sampler, Float const & p_value );
	GlslWriter_API Optional< Vec4 > texture( Optional< Sampler1D > const & p_sampler, Float const & p_value, Float const & p_lod );
	GlslWriter_API Optional< Vec4 > texture( Optional< Sampler2D > const & p_sampler, Vec2 const & p_value );
	GlslWriter_API Optional< Vec4 > texture( Optional< Sampler2D > const & p_sampler, Vec2 const & p_value, Float const & p_lod );
	GlslWriter_API Optional< Vec4 > texture( Optional< Sampler3D > const & p_sampler, Vec3 const & p_value );
	GlslWriter_API Optional< Vec4 > texture( Optional< Sampler3D > const & p_sampler, Vec3 const & p_value, Float const & p_lod );
	GlslWriter_API Optional< Vec4 > texture( Optional< SamplerCube > const & p_sampler, Vec3 const & p_value );
	GlslWriter_API Optional< Vec4 > texture( Optional< SamplerCube > const & p_sampler, Vec3 const & p_value, Float const & p_lod );
	GlslWriter_API Optional< Vec4 > textureOffset( Optional< Sampler1D > const & p_sampler, Float const & p_value, Int const p_offset );
	GlslWriter_API Optional< Vec4 > textureOffset( Optional< Sampler1D > const & p_sampler, Float const & p_value, Int const p_offset, Float const & p_lod );
	GlslWriter_API Optional< Vec4 > textureOffset( Optional< Sampler2D > const & p_sampler, Vec2 const & p_value, IVec2 const p_offset );
	GlslWriter_API Optional< Vec4 > textureOffset( Optional< Sampler2D > const & p_sampler, Vec2 const & p_value, IVec2 const p_offset, Float const & p_lod );
	GlslWriter_API Optional< Vec4 > textureOffset( Optional< Sampler3D > const & p_sampler, Vec3 const & p_value, IVec3 const p_offset );
	GlslWriter_API Optional< Vec4 > textureOffset( Optional< Sampler3D > const & p_sampler, Vec3 const & p_value, IVec3 const p_offset, Float const & p_lod );
	GlslWriter_API Optional< Vec4 > textureLodOffset( Optional< Sampler1D > const & p_sampler, Vec2 const & p_value, Float const & p_lod, Int const p_offset );
	GlslWriter_API Optional< Vec4 > textureLodOffset( Optional< Sampler2D > const & p_sampler, Vec2 const & p_value, Float const & p_lod, IVec2 const p_offset );
	GlslWriter_API Optional< Vec4 > textureLodOffset( Optional< Sampler3D > const & p_sampler, Vec3 const & p_value, Float const & p_lod, IVec3 const p_offset );
	GlslWriter_API Optional< Vec4 > texelFetch( Optional< SamplerBuffer > const & p_sampler, Type const & p_value );
	GlslWriter_API Optional< Vec4 > texelFetch( Optional< Sampler1D > const & p_sampler, Type const & p_value, Int const & p_modif );
	GlslWriter_API Optional< Vec4 > texelFetch( Optional< Sampler2D > const & p_sampler, Type const & p_value, Int const & p_modif );
	GlslWriter_API Optional< Vec4 > texelFetch( Optional< Sampler3D > const & p_sampler, Type const & p_value, Int const & p_modif );
	GlslWriter_API Optional< Vec4 > texture( Optional< Sampler1DArray > const & p_sampler, Vec2 const & p_value );
	GlslWriter_API Optional< Vec4 > texture( Optional< Sampler1DArray > const & p_sampler, Vec2 const & p_value, Float const & p_lod );
	GlslWriter_API Optional< Vec4 > texture( Optional< Sampler2DArray > const & p_sampler, Vec3 const & p_value );
	GlslWriter_API Optional< Vec4 > texture( Optional< Sampler2DArray > const & p_sampler, Vec3 const & p_value, Float const & p_lod );
	GlslWriter_API Optional< Vec4 > texture( Optional< SamplerCubeArray > const & p_sampler, Vec4 const & p_value );
	GlslWriter_API Optional< Vec4 > texture( Optional< SamplerCubeArray > const & p_sampler, Vec4 const & p_value, Float const & p_lod );
	GlslWriter_API Optional< Vec4 > textureOffset( Optional< Sampler1DArray > const & p_sampler, Vec2 const & p_value, Int const p_offset );
	GlslWriter_API Optional< Vec4 > textureOffset( Optional< Sampler1DArray > const & p_sampler, Vec2 const & p_value, Int const p_offset, Float const & p_lod );
	GlslWriter_API Optional< Vec4 > textureOffset( Optional< Sampler2DArray > const & p_sampler, Vec3 const & p_value, IVec2 const p_offset );
	GlslWriter_API Optional< Vec4 > textureOffset( Optional< Sampler2DArray > const & p_sampler, Vec3 const & p_value, IVec2 const p_offset, Float const & p_lod );
	GlslWriter_API Optional< Vec4 > textureLodOffset( Optional< Sampler1DArray > const & p_sampler, Vec2 const & p_value, Float const & p_lod, Int const p_offset );
	GlslWriter_API Optional< Vec4 > textureLodOffset( Optional< Sampler2DArray > const & p_sampler, Vec3 const & p_value, Float const & p_lod, IVec2 const p_offset );
	GlslWriter_API Optional< Float > texture( Optional< Sampler1DShadow > const & p_sampler, Vec2 const & p_value );
	GlslWriter_API Optional< Float > texture( Optional< Sampler1DShadow > const & p_sampler, Vec2 const & p_value, Float const & p_lod );
	GlslWriter_API Optional< Float > texture( Optional< Sampler2DShadow > const & p_sampler, Vec3 const & p_value );
	GlslWriter_API Optional< Float > texture( Optional< Sampler2DShadow > const & p_sampler, Vec3 const & p_value, Float const & p_lod );
	GlslWriter_API Optional< Float > texture( Optional< SamplerCubeShadow > const & p_sampler, Vec4 const & p_value );
	GlslWriter_API Optional< Float > texture( Optional< SamplerCubeShadow > const & p_sampler, Vec4 const & p_value, Float const & p_lod );
	GlslWriter_API Optional< Float > textureOffset( Optional< Sampler1DShadow > const & p_sampler, Vec2 const & p_value, Int const p_offset );
	GlslWriter_API Optional< Float > textureOffset( Optional< Sampler1DShadow > const & p_sampler, Vec2 const & p_value, Int const p_offset, Float const & p_lod );
	GlslWriter_API Optional< Float > textureOffset( Optional< Sampler2DShadow > const & p_sampler, Vec3 const & p_value, IVec2 const p_offset );
	GlslWriter_API Optional< Float > textureOffset( Optional< Sampler2DShadow > const & p_sampler, Vec3 const & p_value, IVec2 const p_offset, Float const & p_lod );
	GlslWriter_API Optional< Float > textureLodOffset( Optional< Sampler1DShadow > const & p_sampler, Vec2 const & p_value, Float const & p_lod, Int const p_offset );
	GlslWriter_API Optional< Float > textureLodOffset( Optional< Sampler2DShadow > const & p_sampler, Vec3 const & p_value, Float const & p_lod, IVec2 const p_offset );
	GlslWriter_API Optional< Float > texture( Optional< Sampler1DArrayShadow > const & p_sampler, Vec3 const & p_value );
	GlslWriter_API Optional< Float > texture( Optional< Sampler1DArrayShadow > const & p_sampler, Vec3 const & p_value, Float const & p_lod );
	GlslWriter_API Optional< Float > texture( Optional< Sampler2DArrayShadow > const & p_sampler, Vec4 const & p_value );
	GlslWriter_API Optional< Float > texture( Optional< Sampler2DArrayShadow > const & p_sampler, Vec4 const & p_value, Float const & p_lod );
	GlslWriter_API Optional< Float > texture( Optional< SamplerCubeArrayShadow > const & p_sampler, Vec4 const & p_value, Float const & p_layer );
	GlslWriter_API Optional< Float > texture( Optional< SamplerCubeArrayShadow > const & p_sampler, Vec4 const & p_value, Float const & p_layer, Float const & p_lod );
	GlslWriter_API Optional< Float > textureOffset( Optional< Sampler1DArrayShadow > const & p_sampler, Vec3 const & p_value, Int const p_offset );
	GlslWriter_API Optional< Float > textureOffset( Optional< Sampler1DArrayShadow > const & p_sampler, Vec3 const & p_value, Int const p_offset, Float const & p_lod );
	GlslWriter_API Optional< Float > textureOffset( Optional< Sampler2DArrayShadow > const & p_sampler, Vec4 const & p_value, IVec2 const p_offset );
	GlslWriter_API Optional< Float > textureOffset( Optional< Sampler2DArrayShadow > const & p_sampler, Vec4 const & p_value, IVec2 const p_offset, Float const & p_lod );
	GlslWriter_API Optional< Float > textureLodOffset( Optional< Sampler1DArrayShadow > const & p_sampler, Vec3 const & p_value, Float const & p_lod, Int const p_offset );
	GlslWriter_API Optional< Float > textureLodOffset( Optional< Sampler2DArrayShadow > const & p_sampler, Vec4 const & p_value, Float const & p_lod, IVec2 const p_offset );
	GlslWriter_API Optional< Vec2 > reflect( Optional< Vec2 > const & p_a, Type const & p_b );
	GlslWriter_API Optional< Vec3 > reflect( Optional< Vec3 > const & p_a, Type const & p_b );
	GlslWriter_API Optional< Vec4 > reflect( Optional< Vec4 > const & p_a, Type const & p_b );
	GlslWriter_API Optional< Float > length( Optional< Type > const & p_value );
	GlslWriter_API Optional< Float > radians( Optional< Type > const & p_value );
	GlslWriter_API Optional< Float > cos( Optional< Type > const & p_value );
	GlslWriter_API Optional< Float > sin( Optional< Type > const & p_value );
	GlslWriter_API Optional< Float > tan( Optional< Type > const & p_value );
	GlslWriter_API Optional< Float > fract( Optional< Type > const & p_value );
}

#include "GlslIntrinsics.inl"

#endif
