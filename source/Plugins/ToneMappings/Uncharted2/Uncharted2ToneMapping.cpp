#include "Uncharted2/Uncharted2ToneMapping.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>
#include <Castor3D/Shader/Ubos/HdrConfigUbo.hpp>

#include <ShaderWriter/Source.hpp>

using namespace castor;
using namespace castor3d;

namespace Uncharted2
{
	String ToneMapping::Type = cuT( "uncharted2" );
	String ToneMapping::Name = cuT( "Uncharted 2 Tone Mapping" );

	ToneMapping::ToneMapping( Engine & engine
		, HdrConfig & hdrConfig
		, Parameters const & parameters )
		: castor3d::ToneMapping{ Type, Name, engine, hdrConfig, parameters }
	{
	}

	ToneMapping::~ToneMapping()
	{
	}

	ToneMappingSPtr ToneMapping::create( Engine & engine
		, HdrConfig & hdrConfig
		, Parameters const & parameters )
	{
		return std::make_shared< ToneMapping >( engine
			, hdrConfig
			, parameters );
	}

	castor3d::ShaderPtr ToneMapping::doCreate()
	{
		using namespace sdw;
		FragmentWriter writer;

		// Shader inputs
		UBO_HDR_CONFIG( writer, 0u, 0u );
		auto c3d_mapDiffuse = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapDiffuse", 1u, 0u );
		auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

		// Shader outputs
		auto pxl_rgb = writer.declOutput< Vec4 >( "pxl_rgb", 0 );

		shader::Utils utils{ writer };
		utils.declareApplyGamma();

		auto ShoulderStrength = writer.declConstant( "ShoulderStrength", 0.15_f );
		auto LinearStrength = writer.declConstant( "LinearStrength", 0.50_f );
		auto LinearAngle = writer.declConstant( "LinearAngle", 0.10_f );
		auto ToeStrength = writer.declConstant( "ToeStrength", 0.20_f );
		auto ToeNumerator = writer.declConstant( "ToeNumerator", 0.02_f );
		auto ToeDenominator = writer.declConstant( "ToeDenominator", 0.30_f );
		auto LinearWhitePointValue = writer.declConstant( "LinearWhitePointValue", 11.2_f );
		auto ExposureBias = writer.declConstant( "ExposureBias", 2.0_f );

		auto uncharted2ToneMap = writer.implementFunction< Vec3 >( "uncharted2ToneMap"
			, [&]( Vec3 const & x )
			{
				writer.returnStmt( writer.paren(
						writer.paren(
							x
							* writer.paren( x * ShoulderStrength + LinearAngle * LinearStrength )
							+ ToeStrength * ToeNumerator )
						/ writer.paren(
							x
							* writer.paren( x * ShoulderStrength + LinearStrength )
							+ ToeStrength * ToeDenominator ) )
					- ToeNumerator / ToeDenominator );
			}
			, InVec3{ writer, "x" } );

		writer.implementFunction< sdw::Void >( "main", [&]()
		{
			auto hdrColor = writer.declLocale( "hdrColor"
				, texture( c3d_mapDiffuse, vtx_texture ).rgb() );
			hdrColor *= vec3( Float( ExposureBias ) ); // Hardcoded Exposure Adjustment.

			auto current = writer.declLocale( "current"
				, uncharted2ToneMap( hdrColor * c3d_exposure ) );

			auto whiteScale = writer.declLocale( "whiteScale"
				, vec3( 1.0_f ) / uncharted2ToneMap( vec3( Float( LinearWhitePointValue ) ) ) );
			auto colour = writer.declLocale( "colour"
				, current * whiteScale );

			pxl_rgb = vec4( utils.applyGamma( c3d_gamma, colour ), 1.0_f );
		} );

		return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
	}

	void ToneMapping::doDestroy()
	{
	}

	void ToneMapping::doUpdate()
	{
	}
}
