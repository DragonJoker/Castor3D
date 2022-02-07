/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslPhongReflectionModel_H___
#define ___C3D_GlslPhongReflectionModel_H___

#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslReflection.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

namespace castor3d::shader
{
	class PhongReflectionModel
		: public ReflectionModel
	{
	public:
		C3D_API PhongReflectionModel( sdw::ShaderWriter & writer
			, Utils & utils
			, PassFlags const & passFlags
			, uint32_t & envMapBinding
			, uint32_t envMapSet );
		C3D_API PhongReflectionModel( sdw::ShaderWriter & writer
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
		sdw::Vec3 computeIncident( sdw::Vec3 const & wsPosition
			, sdw::Vec3 const & wsCamera )const;
		sdw::Vec3 computeRefl( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::CombinedImageCubeRgba32 const & envMap
			, PhongLightMaterial const & material );
		sdw::Vec3 computeRefr( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::CombinedImageCubeRgba32 const & envMap
			, sdw::Float const & refractionRatio
			, sdw::Vec3 const & transmission
			, PhongLightMaterial & material );
		void computeRefr( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::CombinedImageCubeRgba32 const & envMap
			, sdw::Float const & refractionRatio
			, sdw::Vec3 const & transmission
			, PhongLightMaterial & material
			, sdw::Vec3 & reflection
			, sdw::Vec3 & refraction );
		void computeReflRefr( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::CombinedImageCubeRgba32 const & envMap
			, sdw::Float const & refractionRatio
			, sdw::Vec3 const & transmission
			, PhongLightMaterial & material
			, sdw::Vec3 & reflection
			, sdw::Vec3 & refraction );
		sdw::Vec3 computeRefls( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::CombinedImageCubeArrayRgba32 const & envMap
			, sdw::Int const & envMapIndex
			, PhongLightMaterial const & material );
		sdw::Vec3 computeRefrs( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::CombinedImageCubeArrayRgba32 const & envMap
			, sdw::Int const & envMapIndex
			, sdw::Float const & refractionRatio
			, sdw::Vec3 const & transmission
			, PhongLightMaterial & material );
		void computeRefrs( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::CombinedImageCubeArrayRgba32 const & envMap
			, sdw::Int const & envMapIndex
			, sdw::Float const & refractionRatio
			, sdw::Vec3 const & transmission
			, PhongLightMaterial & material
			, sdw::Vec3 & reflection
			, sdw::Vec3 & refraction );
		void computeReflRefrs( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::CombinedImageCubeArrayRgba32 const & envMap
			, sdw::Int const & envMapIndex
			, sdw::Float const & refractionRatio
			, sdw::Vec3 const & transmission
			, PhongLightMaterial & material
			, sdw::Vec3 & reflection
			, sdw::Vec3 & refraction );
		void doDeclareComputeRefl();
		void doDeclareComputeRefr();
		void doDeclareComputeMergeRefr();
		void doDeclareComputeReflRefr();
		void doDeclareComputeRefls();
		void doDeclareComputeRefrs();
		void doDeclareComputeMergeRefrs();
		void doDeclareComputeReflRefrs();

	private:
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InCombinedImageCubeRgba32
			, InPhongLightMaterial > m_computeRefl;
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InCombinedImageCubeRgba32
			, sdw::InFloat
			, sdw::InVec3
			, InOutPhongLightMaterial > m_computeRefr;
		sdw::Function< sdw::Void
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InCombinedImageCubeRgba32
			, sdw::InFloat
			, sdw::InVec3
			, InOutPhongLightMaterial
			, sdw::InOutVec3
			, sdw::OutVec3 > m_computeMergeRefr;
		sdw::Function< sdw::Void
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InCombinedImageCubeRgba32
			, sdw::InFloat
			, sdw::InVec3
			, InOutPhongLightMaterial
			, sdw::OutVec3
			, sdw::OutVec3 > m_computeReflRefr;
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InCombinedImageCubeArrayRgba32
			, sdw::InInt
			, InPhongLightMaterial > m_computeRefls;
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InCombinedImageCubeArrayRgba32
			, sdw::InInt
			, sdw::InFloat
			, sdw::InVec3
			, InOutPhongLightMaterial > m_computeRefrs;
		sdw::Function< sdw::Void
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InCombinedImageCubeArrayRgba32
			, sdw::InInt
			, sdw::InFloat
			, sdw::InVec3
			, InOutPhongLightMaterial
			, sdw::InOutVec3
			, sdw::OutVec3 > m_computeMergeRefrs;
		sdw::Function< sdw::Void
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InCombinedImageCubeArrayRgba32
			, sdw::InInt
			, sdw::InFloat
			, sdw::InVec3
			, InOutPhongLightMaterial
			, sdw::OutVec3
			, sdw::OutVec3 > m_computeReflRefrs;
	};
}

#endif
