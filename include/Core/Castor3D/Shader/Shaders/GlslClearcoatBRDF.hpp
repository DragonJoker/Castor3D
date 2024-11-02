/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslClearcoatBRDF_H___
#define ___C3D_GlslClearcoatBRDF_H___

#include "Castor3D/Shader/Shaders/GlslLighting.hpp"

namespace castor3d::shader
{
	class ClearcoatBRDF
	{
	public:
		C3D_API explicit ClearcoatBRDF( sdw::ShaderWriter & writer
			, BRDFHelpers & brdfHelpers );
		C3D_API virtual ~ClearcoatBRDF() = default;

		C3D_API sdw::RetVec3 compute( BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::Vec3 const & radiance
			, sdw::Float const & intensity
			, sdw::Float const & NdotL
			, sdw::Float const & NdotH );

		C3D_API static ClearcoatBRDFUPtr create( sdw::ShaderWriter & writer
			, BRDFHelpers & brdfHelpers );
		C3D_API static castor::StringView constexpr Name{ cuT( "default" ) };

	private:
		virtual void doGenerate( BlendComponents const & components
			, LightSurface const & lightSurface );

	protected:
		sdw::ShaderWriter & m_writer;
		BRDFHelpers & m_brdfHelpers;
		sdw::Function< sdw::Vec3
			, InBlendComponents
			, InLightSurface
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat > m_compute;
	};
}

#endif
