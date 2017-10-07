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
#ifndef ___GLSL_WRITER_PREREQUISITES_H___
#define ___GLSL_WRITER_PREREQUISITES_H___

#include <CastorUtils.hpp>
#include <Design/Factory.hpp>

#ifdef max
#	undef max
#	undef min
#	undef abs
#endif

#ifdef OUT
#	undef OUT
#endif

#ifdef IN
#	undef IN
#endif

#ifdef CASTOR_PLATFORM_WINDOWS
#	ifdef GlslWriter_EXPORTS
#		define GlslWriter_API __declspec( dllexport )
#	else
#		define GlslWriter_API __declspec( dllimport )
#	endif
#else
#	define GlslWriter_API
#endif

#define GLSL_MATERIALS_STRUCT_OF_ARRAY 0

#define DECLARE_GLSL_PARAMETER( TypeName )\
	using In##TypeName = glsl::InParam< TypeName >;\
	using Out##TypeName = glsl::OutParam< TypeName >;\
	using InOut##TypeName = glsl::InOutParam< TypeName >\

namespace glsl
{
	struct IndentBlock;
	struct GlslWriterConfig;
	class GlslWriter;
	class Shader;
	class KeywordsBase;
	template< int Version, class Enable = void >
	class Keywords;

	enum class TypeName
	{
		eBool,
		eInt,
		eUInt,
		eFloat,
		eVec2B,
		eVec3B,
		eVec4B,
		eVec2I,
		eVec3I,
		eVec4I,
		eVec2F,
		eVec3F,
		eVec4F,
		eMat2x2B,
		eMat3x3B,
		eMat4x4B,
		eMat2x2I,
		eMat3x3I,
		eMat4x4I,
		eMat2x2F,
		eMat3x3F,
		eMat4x4F,
		eSamplerBuffer,
		eSampler1D,
		eSampler2D,
		eSampler3D,
		eSamplerCube,
		eSampler2DRect,
		eSampler1DArray,
		eSampler2DArray,
		eSamplerCubeArray,
		eSampler1DShadow,
		eSampler2DShadow,
		eSamplerCubeShadow,
		eSampler2DRectShadow,
		eSampler1DArrayShadow,
		eSampler2DArrayShadow,
		eSamplerCubeArrayShadow,
		CASTOR_SCOPED_ENUM_BOUNDS( eBool )
	};

	enum class SamplerType
	{
		eBuffer,
		e1D,
		e2D,
		e3D,
		eCube,
		e2DRect,
		e1DArray,
		e2DArray,
		eCubeArray,
		e1DShadow,
		e2DShadow,
		eCubeShadow,
		e2DRectShadow,
		e1DArrayShadow,
		e2DArrayShadow,
		eCubeArrayShadow,
		CASTOR_SCOPED_ENUM_BOUNDS( eBuffer )
	};

	template< typename T >
	struct Array;
	template< typename TypeT >
	struct InParam;
	template< typename TypeT >
	struct OutParam;
	template< typename TypeT >
	struct InOutParam;
	template< typename TypeT >
	struct Optional;
	template< typename TypeT >
	struct Vec2T;
	template< typename TypeT >
	struct Vec3T;
	template< typename TypeT >
	struct Vec4T;
	template< typename TypeT >
	struct Mat2T;
	template< typename TypeT >
	struct Mat3T;
	template< typename TypeT >
	struct Mat4T;
	template< SamplerType T >
	struct SamplerT;

	struct Expr;
	struct Type;
	struct Void;
	struct Boolean;
	struct Int;
	struct UInt;
	struct Float;

	using Vec2 = Vec2T< Float >;
	using Vec3 = Vec3T< Float >;
	using Vec4 = Vec4T< Float >;
	using IVec2 = Vec2T< Int >;
	using IVec3 = Vec3T< Int >;
	using IVec4 = Vec4T< Int >;
	using BVec2 = Vec2T< Boolean >;
	using BVec3 = Vec3T< Boolean >;
	using BVec4 = Vec4T< Boolean >;
	using Mat2 = Mat2T< Float >;
	using Mat3 = Mat3T< Float >;
	using Mat4 = Mat4T< Float >;
	using IMat2 = Mat2T< Int >;
	using IMat3 = Mat3T< Int >;
	using IMat4 = Mat4T< Int >;
	using BMat2 = Mat2T< Boolean >;
	using BMat3 = Mat3T< Boolean >;
	using BMat4 = Mat4T< Boolean >;

