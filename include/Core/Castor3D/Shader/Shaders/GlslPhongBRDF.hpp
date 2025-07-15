/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslPhongBRDF_H___
#define ___C3D_GlslPhongBRDF_H___

#include "Castor3D/Shader/Shaders/GlslClearcoatBRDF.hpp"
#include "Castor3D/Shader/Shaders/GlslDiffuseBRDF.hpp"
#include "Castor3D/Shader/Shaders/GlslSheenBRDF.hpp"
#include "Castor3D/Shader/Shaders/GlslSpecularBRDF.hpp"

namespace c3d::shader
{
	namespace c3ds = c3d::shader;

	class PhongDiffuseBRDF
		: public c3ds::DiffuseBRDF
	{
	public:
		PhongDiffuseBRDF( sdw::ShaderWriter & writer
			, c3ds::BRDFHelpers & brdfHelpers );

		static c3ds::DiffuseBRDFPtr create( sdw::ShaderWriter & writer
			, c3ds::BRDFHelpers & brdfHelpers );
		static StringView constexpr Name{ cuT( "phong" ) };

	private:
		void doGenerate( c3ds::BlendComponents const & components
			, c3ds::LightSurface const & lightSurface )override;
	};

	class PhongSpecularBRDF
		: public c3ds::SpecularBRDF
	{
	public:
		PhongSpecularBRDF( sdw::ShaderWriter & writer
			, c3ds::BRDFHelpers & brdfHelpers );

		void computeDerived( Utils & utils
			, BlendComponents const & components
			, sdw::Float const & HdotV
			, DirectLighting & output )override;

		static c3ds::SpecularBRDFPtr create( sdw::ShaderWriter & writer
			, c3ds::BRDFHelpers & brdfHelpers );
		static StringView constexpr Name{ cuT( "phong" ) };

	private:
		void doGenerate( c3ds::BlendComponents const & components )override;

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
		: public c3ds::ClearcoatBRDF
	{
	public:
		PhongClearcoatBRDF( sdw::ShaderWriter & writer
			, c3ds::BRDFHelpers & brdfHelpers );

		static c3ds::ClearcoatBRDFPtr create( sdw::ShaderWriter & writer
			, c3ds::BRDFHelpers & brdfHelpers );
		static StringView constexpr Name{ cuT( "phong" ) };

	private:
		void doGenerate( c3ds::BlendComponents const & components )override;

		sdw::Function< sdw::Float
			, sdw::InFloat
			, sdw::InFloat > m_distribution;
		sdw::Function< sdw::Float
			, sdw::InFloat
			, sdw::InFloat > m_smithGGX;
	};
}

#endif
