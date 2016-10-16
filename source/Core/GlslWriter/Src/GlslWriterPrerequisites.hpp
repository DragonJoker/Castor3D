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

#ifdef _WIN32
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
	enum class ShadowType
	{
		None,
		Raw,
		Poisson,
		StratifiedPoisson
	};

	struct GlslWriterConfig;
	class GlslWriter;
	class KeywordsBase;
	template< int Version, class Enable = void >
	class Keywords;
	class LightingModel;
	class PhongLightingModel;
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

	struct Expr;
	struct Type;
	struct Void;
	struct GlslBool;
	struct Int;
	struct Float;
	struct Vec2;
	struct Vec3;
	struct Vec4;
	struct IVec2;
	struct IVec3;
	struct iVec4;
	struct Mat3;
	struct Mat4;
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
}

#endif
