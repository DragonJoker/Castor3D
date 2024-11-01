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

		C3D_API static SpecularBRDFUPtr create( sdw::ShaderWriter & writer
			, BRDFHelpers & brdfHelpers );
		C3D_API static castor::StringView constexpr Name{ cuT( "cook_torrance" ) };

	private:
		void doGenerate( BlendComponents const & components
			, LightSurface const & lightSurface )override;
	};
}

#endif
