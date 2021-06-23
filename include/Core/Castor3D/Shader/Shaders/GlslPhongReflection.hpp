/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslPhongReflectionModel_H___
#define ___C3D_GlslPhongReflectionModel_H___

#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

namespace castor3d::shader
{
	class PhongReflectionModel
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
			, uint32_t envMapSet
			, uint32_t maxEnvMapsCount );
		C3D_API void computeDeferred( sdw::Int envMapIndex
			, sdw::Int const & reflection
			, sdw::Int const & refraction
			, sdw::Float const & refractionRatio
			, sdw::Vec3 const & specular
			, sdw::Float const & shininess
			, sdw::Vec3 const & transmission
			, Surface const & surface
			, SceneData const & sceneData
			, sdw::Vec3 & ambient
			, sdw::Vec3 & diffuse
			, sdw::Vec3 & reflected
			, sdw::Vec3 & refracted )const;
		C3D_API void computeForward( sdw::Float const & refractionRatio
			, sdw::Vec3 const & specular
			, sdw::Float const & shininess
			, sdw::Vec3 const & transmission
			, Surface const & surface
			, SceneData const & sceneData
			, sdw::Vec3 & ambient
			, sdw::Vec3 & diffuse
			, sdw::Vec3 & reflected
			, sdw::Vec3 & refracted )const;

	private:
		sdw::Vec3 computeIncident( sdw::Vec3 const & wsPosition
			, sdw::Vec3 const & wsCamera )const;
		sdw::Vec3 computeRefl( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::SampledImageCubeRgba32 const & envMap
			, sdw::Float const & shininess
			, sdw::Vec3 const & specular )const;
		void computeRefr( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::SampledImageCubeRgba32 const & envMap
			, sdw::Float const & refractionRatio
			, sdw::Vec3 const & transmission
			, sdw::Float const & shininess
			, sdw::Vec3 & reflection
			, sdw::Vec3 & refraction )const;
		void computeReflRefr( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::SampledImageCubeRgba32 const & envMap
			, sdw::Float const & refractionRatio
			, sdw::Vec3 const & specular
			, sdw::Vec3 const & transmission
			, sdw::Float const & shininess
			, sdw::Vec3 & reflection
			, sdw::Vec3 & refraction )const;
		void doDeclareComputeRefl();
		void doDeclareComputeRefr();
		void doDeclareComputeReflRefr();

	private:
		sdw::ShaderWriter & m_writer;
		Utils & m_utils;
		uint32_t m_maxEnvMapsCount;
		PassFlags m_passFlags;
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InSampledImageCubeRgba32
			, sdw::InFloat
			, sdw::InVec3 > m_computeRefl;
		sdw::Function< sdw::Void
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InSampledImageCubeRgba32
			, sdw::InFloat
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InOutVec3
			, sdw::OutVec3 > m_computeRefr;
		sdw::Function< sdw::Void
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InSampledImageCubeRgba32
			, sdw::InFloat
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InFloat
			, sdw::OutVec3
			, sdw::OutVec3 > m_computeReflRefr;
	};
}

#endif
