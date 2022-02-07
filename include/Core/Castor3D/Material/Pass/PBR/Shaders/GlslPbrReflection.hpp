/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslMetallicPbrReflectionModel_H___
#define ___C3D_GlslMetallicPbrReflectionModel_H___

#include "Castor3D/Shader/Shaders/GlslCookTorranceBRDF.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslReflection.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

namespace castor3d::shader
{
	class PbrReflectionModel
		: public ReflectionModel
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
			, uint32_t envMapSet );
		C3D_API void computeDeferred( LightMaterial & material
			, Surface const & surface
			, SceneData const & sceneData
			, sdw::Int envMapIndex
			, sdw::Int const & reflection
			, sdw::Int const & refraction
			, sdw::Float const & refractionRatio
			, sdw::Vec3 const & transmission
			, sdw::Vec3 & ambient
			, sdw::Vec3 & reflected
			, sdw::Vec3 & refracted )override;
		C3D_API sdw::Vec3 computeForward( LightMaterial & material
			, Surface const & surface
			, SceneData const & sceneData )override;
		C3D_API sdw::Vec3 computeForward( LightMaterial & material
			, Surface const & surface
			, SceneData const & sceneData
			, sdw::Float const & refractionRatio
			, sdw::Vec3 const & transmission )override;
		C3D_API void computeForward( LightMaterial & material
			, Surface const & surface
			, SceneData const & sceneData
			, sdw::Float const & refractionRatio
			, sdw::Vec3 const & transmission
			, sdw::Vec3 & ambient
			, sdw::Vec3 & reflected
			, sdw::Vec3 & refracted )override;

	private:
		sdw::Vec3 computeIBL( Surface surface
			, PbrLightMaterial const & material
			, sdw::Vec3 const & worldEye
			, sdw::CombinedImageCubeRgba32 const & irradiance
			, sdw::CombinedImageCubeRgba32 const & prefiltered
			, sdw::CombinedImage2DRgba32 const & brdf );
		sdw::Vec3 computeIncident( sdw::Vec3 const & wsPosition
			, sdw::Vec3 const & wsCamera )const;
		sdw::Vec3 computeReflEnvMap( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::CombinedImageCubeRgba32 const & envMap
			, PbrLightMaterial const & material );
		sdw::Void computeRefrEnvMap( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::CombinedImageCubeRgba32 const & envMap
			, sdw::Float const & refractionRatio
			, sdw::Vec3 const & transmission
			, PbrLightMaterial const & material
			, sdw::Vec3 & reflection
			, sdw::Vec3 & refraction );
		sdw::Vec3 computeRefrEnvMap( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::CombinedImageCubeRgba32 const & envMap
			, sdw::Float const & refractionRatio
			, sdw::Vec3 const & transmission
			, PbrLightMaterial const & material );
		sdw::Vec3 computeReflEnvMaps( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::CombinedImageCubeArrayRgba32 const & envMap
			, sdw::Int const & envMapIndex
			, PbrLightMaterial const & material );
		sdw::Vec3 computeRefrEnvMaps( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::CombinedImageCubeArrayRgba32 const & envMap
			, sdw::Int const & envMapIndex
			, sdw::Float const & refractionRatio
			, sdw::Vec3 const & transmission
			, PbrLightMaterial const & material );
		sdw::Void computeRefrEnvMaps( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::CombinedImageCubeArrayRgba32 const & envMap
			, sdw::Int const & envMapIndex
			, sdw::Float const & refractionRatio
			, sdw::Vec3 const & transmission
			, PbrLightMaterial const & material
			, sdw::Vec3 & reflection
			, sdw::Vec3 & refraction );
		sdw::Void computeRefrSkybox( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::CombinedImageCubeRgba32 const & envMap
			, sdw::Float const & refractionRatio
			, sdw::Vec3 const & transmission
			, PbrLightMaterial const & material
			, sdw::Vec3 & reflection
			, sdw::Vec3 & refraction );
		sdw::Vec3 computeRefrSkybox( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::CombinedImageCubeRgba32 const & envMap
			, sdw::Float const & refractionRatio
			, sdw::Vec3 const & transmission
			, PbrLightMaterial const & material );
		void doDeclareComputeIBL();
		void doDeclareComputeReflEnvMap();
		void doDeclareComputeRefrEnvMap();
		void doDeclareComputeMergeRefrEnvMap();
		void doDeclareComputeReflEnvMaps();
		void doDeclareComputeRefrEnvMaps();
		void doDeclareComputeMergeRefrEnvMaps();
		void doDeclareComputeRefrSkybox();
		void doDeclareComputeMergeRefrSkybox();

	private:
		sdw::Function< sdw::Vec3
			, InSurface
			, InPbrLightMaterial
			, sdw::InVec3
			, sdw::InCombinedImageCubeRgba32
			, sdw::InCombinedImageCubeRgba32
			, sdw::InCombinedImage2DRgba32 > m_computeIBL;
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InCombinedImageCubeRgba32
			, InPbrLightMaterial > m_computeReflEnvMap;
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InCombinedImageCubeRgba32
			, sdw::InFloat
			, sdw::InVec3
			, InPbrLightMaterial > m_computeRefrEnvMap;
		sdw::Function< sdw::Void
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InCombinedImageCubeRgba32
			, sdw::InFloat
			, sdw::InVec3
			, InPbrLightMaterial
			, sdw::InOutVec3
			, sdw::OutVec3 > m_computeMergeRefrEnvMap;
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InCombinedImageCubeRgba32
			, sdw::InFloat
			, sdw::InVec3
			, InPbrLightMaterial > m_computeRefrSkybox;
		sdw::Function< sdw::Void
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InCombinedImageCubeRgba32
			, sdw::InFloat
			, sdw::InVec3
			, InPbrLightMaterial
			, sdw::InOutVec3
			, sdw::OutVec3 > m_computeMergeRefrSkybox;
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InCombinedImageCubeArrayRgba32
			, sdw::InInt
			, InPbrLightMaterial > m_computeReflEnvMaps;
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InCombinedImageCubeArrayRgba32
			, sdw::InInt
			, sdw::InFloat
			, sdw::InVec3
			, InPbrLightMaterial > m_computeRefrEnvMaps;
		sdw::Function< sdw::Void
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InCombinedImageCubeArrayRgba32
			, sdw::InInt
			, sdw::InFloat
			, sdw::InVec3
			, InPbrLightMaterial
			, sdw::InOutVec3
			, sdw::OutVec3 > m_computeMergeRefrEnvMaps;
	};
}

#endif
