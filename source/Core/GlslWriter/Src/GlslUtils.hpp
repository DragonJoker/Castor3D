/*
See LICENSE file in root folder
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
		GlslWriter_API void declareCalcWSPosition();
		GlslWriter_API void declareApplyGamma();
		GlslWriter_API void declareRemoveGamma();
		GlslWriter_API void declareLineariseDepth();
		GlslWriter_API void declareGetMapNormal();
		GlslWriter_API void declareFresnelSchlick();
		GlslWriter_API void declareComputeMetallicIBL();
		GlslWriter_API void declareComputeSpecularIBL();
		GlslWriter_API Vec2 calcTexCoord( Vec2 const & renderSize );
		GlslWriter_API Vec3 calcVSPosition( Vec2 const & uv
			, Float const & depth
			, Mat4 const & invProj );
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
			, Float const & nearPlane
			, Float const & farPlane );
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
		Function< Vec2
			, InVec2 > m_calcTexCoord;
		Function< Vec3
			, InVec2
			, InFloat
			, InMat4 > m_calcVSPosition;
		Function< Vec3
			, InVec2
			, InFloat
			, InMat4 > m_calcWSPosition;
		Function< Vec3
			, InFloat
			, InVec3 > m_applyGamma;
		Function< Vec3
			, InFloat
			, InVec3 > m_removeGamma;
		Function< Vec3
			, InVec2
			, InVec3
			, InVec3 > m_getMapNormal;
		Function< Float
			, InFloat
			, InFloat
			, InFloat > m_lineariseDepth;
		Function< Vec3
			, InFloat
			, InVec3
			, InFloat > m_fresnelSchlick;
		Function< Vec3
			, InVec3
			, InVec3
			, InVec3
			, InFloat
			, InFloat
			, InVec3
			, InParam< SamplerCube >
			, InParam< SamplerCube >
			, InParam< Sampler2D > > m_computeMetallicIBL;
		Function< Vec3
			, InVec3
			, InVec3
			, InVec3
			, InVec3
			, InFloat
			, InVec3
			, InParam< SamplerCube >
			, InParam< SamplerCube >
			, InParam< Sampler2D > > m_computeSpecularIBL;
	};
}

#endif
