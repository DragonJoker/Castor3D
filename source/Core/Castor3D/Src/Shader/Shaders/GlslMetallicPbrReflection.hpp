/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MetallicPbrReflectionModel_H___
#define ___C3D_MetallicPbrReflectionModel_H___

#include "GlslLighting.hpp"

namespace castor3d
{
	namespace shader
	{
		class MetallicPbrReflectionModel
		{
		public:
			C3D_API explicit MetallicPbrReflectionModel( sdw::ShaderWriter & writer );
			C3D_API sdw::Vec3 computeIncident( sdw::Vec3 const & wsPosition
				, sdw::Vec3 const & wsCamera )const;
			C3D_API sdw::Vec3 computeRefl( sdw::Vec3 const & wsIncident
				, sdw::Vec3 const & wsNormal
				, sdw::Float const & occlusion
				, sdw::SampledImageCubeRgba32 const & envMap
				, sdw::Vec3 const & ambientLight
				, sdw::Vec3 const & albedo )const;
			C3D_API sdw::Vec3 computeRefr( sdw::Vec3 const & wsIncident
				, sdw::Vec3 const & wsNormal
				, sdw::Float const & occlusion
				, sdw::SampledImageCubeRgba32 const & envMap
				, sdw::Float const & refractionRatio
				, sdw::Vec3 const & reflection
				, sdw::Vec3 const & albedo
				, sdw::Float const & roughness )const;

		private:
			void doDeclareComputeIncident();
			void doDeclareComputeRefl();
			void doDeclareComputeRefr();

		public:
			sdw::ShaderWriter & m_writer;
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
				, sdw::InFloat > m_computeRefr;
		};
	}
}

#endif
