/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslPhongBRDF_H___
#define ___C3D_GlslPhongBRDF_H___

#include "Castor3D/Shader/Shaders/GlslClearcoatBRDF.hpp"
#include "Castor3D/Shader/Shaders/GlslDiffuseBRDF.hpp"
#include "Castor3D/Shader/Shaders/GlslSheenBRDF.hpp"
#include "Castor3D/Shader/Shaders/GlslSpecularBRDF.hpp"

namespace castor3d::shader
{
	namespace c3d = castor3d::shader;

	class PhongDiffuseBRDF
		: public c3d::DiffuseBRDF
	{
	public:
		PhongDiffuseBRDF( sdw::ShaderWriter & writer
			, c3d::BRDFHelpers & brdfHelpers );

		static c3d::DiffuseBRDFPtr create( sdw::ShaderWriter & writer
			, c3d::BRDFHelpers & brdfHelpers );
		static castor::StringView constexpr Name{ cuT( "phong" ) };

	private:
		void doGenerate( c3d::BlendComponents const & components
			, c3d::LightSurface const & lightSurface )override;
	};

	class PhongSpecularBRDF
		: public c3d::SpecularBRDF
	{
	public:
		PhongSpecularBRDF( sdw::ShaderWriter & writer
			, c3d::BRDFHelpers & brdfHelpers );

		static c3d::SpecularBRDFPtr create( sdw::ShaderWriter & writer
			, c3d::BRDFHelpers & brdfHelpers );
		static castor::StringView constexpr Name{ cuT( "phong" ) };

	private:
		void doGenerate( c3d::BlendComponents const & components )override;

		sdw::Function< sdw::Float
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat > m_distribution;
		sdw::Function< sdw::Float
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat > m_smithGGX;
	};

	class PhongClearcoatBRDF
		: public c3d::ClearcoatBRDF
	{
	public:
		PhongClearcoatBRDF( sdw::ShaderWriter & writer
			, c3d::BRDFHelpers & brdfHelpers );

		static c3d::ClearcoatBRDFPtr create( sdw::ShaderWriter & writer
			, c3d::BRDFHelpers & brdfHelpers );
		static castor::StringView constexpr Name{ cuT( "phong" ) };

	private:
		void doGenerate( c3d::BlendComponents const & components )override;

		sdw::Function< sdw::Float
			, sdw::InFloat
			, sdw::InFloat > m_distribution;
		sdw::Function< sdw::Float
			, sdw::InFloat
			, sdw::InFloat > m_smithGGX;
	};
}

#endif
