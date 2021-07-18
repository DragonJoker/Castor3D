/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslToonReflectionModel_H___
#define ___C3D_GlslToonReflectionModel_H___

#include "GlslToonMaterial.hpp"

#include <Castor3D/Shader/Shaders/GlslLighting.hpp>
#include <Castor3D/Shader/Shaders/GlslReflection.hpp>
#include <Castor3D/Shader/Ubos/UbosModule.hpp>

namespace toon::shader
{
	namespace c3d = castor3d::shader;

	class ToonPhongReflectionModel
		: public c3d::ReflectionModel
	{
	public:
		ToonPhongReflectionModel( sdw::ShaderWriter & writer
			, c3d::Utils & utils
			, castor3d::PassFlags const & passFlags
			, uint32_t & envMapBinding
			, uint32_t envMapSet );
		ToonPhongReflectionModel( sdw::ShaderWriter & writer
			, c3d::Utils & utils
			, uint32_t envMapBinding
			, uint32_t envMapSet );
		void computeDeferred( c3d::LightMaterial & material
			, c3d::Surface const & surface
			, c3d::SceneData const & sceneData
			, sdw::SampledImage2DR32 const & depth
			, sdw::SampledImage2DRgba32 const & normal
			, sdw::Int envMapIndex
			, sdw::Int const & reflection
			, sdw::Int const & refraction
			, sdw::Float const & refractionRatio
			, sdw::Vec3 const & transmission
			, sdw::Vec3 & ambient
			, sdw::Vec3 & reflected
			, sdw::Vec3 & refracted )const override;
		void computeForward( c3d::LightMaterial & material
			, c3d::Surface const & surface
			, c3d::SceneData const & sceneData
			, sdw::Float const & refractionRatio
			, sdw::Vec3 const & transmission
			, sdw::Vec3 & ambient
			, sdw::Vec3 & reflected
			, sdw::Vec3 & refracted )const override;

	private:
		sdw::Vec3 computeIncident( sdw::Vec3 const & wsPosition
			, sdw::Vec3 const & wsCamera )const;
		sdw::Vec3 computeRefl( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::SampledImageCubeRgba32 const & envMap
			, ToonPhongLightMaterial const & material )const;
		void computeRefr( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::SampledImageCubeRgba32 const & envMap
			, sdw::Float const & refractionRatio
			, sdw::Vec3 const & transmission
			, ToonPhongLightMaterial const & material
			, sdw::Vec3 & reflection
			, sdw::Vec3 & refraction )const;
		void computeReflRefr( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::SampledImageCubeRgba32 const & envMap
			, sdw::Float const & refractionRatio
			, sdw::Vec3 const & transmission
			, ToonPhongLightMaterial const & material
			, sdw::Vec3 & reflection
			, sdw::Vec3 & refraction )const;
		sdw::Vec3 computeRefls( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::SampledImageCubeArrayRgba32 const & envMap
			, sdw::Int const & envMapIndex
			, ToonPhongLightMaterial const & material )const;
		void computeRefrs( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::SampledImageCubeArrayRgba32 const & envMap
			, sdw::Int const & envMapIndex
			, sdw::Float const & refractionRatio
			, sdw::Vec3 const & transmission
			, ToonPhongLightMaterial const & material
			, sdw::Vec3 & reflection
			, sdw::Vec3 & refraction )const;
		void computeReflRefrs( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::SampledImageCubeArrayRgba32 const & envMap
			, sdw::Int const & envMapIndex
			, sdw::Float const & refractionRatio
			, sdw::Vec3 const & transmission
			, ToonPhongLightMaterial const & material
			, sdw::Vec3 & reflection
			, sdw::Vec3 & refraction )const;
		void doDeclareComputeRefl();
		void doDeclareComputeRefr();
		void doDeclareComputeReflRefr();
		void doDeclareComputeRefls();
		void doDeclareComputeRefrs();
		void doDeclareComputeReflRefrs();

	private:
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InSampledImageCubeRgba32
			, InToonPhongLightMaterial > m_computeRefl;
		sdw::Function< sdw::Void
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InSampledImageCubeRgba32
			, sdw::InFloat
			, sdw::InVec3
			, InToonPhongLightMaterial
			, sdw::InOutVec3
			, sdw::OutVec3 > m_computeRefr;
		sdw::Function< sdw::Void
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InSampledImageCubeRgba32
			, sdw::InFloat
			, sdw::InVec3
			, InToonPhongLightMaterial
			, sdw::OutVec3
			, sdw::OutVec3 > m_computeReflRefr;
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InSampledImageCubeArrayRgba32
			, sdw::InInt
			, InToonPhongLightMaterial > m_computeRefls;
		sdw::Function< sdw::Void
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InSampledImageCubeArrayRgba32
			, sdw::InInt
			, sdw::InFloat
			, sdw::InVec3
			, InToonPhongLightMaterial
			, sdw::InOutVec3
			, sdw::OutVec3 > m_computeRefrs;
		sdw::Function< sdw::Void
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InSampledImageCubeArrayRgba32
			, sdw::InInt
			, sdw::InFloat
			, sdw::InVec3
			, InToonPhongLightMaterial
			, sdw::OutVec3
			, sdw::OutVec3 > m_computeReflRefrs;
	};

