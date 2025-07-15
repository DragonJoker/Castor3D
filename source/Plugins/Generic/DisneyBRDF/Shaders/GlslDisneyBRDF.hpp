/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslDisneyBRDF_H___
#define ___C3D_GlslDisneyBRDF_H___

#include <Castor3D/Shader/Shaders/GlslClearcoatBRDF.hpp>
#include <Castor3D/Shader/Shaders/GlslDiffuseBRDF.hpp>
#include <Castor3D/Shader/Shaders/GlslSheenBRDF.hpp>
#include <Castor3D/Shader/Shaders/GlslSpecularBRDF.hpp>

namespace disney::shader
{
	namespace c3ds = c3d::shader;

	class DisneyDiffuseBRDF
		: public c3ds::DiffuseBRDF
	{
	public:
		DisneyDiffuseBRDF( sdw::ShaderWriter & writer
			, c3ds::BRDFHelpers & brdfHelpers );

		static c3ds::DiffuseBRDFPtr create( sdw::ShaderWriter & writer
			, c3ds::BRDFHelpers & brdfHelpers );
		static c3d::StringView constexpr Name{ cuT( "disney" ) };

	private:
		void doGenerate( c3ds::BlendComponents const & components
			, c3ds::LightSurface const & lightSurface )override;
	};

	class DisneySpecularBRDF
		: public c3ds::SpecularBRDF
	{
	public:
		DisneySpecularBRDF( sdw::ShaderWriter & writer
			, c3ds::BRDFHelpers & brdfHelpers );

		static c3ds::SpecularBRDFPtr create( sdw::ShaderWriter & writer
			, c3ds::BRDFHelpers & brdfHelpers );
		static c3d::StringView constexpr Name{ cuT( "disney" ) };

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

	class DisneyClearcoatBRDF
		: public c3ds::ClearcoatBRDF
	{
	public:
		DisneyClearcoatBRDF( sdw::ShaderWriter & writer
			, c3ds::BRDFHelpers & brdfHelpers );

		static c3ds::ClearcoatBRDFPtr create( sdw::ShaderWriter & writer
			, c3ds::BRDFHelpers & brdfHelpers );
		static c3d::StringView constexpr Name{ cuT( "disney" ) };

	private:
		void doGenerate( c3ds::BlendComponents const & components )override;

		sdw::Function< sdw::Float
			, sdw::InFloat
			, sdw::InFloat > m_distribution;
		sdw::Function< sdw::Float
			, sdw::InFloat
			, sdw::InFloat > m_smithGGX;
	};

	class DisneySheenBRDF
		: public c3ds::SheenBRDF
	{
	public:
		DisneySheenBRDF( sdw::ShaderWriter & writer
			, c3ds::BRDFHelpers & brdfHelpers );

		static c3ds::SheenBRDFPtr create( sdw::ShaderWriter & writer
			, c3ds::BRDFHelpers & brdfHelpers );
		static c3d::StringView constexpr Name{ cuT( "disney" ) };

	private:
		void doGenerate( c3ds::Utils & utils
			, c3ds::BlendComponents const & components
			, c3ds::LightSurface const & lightSurface )override;
	};
}

#endif
