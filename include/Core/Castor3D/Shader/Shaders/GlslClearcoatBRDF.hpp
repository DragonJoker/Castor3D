/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslClearcoatBRDF_H___
#define ___C3D_GlslClearcoatBRDF_H___

#include "Castor3D/Shader/Shaders/GlslLighting.hpp"

namespace c3d::shader
{
	class ClearcoatBRDF
	{
	public:
		C3D_API explicit ClearcoatBRDF( sdw::ShaderWriter & writer
			, BRDFHelpers & brdfHelpers );
		C3D_API virtual ~ClearcoatBRDF() = default;

		C3D_API sdw::RetVec3 compute( BlendComponents const & components
			, sdw::Vec3 const & N
			, sdw::Vec3 const & L
			, sdw::Vec3 const & H
			, sdw::Vec3 const & V
			, sdw::Float const & NdotL
			, sdw::Float const & NdotH );

		C3D_API static ClearcoatBRDFPtr create( sdw::ShaderWriter & writer
			, BRDFHelpers & brdfHelpers );
		C3D_API static StringView constexpr Name{ cuT( "default" ) };

	private:
		virtual void doGenerate( BlendComponents const & components );

	protected:
		sdw::ShaderWriter & m_writer;
		BRDFHelpers & m_brdfHelpers;
		sdw::Function< sdw::Vec3
			, InBlendComponents
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InFloat > m_compute;
	};
}

#endif
