/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslLambertianBRDF_H___
#define ___C3D_GlslLambertianBRDF_H___

#include "Castor3D/Shader/Shaders/GlslDiffuseBRDF.hpp"

namespace castor3d::shader
{
	class LambertianBRDF
		: public DiffuseBRDF
	{
	public:
		C3D_API explicit LambertianBRDF( sdw::ShaderWriter & writer
			, BRDFHelpers & brdfHelpers );

		C3D_API sdw::RetVec3 compute( sdw::Vec3 const & radiance
			, sdw::Float const & intensity
			, sdw::Float const & NdotL
			, sdw::Float const & NdotV
			, sdw::Float const & LdotV
			, sdw::Vec3 const & F
			, sdw::Float const & roughness )override;

		C3D_API static DiffuseBRDFUPtr create( sdw::ShaderWriter & writer
			, BRDFHelpers & brdfHelpers );
		C3D_API static castor::StringView constexpr Name{ cuT( "lambertian" ) };
	};
}

#endif
