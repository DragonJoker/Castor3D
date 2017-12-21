/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PhongReflection_H___
#define ___C3D_PhongReflection_H___

#include "GlslLighting.hpp"

namespace castor3d
{
	namespace shader
	{
		class PhongReflectionModel
		{
		public:
			C3D_API PhongReflectionModel( glsl::GlslWriter & writer );
			C3D_API glsl::Vec3 computeIncident( glsl::Vec3 const & wsPosition
				, glsl::Vec3 const & wsCamera )const;
			C3D_API glsl::Vec3 computeRefl( glsl::Vec3 const & wsIncident
				, glsl::Vec3 const & wsNormal
				, glsl::Float const & occlusion
				, glsl::SamplerCube const & envMap )const;
			C3D_API glsl::Vec3 computeRefr( glsl::Vec3 const & wsIncident
				, glsl::Vec3 const & wsNormal
				, glsl::Float const & occlusion
				, glsl::SamplerCube const & envMap
				, glsl::Float const & refractionRatio
				, glsl::Vec3 const & diffuse )const;
			C3D_API glsl::Vec3 computeReflRefr( glsl::Vec3 const & wsIncident
				, glsl::Vec3 const & wsNormal
				, glsl::Float const & occlusion
				, glsl::SamplerCube const & envMap
				, glsl::Float const & refractionRatio
				, glsl::Vec3 const & diffuse )const;

		private:
			void doDeclareComputeIncident();
			void doDeclareComputeRefl();
			void doDeclareComputeRefr();
			void doDeclareComputeReflRefr();

		public:
			glsl::GlslWriter & m_writer;
			glsl::Function< glsl::Vec3
				, glsl::InVec3
				, glsl::InVec3 > m_computeIncident;
			glsl::Function< glsl::Vec3
				, glsl::InVec3
				, glsl::InVec3
				, glsl::InFloat
				, glsl::InSamplerCube > m_computeRefl;
			glsl::Function< glsl::Vec3
				, glsl::InVec3
				, glsl::InVec3
				, glsl::InFloat
				, glsl::InSamplerCube
				, glsl::InFloat
				, glsl::InVec3 > m_computeRefr;
			glsl::Function< glsl::Vec3
				, glsl::InVec3
				, glsl::InVec3
				, glsl::InFloat
				, glsl::InSamplerCube
				, glsl::InFloat
				, glsl::InVec3 > m_computeReflRefr;
		};
	}
}

#endif
