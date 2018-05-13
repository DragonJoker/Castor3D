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
			C3D_API explicit MetallicPbrReflectionModel( glsl::GlslWriter & writer );
			C3D_API glsl::Vec3 computeIncident( glsl::Vec3 const & wsPosition
				, glsl::Vec3 const & wsCamera )const;
			C3D_API glsl::Vec3 computeRefl( glsl::Vec3 const & wsIncident
				, glsl::Vec3 const & wsNormal
				, glsl::Float const & occlusion
				, glsl::SamplerCube const & envMap
				, glsl::Vec3 const & ambientLight
				, glsl::Vec3 const & albedo )const;
			C3D_API glsl::Vec3 computeRefr( glsl::Vec3 const & wsIncident
				, glsl::Vec3 const & wsNormal
				, glsl::Float const & occlusion
				, glsl::SamplerCube const & envMap
				, glsl::Float const & refractionRatio
				, glsl::Vec3 const & reflection
				, glsl::Vec3 const & albedo
				, glsl::Float const & roughness )const;

		private:
			void doDeclareComputeIncident();
			void doDeclareComputeRefl();
			void doDeclareComputeRefr();

		public:
			glsl::GlslWriter & m_writer;
			glsl::Function< glsl::Vec3
				, glsl::InVec3
				, glsl::InVec3 > m_computeIncident;
			glsl::Function< glsl::Vec3
				, glsl::InVec3
				, glsl::InVec3
				, glsl::InFloat
				, glsl::InSamplerCube
				, glsl::InVec3
				, glsl::InVec3 > m_computeRefl;
			glsl::Function< glsl::Vec3
				, glsl::InVec3
				, glsl::InVec3
				, glsl::InFloat
				, glsl::InSamplerCube
				, glsl::InFloat
				, glsl::InVec3
				, glsl::InVec3
				, glsl::InFloat > m_computeRefr;
		};
	}
}

#endif
