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
			, BRDFHelpers & brdf );
		C3D_API sdw::RetVec3 computeSpecular( sdw::Vec3 const & radiance
			, sdw::Float const & intensity
			, sdw::Float const & pNdotL
			, sdw::Float const & pNdotH
			, sdw::Float const & pNdotV
			, sdw::Vec3 const & F
			, sdw::Float const & roughness );
		C3D_API sdw::RetVec3 computeDiffuse( sdw::Vec3 const & radiance
			, sdw::Float const & intensity
			, sdw::Vec3 const & F );

	private:
		sdw::ShaderWriter & m_writer;
		BRDFHelpers & m_brdf;
		castor::RawUniquePtr< sdw::Float > m_diffuseFactor;
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InVec3
			, sdw::InFloat > m_computeCookTorranceSpecular;
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InVec3 > m_computeCookTorranceDiffuse;
	};
}

#endif
