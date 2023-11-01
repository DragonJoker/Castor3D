#include "HaarmPieterDuikerToneMapping/HaarmPieterDuikerToneMapping.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Shader/Shaders/GlslBaseIO.hpp>
#include <Castor3D/Shader/Ubos/HdrConfigUbo.hpp>

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>

namespace HaarmPieterDuiker
{
	namespace c3d = castor3d::shader;
	castor::String ToneMapping::Type = cuT( "haarm" );
	castor::String ToneMapping::Name = cuT( "Haarm Pieter Duiker Tone Mapping" );

	castor3d::ShaderPtr ToneMapping::create( castor3d::Engine & engine )
	{
		sdw::TraditionalGraphicsWriter writer{ &engine.getShaderAllocator() };

		C3D_HdrConfig( writer, 0u, 0u );
		auto c3d_mapHdr = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapHdr", 1u, 0u );

		auto log10 = writer.implementFunction< sdw::Vec3 >( "log10"
			, [&]( sdw::Vec3 const & v )
			{
				writer.returnStmt( log2( v ) / log2( 10.0_f ) );
			}
			, sdw::InVec3{ writer, "v" } );

		castor3d::ToneMapping::getVertexProgram( writer );

		writer.implementEntryPointT< c3d::Uv2FT, c3d::Colour4FT >( [&]( sdw::FragmentInT< c3d::Uv2FT > in
			, sdw::FragmentOutT< c3d::Colour4FT > out )
			{
				auto hdrColor = writer.declLocale( "hdrColor"
					, c3d_mapHdr.sample( in.uv() ).rgb() );
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
				out.colour().r() = mix( padding, 1.0f - padding, logColor.r() );
				out.colour().g() = mix( padding, 1.0f - padding, logColor.g() );
				out.colour().b() = mix( padding, 1.0f - padding, logColor.b() );
				out.colour().a() = 1.0f;
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}
}
