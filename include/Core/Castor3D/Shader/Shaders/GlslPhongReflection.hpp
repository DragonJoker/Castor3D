/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PhongReflection_H___
#define ___C3D_PhongReflection_H___

#include "Castor3D/Shader/Shaders/GlslLighting.hpp"

namespace castor3d
{
	namespace shader
	{
		class PhongReflectionModel
		{
		public:
			C3D_API explicit PhongReflectionModel( sdw::ShaderWriter & writer
				, Utils & utils );
			C3D_API sdw::Vec3 computeIncident( sdw::Vec3 const & wsPosition
				, sdw::Vec3 const & wsCamera )const;
			C3D_API sdw::Vec3 computeRefl( sdw::Vec3 const & wsIncident
				, sdw::Vec3 const & wsNormal
				, sdw::Float const & occlusion
				, sdw::SampledImageCubeRgba32 const & envMap )const;
			C3D_API sdw::Vec3 computeRefr( sdw::Vec3 const & wsIncident
				, sdw::Vec3 const & wsNormal
				, sdw::Float const & occlusion
				, sdw::SampledImageCubeRgba32 const & envMap
				, sdw::Float const & refractionRatio
				, sdw::Vec3 const & diffuse
				, sdw::Float const & shininess )const;
			C3D_API sdw::Vec3 computeReflRefr( sdw::Vec3 const & wsIncident
				, sdw::Vec3 const & wsNormal
				, sdw::Float const & occlusion
				, sdw::SampledImageCubeRgba32 const & envMap
				, sdw::Float const & refractionRatio
				, sdw::Vec3 const & diffuse
				, sdw::Float const & shininess )const;

		private:
			void doDeclareComputeIncident();
			void doDeclareComputeRefl();
			void doDeclareComputeRefr();
			void doDeclareComputeReflRefr();

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
				, sdw::InSampledImageCubeRgba32 > m_computeRefl;
			sdw::Function< sdw::Vec3
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InFloat
				, sdw::InSampledImageCubeRgba32
				, sdw::InFloat
				, sdw::InVec3
				, sdw::InFloat > m_computeRefr;
			sdw::Function< sdw::Vec3
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InFloat
				, sdw::InSampledImageCubeRgba32
				, sdw::InFloat
				, sdw::InVec3
				, sdw::InFloat > m_computeReflRefr;
		};
	}
}

#endif
