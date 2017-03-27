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

#ifdef CASTOR_PLATFORM_WINDOWS
#	ifdef GlslWriter_EXPORTS
#		define GlslWriter_API __declspec( dllexport )
#	else
#		define GlslWriter_API __declspec( dllimport )
#	endif
#else
#	define GlslWriter_API
#endif

namespace GLSL
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date		31/05/2016
	\~english
	\brief		Shadow filter types enumeration.
	\~french
	\brief		Enum�ration des types de filtrage des ombres.
	*/
	enum class ShadowType
	{
		//!\~english	No shadows at all.
		//!\~french		Pas d'ombres du tout.
		eNone,
		//!\~english	Poisson filtering.
		//!\~french		Filtrage poisson.
		eRaw,
		//!\~english	Poisson filtering.
		//!\~french		Filtrage poisson.
		ePoisson,
		//!\~english	Stratified poisson filtering.
		//!\~french		Filtrage poisson stratifi�.
		eStratifiedPoisson,
		CASTOR_SCOPED_ENUM_BOUNDS( eNone )
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date		31/05/2016
	\~english
	\brief		Fog types enumeration.
	\~french
	\brief		Enum�ration des types de brouillard.
	*/
	enum class FogType
	{
		//!\~english	No fog.
		//!\~french		Pas de brouillard
		eDisabled,
		//!\~english	Fog intensity increases linearly with distance to camera.
		//!\~french		L'intensit� du brouillard augmente lin�airement avec la distance � la cam�ra.
		eLinear,
		//!\~english	Fog intensity increases exponentially with distance to camera.
		//!\~french		L'intensit� du brouillard augmente exponentiellement avec la distance � la cam�ra.
		//!\~french		
		eExponential,
		//!\~english	Fog intensity increases even more with distance to camera.
		//!\~french		L'intensit� du brouillard augmente encore plus avec la distance � la cam�ra.
		eSquaredExponential,
		CASTOR_SCOPED_ENUM_BOUNDS( eDisabled )
	};
	enum class UboLayout
	{
		eStd140,
		ePacked,
		eShared
	};

	struct GlslWriterConfig;
	class GlslWriter;
	class KeywordsBase;
	template< int Version, class Enable = void >
	class Keywords;
	class LightingModel;
	class PhongLightingModel;
	class DeferredPhongLightingModel;
	struct Light;

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

	struct Expr;
	struct Type;
	struct Void;
	struct Boolean;
	struct Int;
	struct Float;
	struct SamplerBuffer;
	struct Sampler1D;
	struct Sampler2D;
	struct Sampler3D;
	struct SamplerCube;
	struct Sampler1DArray;
	struct Sampler2DArray;
	struct SamplerCubeArray;
	struct Sampler1DShadow;
	struct Sampler2DShadow;
	struct SamplerCubeShadow;
	struct Sampler1DArrayShadow;
	struct Sampler2DArrayShadow;
	struct SamplerCubeArrayShadow;

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

	template< typename RetT, typename ... ParamsT >
	struct Function;

	constexpr uint32_t SpotShadowMapCount = 10u;
	constexpr uint32_t PointShadowMapCount = 6u;
}

#endif
