/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslCookTorranceBRDF_H___
#define ___C3D_GlslCookTorranceBRDF_H___

#include "Castor3D/Shader/Shaders/GlslSpecularBRDF.hpp"

namespace castor3d::shader
{
	class CookTorranceBRDF
		: public SpecularBRDF
	{
	public:
		C3D_API CookTorranceBRDF( sdw::ShaderWriter & writer
			, BRDFHelpers & brdfHelpers );
		C3D_API sdw::RetVec3 compute( sdw::Vec3 const & radiance
			, sdw::Float const & intensity
			, sdw::Float const & NdotL
			, sdw::Float const & NdotH
			, sdw::Float const & NdotV
			, sdw::Vec3 const & F
			, sdw::Float const & roughness )override;

		C3D_API static SpecularBRDFUPtr create( sdw::ShaderWriter & writer
			, BRDFHelpers & brdfHelpers );
		C3D_API static castor::StringView constexpr Name{ cuT( "cook_torrance" ) };

	private:
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InVec3
			, sdw::InFloat > m_compute;
	};
}

#endif
