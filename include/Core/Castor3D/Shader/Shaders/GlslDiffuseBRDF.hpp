/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslDiffuseBRDF_H___
#define ___C3D_GlslDiffuseBRDF_H___

#include "Castor3D/Shader/Shaders/GlslLighting.hpp"

namespace c3d::shader
{
	class DiffuseBRDF
	{
	public:
		C3D_API explicit DiffuseBRDF( sdw::ShaderWriter & writer
			, BRDFHelpers & brdfHelpers );
		C3D_API virtual ~DiffuseBRDF() = default;

		C3D_API sdw::RetVec3 compute( BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::Vec3 const & radiance
			, sdw::Float const & intensity
			, sdw::Float const & NdotL );

	private:
		virtual void doGenerate( BlendComponents const & components
			, LightSurface const & lightSurface ) = 0;

	protected:
		sdw::ShaderWriter & m_writer;
		BRDFHelpers & m_brdfHelpers;
		sdw::Function< sdw::Vec3
			, InBlendComponents
			, InLightSurface
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InFloat > m_compute;
	};
}

#endif
