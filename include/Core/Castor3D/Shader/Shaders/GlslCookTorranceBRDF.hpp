/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslCookTorranceBRDF_H___
#define ___C3D_GlslCookTorranceBRDF_H___

#include "Castor3D/Shader/Shaders/GlslLighting.hpp"

namespace castor3d::shader
{
	class CookTorranceBRDF
	{
	public:
		C3D_API explicit CookTorranceBRDF( sdw::ShaderWriter & writer
			, Utils & utils );
		C3D_API sdw::Vec3 compute( Light const & light
			, sdw::Vec3 const & worldEye
			, sdw::Vec3 const & direction
			, sdw::Vec3 const & specular
			, sdw::Float const & metalness
			, sdw::Float const & roughness
			, Surface surface
			, OutputComponents & output );
		C3D_API void computeAON( Light const & light
			, sdw::Vec3 const & worldEye
			, sdw::Vec3 const & direction
			, sdw::Vec3 const & specular
			, sdw::Float const & metalness
			, sdw::Float const & roughness
			, sdw::Float const & smoothBand
			, Surface surface
			, OutputComponents & output );
		C3D_API sdw::Vec3 computeDiffuse( sdw::Vec3 const & colour
			, sdw::Vec3 const & worldEye
			, sdw::Vec3 const & direction
			, sdw::Vec3 const & specular
			, sdw::Float const & metalness
			, Surface surface );
		C3D_API sdw::Vec3 computeDiffuse( Light const & light
			, sdw::Vec3 const & worldEye
			, sdw::Vec3 const & direction
			, sdw::Vec3 const & specular
			, sdw::Float const & metalness
			, Surface surface );
		C3D_API sdw::Vec3 computeDiffuseAON( sdw::Vec3 const & colour
			, sdw::Vec3 const & worldEye
			, sdw::Vec3 const & direction
			, sdw::Vec3 const & specular
			, sdw::Float const & metalness
			, sdw::Float const & smoothBand
			, Surface surface );
		C3D_API sdw::Vec3 computeDiffuseAON( Light const & light
			, sdw::Vec3 const & worldEye
			, sdw::Vec3 const & direction
			, sdw::Vec3 const & specular
			, sdw::Float const & metalness
			, sdw::Float const & smoothBand
			, Surface surface );

	private:
		void declareDistribution();
		void declareGeometry();
		void declareComputeCookTorrance();
		void declareComputeCookTorranceAON();
		void declareComputeCookTorranceDiffuse();
		void declareComputeCookTorranceDiffuseAON();

	private:
		sdw::ShaderWriter & m_writer;
		Utils & m_utils;
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
			, InLight
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InFloat
			, InSurface
			, OutputComponents & > m_computeCookTorrance;
		sdw::Function< sdw::Void
			, InLight
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat
			, InSurface
			, OutputComponents & > m_computeCookTorranceAON;
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InFloat
			, InSurface > m_computeCookTorranceDiffuse;
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InFloat
			, InSurface > m_computeCookTorranceDiffuseAON;
	};
}

#endif
