/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslPhongReflectionModel_H___
#define ___C3D_GlslPhongReflectionModel_H___

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
			C3D_API void computeRefl( sdw::Vec3 const & wsIncident
				, sdw::Vec3 const & wsNormal
				, sdw::Float const & occlusion
				, sdw::SampledImageCubeRgba32 const & envMap
				, sdw::Float const & shininess
				, sdw::Vec3 & ambient
				, sdw::Vec3 & diffuse )const;
			C3D_API void computeRefr( sdw::Vec3 const & wsIncident
				, sdw::Vec3 const & wsNormal
				, sdw::Float const & occlusion
				, sdw::SampledImageCubeRgba32 const & envMap
				, sdw::Float const & refractionRatio
				, sdw::Float const & shininess
				, sdw::Vec3 & ambient
				, sdw::Vec3 & diffuse )const;
			C3D_API void computeReflRefr( sdw::Vec3 const & wsIncident
				, sdw::Vec3 const & wsNormal
				, sdw::Float const & occlusion
				, sdw::SampledImageCubeRgba32 const & envMap
				, sdw::Float const & refractionRatio
				, sdw::Float const & shininess
				, sdw::Vec3 & ambient
				, sdw::Vec3 & diffuse )const;

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
			sdw::Function< sdw::Void
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InFloat
				, sdw::InSampledImageCubeRgba32
				, sdw::InFloat
				, sdw::OutVec3
				, sdw::OutVec3 > m_computeRefl;
			sdw::Function< sdw::Void
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InFloat
				, sdw::InSampledImageCubeRgba32
				, sdw::InFloat
				, sdw::InFloat
				, sdw::OutVec3
				, sdw::OutVec3 > m_computeRefr;
			sdw::Function< sdw::Void
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InFloat
				, sdw::InSampledImageCubeRgba32
				, sdw::InFloat
				, sdw::InFloat
				, sdw::OutVec3
				, sdw::OutVec3 > m_computeReflRefr;
		};
	}
}

#endif
