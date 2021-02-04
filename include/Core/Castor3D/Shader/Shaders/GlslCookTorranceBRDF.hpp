/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslCookTorranceBRDF_H___
#define ___C3D_GlslCookTorranceBRDF_H___

#include "Castor3D/Shader/Shaders/GlslLighting.hpp"

namespace castor3d
{
	namespace shader
	{
		class CookTorranceBRDF
		{
		public:
			C3D_API explicit CookTorranceBRDF( sdw::ShaderWriter & writer );
			C3D_API void declare();
			C3D_API void declareDiffuse();
			C3D_API void compute( Light const & light
				, sdw::Vec3 const & worldEye
				, sdw::Vec3 const & direction
				, sdw::Vec3 const & albedo
				, sdw::Float const & metallic
				, sdw::Float const & roughness
				, Surface const & surface
				, OutputComponents & output );
			C3D_API void compute( Light const & light
				, sdw::Vec3 const & worldEye
				, sdw::Vec3 const & direction
				, sdw::Vec3 const & specular
				, sdw::Float const & roughness
				, Surface const & surface
				, OutputComponents & output );
			C3D_API sdw::Vec3 computeDiffuse( Light const & light
				, sdw::Vec3 const & worldEye
				, sdw::Vec3 const & direction
				, sdw::Vec3 const & albedo
				, sdw::Float const & metallic
				, Surface const & surface );
			C3D_API sdw::Vec3 computeDiffuse( Light const & light
				, sdw::Vec3 const & worldEye
				, sdw::Vec3 const & direction
				, sdw::Vec3 const & specular
				, Surface const & surface );

		protected:
			void doDeclareDistribution();
			void doDeclareGeometry();
			void doDeclareFresnelShlick();
			void doDeclareComputeCookTorrance();
			void doDeclareComputeCookTorranceDiffuse();

		public:
			sdw::ShaderWriter & m_writer;
			sdw::Function< sdw::Float
				, sdw::InFloat
				, sdw::InFloat > m_distributionGGX;
			sdw::Function< sdw::Float
				, sdw::InFloat
				, sdw::InFloat > m_geometrySchlickGGX;
			sdw::Function< sdw::Float
				, sdw::InFloat
				, sdw::InFloat
				, sdw::InFloat > m_geometrySmith;
			sdw::Function< sdw::Float
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InFloat > m_smith;
			sdw::Function< sdw::Vec3
				, sdw::InFloat
				, sdw::InVec3 > m_schlickFresnel;
			sdw::Function< sdw::Void
				, InLight
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InFloat
				, sdw::InFloat
				, InSurface
				, OutputComponents & > m_computeCookTorrance;
			sdw::Function< sdw::Vec3
				, InLight
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InFloat
				, InSurface > m_computeCookTorranceDiffuse;
		};
	}
}

#endif
