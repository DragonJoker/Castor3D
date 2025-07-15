/*
See LICENSE file in root folder
*/
#ifndef ___C3DAM_AnisotropicBRDF_H___
#define ___C3DAM_AnisotropicBRDF_H___

#include <Castor3D/Shader/Shaders/GlslSpecularBRDF.hpp>

#include <CastorUtils/FileParser/FileParser.hpp>
#include <CastorUtils/Graphics/RgbColour.hpp>

namespace anisotropy::shader
{
	namespace c3ds = c3d::shader;

	class AnisotropicBRDF
		: public c3ds::SpecularBRDF
	{
	public:
		AnisotropicBRDF( sdw::ShaderWriter & writer
			, c3ds::BRDFHelpers & brdfHelpers );

		static c3ds::SpecularBRDFPtr create( sdw::ShaderWriter & writer
			, c3ds::BRDFHelpers & brdfHelpers );
		static c3d::StringView constexpr Name{ cuT( "anisotropic" ) };

	private:
		void doGenerate( c3ds::BlendComponents const & components )override;

	private:
		sdw::Function< sdw::Float
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat > m_anisotropicGGXDistribution;
		sdw::Function< sdw::Float
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat > m_anisotropicGGXVisibility;
	};
}

#endif