	using SamplerBuffer = SamplerT< SamplerType::eBuffer >;
	using Sampler1D = SamplerT< SamplerType::e1D >;
	using Sampler2D = SamplerT< SamplerType::e2D >;
	using Sampler3D = SamplerT< SamplerType::e3D >;
	using SamplerCube = SamplerT< SamplerType::eCube >;
	using Sampler2DRect = SamplerT< SamplerType::e2DRect >;
	using Sampler1DArray = SamplerT< SamplerType::e1DArray >;
	using Sampler2DArray = SamplerT< SamplerType::e2DArray >;
	using SamplerCubeArray = SamplerT< SamplerType::eCubeArray >;
	using Sampler1DShadow = SamplerT< SamplerType::e1DShadow >;
	using Sampler2DShadow = SamplerT< SamplerType::e2DShadow >;
	using SamplerCubeShadow = SamplerT< SamplerType::eCubeShadow >;
	using Sampler2DRectShadow = SamplerT< SamplerType::e2DRectShadow >;
	using Sampler1DArrayShadow = SamplerT< SamplerType::e1DArrayShadow >;
	using Sampler2DArrayShadow = SamplerT< SamplerType::e2DArrayShadow >;
	using SamplerCubeArrayShadow = SamplerT< SamplerType::eCubeArrayShadow >;

	DECLARE_GLSL_PARAMETER( Float );
	DECLARE_GLSL_PARAMETER( Int );
	DECLARE_GLSL_PARAMETER( UInt );
	DECLARE_GLSL_PARAMETER( Boolean );
	DECLARE_GLSL_PARAMETER( Vec2 );
	DECLARE_GLSL_PARAMETER( Vec3 );
	DECLARE_GLSL_PARAMETER( Vec4 );
	DECLARE_GLSL_PARAMETER( IVec2 );
	DECLARE_GLSL_PARAMETER( IVec3 );
	DECLARE_GLSL_PARAMETER( IVec4 );
	DECLARE_GLSL_PARAMETER( BVec2 );
	DECLARE_GLSL_PARAMETER( BVec3 );
	DECLARE_GLSL_PARAMETER( BVec4 );
	DECLARE_GLSL_PARAMETER( Mat2 );
	DECLARE_GLSL_PARAMETER( Mat3 );
	DECLARE_GLSL_PARAMETER( Mat4 );
	DECLARE_GLSL_PARAMETER( IMat2 );
	DECLARE_GLSL_PARAMETER( IMat3 );
	DECLARE_GLSL_PARAMETER( IMat4 );
	DECLARE_GLSL_PARAMETER( BMat2 );
	DECLARE_GLSL_PARAMETER( BMat3 );
	DECLARE_GLSL_PARAMETER( BMat4 );
	DECLARE_GLSL_PARAMETER( SamplerBuffer );
	DECLARE_GLSL_PARAMETER( Sampler1D );
	DECLARE_GLSL_PARAMETER( Sampler2D );
	DECLARE_GLSL_PARAMETER( Sampler3D );
	DECLARE_GLSL_PARAMETER( SamplerCube );
	DECLARE_GLSL_PARAMETER( Sampler1DArray );
	DECLARE_GLSL_PARAMETER( Sampler2DArray );
	DECLARE_GLSL_PARAMETER( SamplerCubeArray );
	DECLARE_GLSL_PARAMETER( Sampler1DShadow );
	DECLARE_GLSL_PARAMETER( Sampler2DShadow );
	DECLARE_GLSL_PARAMETER( SamplerCubeShadow );
	DECLARE_GLSL_PARAMETER( Sampler1DArrayShadow );
	DECLARE_GLSL_PARAMETER( Sampler2DArrayShadow );
	DECLARE_GLSL_PARAMETER( SamplerCubeArrayShadow );

	template< typename RetT, typename ... ParamsT >
	struct Function;

	template< typename T >
	struct TypeTraits;
	template< SamplerType ST >
	struct SamplerTypeTraits;
	template< typename T >
	struct TypeOf
	{
		using Type = T;
	};

	struct Endl
	{
	};
	struct Endi
	{
	};
	struct Version
	{
	};
	struct Attribute
	{
	};
	struct In
	{
	};
	struct Out
	{
	};
	struct Layout
	{
		int m_index;
	};
	struct Uniform
	{
	};

	static Endi endi;
	static Endl endl;

	GlslWriter_API void writeLine( GlslWriter & p_writer, castor::String const & p_line );
	GlslWriter_API void registerName( GlslWriter & p_writer, castor::String const & p_name, TypeName p_type );
	GlslWriter_API void checkNameExists( GlslWriter & p_writer, castor::String const & p_name, TypeName p_type );
}

#include "GlslWriterPrerequisites.inl"

#endif
