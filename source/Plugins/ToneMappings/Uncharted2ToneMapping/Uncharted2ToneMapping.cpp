#include "Uncharted2ToneMapping/Uncharted2ToneMapping.hpp"

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

	castor3d::ShaderPtr ToneMapping::create()
	{
		using namespace sdw;
		FragmentWriter writer;

		// Shader inputs
		UBO_HDR_CONFIG( writer, 0u, 0u );
		auto c3d_mapHdr = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapHdr", 1u, 0u );
		auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

		// Shader outputs
		auto pxl_rgb = writer.declOutput< Vec4 >( "pxl_rgb", 0 );

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
		, InVec3{ writer, "x" } );

		writer.implementFunction< sdw::Void >( "main"
			, [&]()
			{
				auto hdrColor = writer.declLocale( "hdrColor"
					, c3d_mapHdr.sample( vtx_texture ).rgb() );
				hdrColor *= vec3( ExposureBias ); // Hardcoded Exposure Adjustment.

				auto current = writer.declLocale( "current"
					, uncharted2ToneMap( hdrColor * c3d_hdrConfigData.getExposure() ) );

				auto whiteScale = writer.declLocale( "whiteScale"
					, vec3( 1.0_f ) / uncharted2ToneMap( vec3( LinearWhitePointValue ) ) );
				auto colour = writer.declLocale( "colour"
					, current * whiteScale );

				pxl_rgb = vec4( c3d_hdrConfigData.applyGamma( colour ), 1.0_f );
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}
}
