/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslSheenBRDF_H___
#define ___C3D_GlslSheenBRDF_H___

#include "Castor3D/Shader/Shaders/GlslLighting.hpp"

namespace c3d::shader
{
	class SheenBRDF
	{
	public:
		C3D_API explicit SheenBRDF( sdw::ShaderWriter & writer
			, BRDFHelpers & brdfHelpers );
		C3D_API virtual ~SheenBRDF() = default;

		C3D_API sdw::RetVec4 compute( Utils & utils
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::Float const & NdotL
			, sdw::Float const & NdotH );

		C3D_API static SheenBRDFPtr create( sdw::ShaderWriter & writer
			, BRDFHelpers & brdfHelpers );
		C3D_API static StringView constexpr Name{ cuT( "default" ) };

	private:
		virtual void doGenerate( Utils & utils
			, BlendComponents const & components
			, LightSurface const & lightSurface );

	protected:
		sdw::ShaderWriter & m_writer;
		BRDFHelpers & m_brdfHelpers;
		sdw::Function< sdw::Vec4
			, InBlendComponents
			, InLightSurface
			, sdw::InFloat
			, sdw::InFloat > m_compute;
	};
}

#endif
