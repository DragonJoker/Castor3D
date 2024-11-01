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

		C3D_API static DiffuseBRDFUPtr create( sdw::ShaderWriter & writer
			, BRDFHelpers & brdfHelpers );
		C3D_API static castor::StringView constexpr Name{ cuT( "lambertian" ) };

	private:
		void doGenerate( BlendComponents const & components
			, LightSurface const & lightSurface )override;
	};
}

#endif
