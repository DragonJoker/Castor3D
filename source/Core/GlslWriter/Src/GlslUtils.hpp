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
#ifndef ___GLSL_Utils_H___
#define ___GLSL_Utils_H___

#include "GlslFunction.hpp"

namespace glsl
{
	class Utils
	{
	public:
		GlslWriter_API Utils( GlslWriter & writer );
		GlslWriter_API void declareCalcTexCoord();
		GlslWriter_API void declareCalcVSPosition();
		GlslWriter_API void declareCalcVSDepth();
		GlslWriter_API void declareCalcWSPosition();
		GlslWriter_API void declareApplyGamma();
		GlslWriter_API void declareRemoveGamma();
		GlslWriter_API void declareLineariseDepth();
		GlslWriter_API void declareGetMapNormal();
		GlslWriter_API void declareFresnelSchlick();
		GlslWriter_API void declareComputeMetallicIBL();
		GlslWriter_API void declareComputeSpecularIBL();
		GlslWriter_API Vec2 calcTexCoord();
		GlslWriter_API Vec3 calcVSPosition( Vec2 const & uv
			, Mat4 const & invProj );
		GlslWriter_API Float calcVSDepth( Vec2 const & uv
			, Mat4 const & proj );
		GlslWriter_API Vec3 calcWSPosition( Vec2 const & uv
			, Float const & depth
			, Mat4 const & invViewProj );
		GlslWriter_API Vec3 applyGamma( Float const & gamma
			, Vec3 const & HDR );
		GlslWriter_API Vec3 removeGamma( Float const & gamma
			, Vec3 const & sRGB );
		GlslWriter_API Vec3 getMapNormal( Vec2 const & uv
			, Vec3 const & normal
			, Vec3 const & position );
		GlslWriter_API Float lineariseDepth( Float const & depth
			, Mat4 const & invProj );
		GlslWriter_API Vec3 fresnelSchlick( Float const & product
			, Vec3 const & f0
			, Float const & roughness );
		GlslWriter_API Vec3 computeMetallicIBL( Vec3 const & normal
			, Vec3 const & position
			, Vec3 const & albedo
			, Float const & metalness
			, Float const & roughness
			, Vec3 const & worldEye
			, SamplerCube const & irradiance
			, SamplerCube const & prefiltered
			, Sampler2D const & brdf );
		GlslWriter_API Vec3 computeSpecularIBL( Vec3 const & normal
			, Vec3 const & position
			, Vec3 const & diffuse
			, Vec3 const & specular
			, Float const & glossiness
			, Vec3 const & worldEye
			, SamplerCube const & irradiance
			, SamplerCube const & prefiltered
			, Sampler2D const & brdf );
	public:
		GlslWriter_API static uint32_t const MaxIblReflectionLod;

	private:
		GlslWriter & m_writer;
		Function< Vec2 > m_calcTexCoord;
		Function< Vec3, InVec2, InMat4 > m_calcVSPosition;
		Function< Float, InVec2, InMat4 > m_calcVSDepth;
		Function< Vec3, InVec2, InFloat, InMat4 > m_calcWSPosition;
		Function< Vec3, InFloat, InVec3 > m_applyGamma;
		Function< Vec3, InFloat, InVec3 > m_removeGamma;
		Function< Vec3, InVec2, InVec3, InVec3 > m_getMapNormal;
		Function< Float, InFloat, InMat4 > m_lineariseDepth;
		Function< Vec3, InFloat, InVec3, InFloat > m_fresnelSchlick;
		Function< Vec3, InVec3, InVec3, InVec3, InFloat, InFloat, InVec3, InParam< SamplerCube >, InParam< SamplerCube >, InParam< Sampler2D > > m_computeMetallicIBL;
		Function< Vec3, InVec3, InVec3, InVec3, InVec3, InFloat, InVec3, InParam< SamplerCube >, InParam< SamplerCube >, InParam< Sampler2D > > m_computeSpecularIBL;
	};
}

#endif
