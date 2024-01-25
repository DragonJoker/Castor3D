#include "Uncharted2ToneMapping/Uncharted2ToneMapping.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Shader/Shaders/GlslBaseIO.hpp>
#include <Castor3D/Shader/Ubos/HdrConfigUbo.hpp>
#include <Castor3D/Shader/Ubos/ColourGradingUbo.hpp>

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>

namespace Uncharted2
{
	namespace c3d = castor3d::shader;
	castor::String ToneMapping::Type = cuT( "uncharted2" );
	castor::MbString ToneMapping::Name = "Uncharted 2 Tone Mapping";

	void ToneMapping::create( ast::ShaderBuilder & builder )
	{
		sdw::TraditionalGraphicsWriter writer{ builder };

		C3D_HdrConfig( writer, 0u, 0u );
		C3D_ColourGrading( writer, 1u, 0u );
		auto c3d_mapHdr = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapHdr", 2u, 0u );

		auto ShoulderStrength = writer.declConstant( "ShoulderStrength", 0.15_f );
		auto LinearStrength = writer.declConstant( "LinearStrength", 0.50_f );
		auto LinearAngle = writer.declConstant( "LinearAngle", 0.10_f );
		auto ToeStrength = writer.declConstant( "ToeStrength", 0.20_f );
		auto ToeNumerator = writer.declConstant( "ToeNumerator", 0.02_f );
		auto ToeDenominator = writer.declConstant( "ToeDenominator", 0.30_f );
		auto LinearWhitePointValue = writer.declConstant( "LinearWhitePointValue", 11.2_f );
		auto ExposureBias = writer.declConstant( "ExposureBias", 2.0_f );

		auto uncharted2ToneMap = writer.implementFunction< sdw::Vec3 >( "uncharted2ToneMap"
			, [&]( sdw::Vec3 const & x )
			{
				writer.returnStmt( (
						(
							x
							* ( x * ShoulderStrength + LinearAngle * LinearStrength )
							+ ToeStrength * ToeNumerator )
						/ (
							x
							* ( x * ShoulderStrength + LinearStrength )
							+ ToeStrength * ToeDenominator ) )
					- ToeNumerator / ToeDenominator );
			}
			, sdw::InVec3{ writer, "x" } );

		writer.implementEntryPointT< c3d::Uv2FT, c3d::Colour4FT >( [&]( sdw::FragmentInT< c3d::Uv2FT > in
			, sdw::FragmentOutT< c3d::Colour4FT > out )
			{
				auto hdrColor = writer.declLocale( "hdrColor"
					, c3d_colourGrading.colourGrade( c3d_mapHdr.sample( in.uv() ).rgb() ) );
				hdrColor *= vec3( ExposureBias ); // Hardcoded Exposure Adjustment.

				auto current = writer.declLocale( "current"
					, uncharted2ToneMap( hdrColor * c3d_hdrConfigData.getExposure() ) );

				auto whiteScale = writer.declLocale( "whiteScale"
					, vec3( 1.0_f ) / uncharted2ToneMap( vec3( LinearWhitePointValue ) ) );
				auto colour = writer.declLocale( "colour"
					, current * whiteScale );

				out.colour() = vec4( c3d_hdrConfigData.applyGamma( colour ), 1.0_f );
			} );
	}
}
