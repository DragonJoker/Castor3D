/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslMetallicPbrReflectionModel_H___
#define ___C3D_GlslMetallicPbrReflectionModel_H___

#include "Castor3D/Shader/Shaders/GlslLighting.hpp"

namespace castor3d
{
	namespace shader
	{
		class MetallicPbrReflectionModel
		{
		public:
			C3D_API MetallicPbrReflectionModel( sdw::ShaderWriter & writer
				, Utils & utils );
			C3D_API sdw::Vec3 computeIncident( sdw::Vec3 const & wsPosition
				, sdw::Vec3 const & wsCamera )const;
			C3D_API sdw::Vec3 computeRefl( sdw::Vec3 const & wsIncident
				, sdw::Vec3 const & wsNormal
				, sdw::SampledImageCubeRgba32 const & envMap
				, sdw::Vec3 const & albedo )const;
			C3D_API sdw::Void computeRefrEnvMap( sdw::Vec3 const & wsIncident
				, sdw::Vec3 const & wsNormal
				, sdw::SampledImageCubeRgba32 const & envMap
				, sdw::Float const & refractionRatio
				, sdw::Vec3 const & albedo
				, sdw::Float const & roughness
				, sdw::Vec3 & reflection
				, sdw::Vec3 & refraction )const;
			C3D_API sdw::Void computeRefrSkybox( sdw::Vec3 const & wsIncident
				, sdw::Vec3 const & wsNormal
				, sdw::SampledImageCubeRgba32 const & envMap
				, sdw::Float const & refractionRatio
				, sdw::Vec3 const & albedo
				, sdw::Float const & roughness
				, sdw::Vec3 & reflection
				, sdw::Vec3 & refraction )const;

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
				, sdw::InSampledImageCubeRgba32
				, sdw::InVec3 > m_computeRefl;
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
}

#endif
