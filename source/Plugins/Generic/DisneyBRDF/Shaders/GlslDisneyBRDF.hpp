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
	namespace c3d = castor3d::shader;

	class DisneyDiffuseBRDF
		: public c3d::DiffuseBRDF
	{
	public:
		DisneyDiffuseBRDF( sdw::ShaderWriter & writer
			, c3d::BRDFHelpers & brdfHelpers );

		static c3d::DiffuseBRDFUPtr create( sdw::ShaderWriter & writer
			, c3d::BRDFHelpers & brdfHelpers );
		static castor::StringView constexpr Name{ cuT( "disney" ) };

	private:
		void doGenerate( c3d::BlendComponents const & components
			, c3d::LightSurface const & lightSurface )override;
	};

	class DisneySpecularBRDF
		: public c3d::SpecularBRDF
	{
	public:
		DisneySpecularBRDF( sdw::ShaderWriter & writer
			, c3d::BRDFHelpers & brdfHelpers );

		static c3d::SpecularBRDFUPtr create( sdw::ShaderWriter & writer
			, c3d::BRDFHelpers & brdfHelpers );
		static castor::StringView constexpr Name{ cuT( "disney" ) };

	private:
		void doGenerate( c3d::BlendComponents const & components
			, c3d::LightSurface const & lightSurface )override;

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
		: public c3d::ClearcoatBRDF
	{
	public:
		DisneyClearcoatBRDF( sdw::ShaderWriter & writer
			, c3d::BRDFHelpers & brdfHelpers );

		static c3d::ClearcoatBRDFUPtr create( sdw::ShaderWriter & writer
			, c3d::BRDFHelpers & brdfHelpers );
		static castor::StringView constexpr Name{ cuT( "disney" ) };

	private:
		void doGenerate( c3d::BlendComponents const & components
			, c3d::LightSurface const & lightSurface )override;

		sdw::Function< sdw::Float
			, sdw::InFloat
			, sdw::InFloat > m_distribution;
		sdw::Function< sdw::Float
			, sdw::InFloat
			, sdw::InFloat > m_smithGGX;
	};

	class DisneySheenBRDF
		: public c3d::SheenBRDF
	{
	public:
		DisneySheenBRDF( sdw::ShaderWriter & writer
			, c3d::BRDFHelpers & brdfHelpers );

		static c3d::SheenBRDFUPtr create( sdw::ShaderWriter & writer
			, c3d::BRDFHelpers & brdfHelpers );
		static castor::StringView constexpr Name{ cuT( "disney" ) };

	private:
		void doGenerate( c3d::Utils & utils
			, c3d::BlendComponents const & components
			, c3d::LightSurface const & lightSurface )override;
	};
}

#endif
