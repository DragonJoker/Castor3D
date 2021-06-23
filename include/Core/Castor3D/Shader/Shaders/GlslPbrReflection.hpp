/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslMetallicPbrReflectionModel_H___
#define ___C3D_GlslMetallicPbrReflectionModel_H___

#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

namespace castor3d::shader
{
	class PbrReflectionModel
	{
	public:
		C3D_API PbrReflectionModel( sdw::ShaderWriter & writer
			, Utils & utils
			, PassFlags const & passFlags
			, uint32_t & envMapBinding
			, uint32_t envMapSet );
		C3D_API PbrReflectionModel( sdw::ShaderWriter & writer
			, Utils & utils
			, uint32_t envMapBinding
			, uint32_t envMapSet
			, uint32_t maxEnvMapsCount );
		C3D_API void computeDeferred( sdw::SampledImage2DRgba32 const & brdf
			, sdw::SampledImageCubeRgba32 const & irradiance
			, sdw::SampledImageCubeRgba32 const & prefiltered
			, sdw::Int envMapIndex
			, sdw::Int const & reflection
			, sdw::Int const & refraction
			, sdw::Float const & refractionRatio
			, sdw::Vec3 const & albedo
			, sdw::Vec3 const & specular
			, sdw::Float const & roughness
			, sdw::Float const & metalness
			, sdw::Vec3 const & transmission
			, Surface const & surface
			, SceneData const & sceneData
			, sdw::Vec3 & ambient
			, sdw::Vec3 & reflected
			, sdw::Vec3 & refracted )const;
		C3D_API void computeForward( sdw::SampledImage2DRgba32 const & brdf
			, sdw::SampledImageCubeRgba32 const & irradiance
			, sdw::SampledImageCubeRgba32 const & prefiltered
			, sdw::Float const & refractionRatio
			, sdw::Vec3 const & albedo
			, sdw::Vec3 const & specular
			, sdw::Float const & roughness
			, sdw::Float const & metalness
			, sdw::Vec3 const & transmission
			, Surface const & surface
			, SceneData const & sceneData
			, sdw::Vec3 & ambient
			, sdw::Vec3 & reflected
			, sdw::Vec3 & refracted )const;
		sdw::Vec3 computeSpecular( sdw::Vec3 const & albedo
			, sdw::Float const & metalness )const;

	private:
		sdw::Vec3 computeIBL( Surface surface
			, sdw::Vec3 const & diffuse
			, sdw::Vec3 const & specular
			, sdw::Float const & roughness
			, sdw::Float const & metalness
			, sdw::Vec3 const & worldEye
			, sdw::SampledImageCubeRgba32 const & irradiance
			, sdw::SampledImageCubeRgba32 const & prefiltered
			, sdw::SampledImage2DRgba32 const & brdf )const;
		sdw::Vec3 computeIncident( sdw::Vec3 const & wsPosition
			, sdw::Vec3 const & wsCamera )const;
		sdw::Vec3 computeReflEnvMap( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::SampledImageCubeRgba32 const & envMap
			, sdw::Vec3 const & specular
			, sdw::Float const & roughness )const;
		sdw::Void computeRefrEnvMap( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::SampledImageCubeRgba32 const & envMap
			, sdw::Float const & refractionRatio
			, sdw::Vec3 const & transmission
			, sdw::Float const & roughness
			, sdw::Vec3 & reflection
			, sdw::Vec3 & refraction )const;
		sdw::Void computeRefrSkybox( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::SampledImageCubeRgba32 const & envMap
			, sdw::Float const & refractionRatio
			, sdw::Vec3 const & transmission
			, sdw::Float const & roughness
			, sdw::Vec3 & reflection
			, sdw::Vec3 & refraction )const;
		void doDeclareComputeIBL();
		void doDeclareComputeReflEnvMap();
		void doDeclareComputeRefrEnvMap();
		void doDeclareComputeRefrSkybox();

	public:
		C3D_API static uint32_t const MaxIblReflectionLod;

	private:
		sdw::ShaderWriter & m_writer;
		Utils & m_utils;
		uint32_t m_maxEnvMapsCount;
		PassFlags m_passFlags;
		sdw::Function< sdw::Vec3
			, InSurface
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InVec3
			, sdw::InSampledImageCubeRgba32
			, sdw::InSampledImageCubeRgba32
			, sdw::InSampledImage2DRgba32 > m_computeIBL;
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InSampledImageCubeRgba32
			, sdw::InVec3
			, sdw::InFloat > m_computeReflEnvMap;
		sdw::Function< sdw::Void
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InSampledImageCubeRgba32
			, sdw::InFloat
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InOutVec3
			, sdw::OutVec3 > m_computeRefrEnvMap;
		sdw::Function< sdw::Void
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InSampledImageCubeRgba32
			, sdw::InFloat
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InOutVec3
			, sdw::OutVec3 > m_computeRefrSkybox;
	};
}

#endif
