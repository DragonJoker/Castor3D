#include "HaarmPieterDuikerToneMapping/HaarmPieterDuikerToneMapping.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Shader/Ubos/HdrConfigUbo.hpp>

#include <ShaderWriter/Source.hpp>

namespace HaarmPieterDuiker
{
	castor::String ToneMapping::Type = cuT( "haarm" );
	castor::String ToneMapping::Name = cuT( "Haarm Pieter Duiker Tone Mapping" );

	castor3d::ShaderPtr ToneMapping::create()
	{
		using namespace sdw;
		FragmentWriter writer;

		// Shader inputs
		C3D_HdrConfig( writer, 0u, 0u );
		auto c3d_mapHdr = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapHdr", 1u, 0u );
		auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

		// Shader outputs
		auto outColour = writer.declOutput< Vec4 >( "outColour", 0 );

		auto log10 = writer.implementFunction< Vec3 >( "log10"
			, [&]( Vec3 const & v )
			{
				writer.returnStmt( log2( v ) / log2( 10.0_f ) );
			}
			, InVec3{ writer, "v" } );

		writer.implementMainT< VoidT, VoidT >( [&]( FragmentIn in
			, FragmentOut out )
			{
				auto hdrColor = writer.declLocale( "hdrColor"
					, c3d_mapHdr.sample( vtx_texture ).rgb() );
				hdrColor *= c3d_hdrConfigData.getExposure();
				auto ld = writer.declLocale( "ld"
					, vec3( 0.002_f ) );
				auto linReference = writer.declLocale( "linReference"
					, 0.18_f );
				auto logReference = writer.declLocale( "logReference"
					, 444.0_f );
				auto logGamma = writer.declLocale( "logGamma"
					, 1.0_f / c3d_hdrConfigData.getGamma() );

				auto logColor = writer.declLocale( "logColor"
					, ( log10( vec3( 0.4_f ) * hdrColor.rgb() / linReference )
						/ ld * logGamma + 444.0_f ) / 1023.0_f );
				logColor = clamp( logColor, vec3( 0.0_f ), vec3( 1.0_f ) );

				auto filmLutWidth = writer.declLocale( "filmLutWidth", 256.0_f );
				auto padding = writer.declLocale( "padding", 0.5_f / filmLutWidth );

				//  apply response lookup and color grading for target display
				outColour.r() = mix( padding, 1.0f - padding, logColor.r() );
				outColour.g() = mix( padding, 1.0f - padding, logColor.g() );
				outColour.b() = mix( padding, 1.0f - padding, logColor.b() );
				outColour.a() = 1.0f;
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}
}
