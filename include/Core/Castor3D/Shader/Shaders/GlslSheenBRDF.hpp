/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslSheenBRDF_H___
#define ___C3D_GlslSheenBRDF_H___

#include "Castor3D/Shader/Shaders/GlslLighting.hpp"

namespace castor3d::shader
{
	class SheenBRDF
	{
	public:
		C3D_API explicit SheenBRDF( sdw::ShaderWriter & writer
			, BRDFHelpers & brdf );
		C3D_API sdw::RetVec2 compute( LightSurface const & lightSurface
			, sdw::Float const & NdotL
			, sdw::Float const & NdotH
			, sdw::Float const & roughness );

	private:
		sdw::ShaderWriter & m_writer;
		BRDFHelpers & m_brdf;
		sdw::Function< sdw::Vec2
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat > m_computeSheen;
	};
}

#endif