	class ToonPbrReflectionModel
		: public c3d::ReflectionModel
	{
	public:
		ToonPbrReflectionModel( sdw::ShaderWriter & writer
			, c3d::Utils & utils
			, castor3d::PassFlags const & passFlags
			, uint32_t & envMapBinding
			, uint32_t envMapSet );
		ToonPbrReflectionModel( sdw::ShaderWriter & writer
			, c3d::Utils & utils
			, uint32_t envMapBinding
			, uint32_t envMapSet );
		void computeDeferred( c3d::LightMaterial & material
			, c3d::Surface const & surface
			, c3d::SceneData const & sceneData
			, sdw::SampledImage2DR32 const & depth
			, sdw::SampledImage2DRgba32 const & normal
			, sdw::Int envMapIndex
			, sdw::Int const & reflection
			, sdw::Int const & refraction
			, sdw::Float const & refractionRatio
			, sdw::Vec3 const & transmission
			, sdw::Vec3 & ambient
			, sdw::Vec3 & reflected
			, sdw::Vec3 & refracted )const override;
		void computeForward( c3d::LightMaterial & material
			, c3d::Surface const & surface
			, c3d::SceneData const & sceneData
			, sdw::Float const & refractionRatio
			, sdw::Vec3 const & transmission
			, sdw::Vec3 & ambient
			, sdw::Vec3 & reflected
			, sdw::Vec3 & refracted )const override;

	private:
		sdw::Vec3 computeIBL( c3d::Surface surface
			, ToonPbrLightMaterial const & material
			, sdw::Vec3 const & worldEye
			, sdw::SampledImageCubeRgba32 const & irradiance
			, sdw::SampledImageCubeRgba32 const & prefiltered
			, sdw::SampledImage2DRgba32 const & brdf )const;
		sdw::Vec3 computeIncident( sdw::Vec3 const & wsPosition
			, sdw::Vec3 const & wsCamera )const;
		sdw::Vec3 computeReflEnvMap( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::SampledImageCubeRgba32 const & envMap
			, ToonPbrLightMaterial const & material )const;
		sdw::Void computeRefrEnvMap( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::SampledImageCubeRgba32 const & envMap
			, sdw::Float const & refractionRatio
			, sdw::Vec3 const & transmission
			, ToonPbrLightMaterial const & material
			, sdw::Vec3 & reflection
			, sdw::Vec3 & refraction )const;
		sdw::Vec3 computeReflEnvMaps( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::SampledImageCubeArrayRgba32 const & envMap
			, sdw::Int const & envMapIndex
			, ToonPbrLightMaterial const & material )const;
		sdw::Void computeRefrEnvMaps( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::SampledImageCubeArrayRgba32 const & envMap
			, sdw::Int const & envMapIndex
			, sdw::Float const & refractionRatio
			, sdw::Vec3 const & transmission
			, ToonPbrLightMaterial const & material
			, sdw::Vec3 & reflection
			, sdw::Vec3 & refraction )const;
		sdw::Void computeRefrSkybox( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::SampledImageCubeRgba32 const & envMap
			, sdw::Float const & refractionRatio
			, sdw::Vec3 const & transmission
			, ToonPbrLightMaterial const & material
			, sdw::Vec3 & reflection
			, sdw::Vec3 & refraction )const;
		void doDeclareComputeIBL();
		void doDeclareComputeReflEnvMap();
		void doDeclareComputeRefrEnvMap();
		void doDeclareComputeReflEnvMaps();
		void doDeclareComputeRefrEnvMaps();
		void doDeclareComputeRefrSkybox();

	private:
		sdw::Function< sdw::Vec3
			, c3d::InSurface
			, InToonPbrLightMaterial
			, sdw::InVec3
			, sdw::InSampledImageCubeRgba32
			, sdw::InSampledImageCubeRgba32
			, sdw::InSampledImage2DRgba32 > m_computeIBL;
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InSampledImageCubeRgba32
			, InToonPbrLightMaterial > m_computeReflEnvMap;
		sdw::Function< sdw::Void
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InSampledImageCubeRgba32
			, sdw::InFloat
			, sdw::InVec3
			, InToonPbrLightMaterial
			, sdw::InOutVec3
			, sdw::OutVec3 > m_computeRefrEnvMap;
		sdw::Function< sdw::Void
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InSampledImageCubeRgba32
			, sdw::InFloat
			, sdw::InVec3
			, InToonPbrLightMaterial
			, sdw::InOutVec3
			, sdw::OutVec3 > m_computeRefrSkybox;
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InSampledImageCubeArrayRgba32
			, sdw::InInt
			, InToonPbrLightMaterial > m_computeReflEnvMaps;
		sdw::Function< sdw::Void
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InSampledImageCubeArrayRgba32
			, sdw::InInt
			, sdw::InFloat
			, sdw::InVec3
			, InToonPbrLightMaterial
			, sdw::InOutVec3
			, sdw::OutVec3 > m_computeRefrEnvMaps;
	};
}

#endif
