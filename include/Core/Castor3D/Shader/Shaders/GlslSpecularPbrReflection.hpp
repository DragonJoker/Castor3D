/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SpecularPbrReflectionModel_H___
#define ___C3D_SpecularPbrReflectionModel_H___

#include "Castor3D/Shader/Shaders/GlslLighting.hpp"

namespace castor3d
{
	namespace shader
	{
		class SpecularPbrReflectionModel
		{
		public:
			C3D_API SpecularPbrReflectionModel( sdw::ShaderWriter & writer
				, Utils & utils );
			C3D_API sdw::Vec3 computeIncident( sdw::Vec3 const & wsPosition
				, sdw::Vec3 const & wsCamera )const;
			C3D_API sdw::Vec3 computeRefl( sdw::Vec3 const & wsIncident
				, sdw::Vec3 const & wsNormal
				, sdw::Float const & occlusion
				, sdw::SampledImageCubeRgba32 const & envMap
				, sdw::Vec3 const & ambientLight
				, sdw::Vec3 const & albedo )const;
			C3D_API sdw::Vec3 computeRefrEnvMap( sdw::Vec3 const & wsIncident
				, sdw::Vec3 const & wsNormal
				, sdw::Float const & occlusion
				, sdw::SampledImageCubeRgba32 const & envMap
				, sdw::Float const & refractionRatio
				, sdw::Vec3 const & reflection
				, sdw::Vec3 const & albedo
				, sdw::Float const & glossiness )const;
			C3D_API sdw::Vec3 computeRefrSkybox( sdw::Vec3 const & wsIncident
				, sdw::Vec3 const & wsNormal
				, sdw::Float const & occlusion
				, sdw::SampledImageCubeRgba32 const & envMap
				, sdw::Float const & refractionRatio
				, sdw::Vec3 const & reflection
				, sdw::Vec3 const & albedo
				, sdw::Float const & glossiness )const;

		private:
			void doDeclareComputeIncident();
			void doDeclareComputeRefl();
			void doDeclareComputeRefrEnvMap();
			void doDeclareComputeRefrSkybox();

		public:
			sdw::ShaderWriter & m_writer;
			Utils & m_utils;
			sdw::Function< sdw::Vec3
				, sdw::InVec3
				, sdw::InVec3 > m_computeIncident;
			sdw::Function< sdw::Vec3
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InFloat
				, sdw::InSampledImageCubeRgba32
				, sdw::InVec3
				, sdw::InVec3 > m_computeRefl;
			sdw::Function< sdw::Vec3
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InFloat
				, sdw::InSampledImageCubeRgba32
				, sdw::InFloat
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InFloat > m_computeRefrEnvMap;
			sdw::Function< sdw::Vec3
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InFloat
				, sdw::InSampledImageCubeRgba32
				, sdw::InFloat
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InFloat > m_computeRefrSkybox;
		};
	}
}

#endif
