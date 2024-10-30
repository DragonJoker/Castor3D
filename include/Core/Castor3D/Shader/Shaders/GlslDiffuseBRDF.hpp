/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslDiffuseBRDF_H___
#define ___C3D_GlslDiffuseBRDF_H___

#include "Castor3D/Shader/Shaders/GlslLighting.hpp"

namespace castor3d::shader
{
	class DiffuseBRDF
	{
	public:
		C3D_API explicit DiffuseBRDF( sdw::ShaderWriter & writer );
		C3D_API virtual ~DiffuseBRDF() = default;

		C3D_API virtual sdw::RetVec3 compute( sdw::Vec3 const & radiance
			, sdw::Float const & intensity
			, sdw::Float const & NdotL
			, sdw::Float const & NdotV
			, sdw::Float const & LdotV
			, sdw::Vec3 const & F
			, sdw::Float const & roughness ) = 0;

	protected:
		sdw::ShaderWriter & m_writer;
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
