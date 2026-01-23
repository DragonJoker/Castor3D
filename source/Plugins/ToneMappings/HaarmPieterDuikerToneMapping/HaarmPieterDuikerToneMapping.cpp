#include "HaarmPieterDuikerToneMapping/HaarmPieterDuikerToneMapping.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Shader/Shaders/GlslBaseIO.hpp>
#include <Castor3D/Shader/Ubos/RenderUbo.hpp>
#include <Castor3D/Shader/Ubos/ColourGradingUbo.hpp>

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>

namespace HaarmPieterDuiker
{
	namespace c3ds = c3d::shader;
	c3d::String ToneMapping::Type = cuT( "haarm" );
	c3d::MbString ToneMapping::Name = "Haarm Pieter Duiker Tone Mapping";

	ToneMapping::~ToneMapping()noexcept = default;

	ToneMapping::ToneMapping( c3d::ToneMapping const & parent
		, c3d::RenderDevice const &
		, c3d::Parameters )
		: c3d::ToneMappingImpl{ parent }
	{
	}

	void ToneMapping::getFragmentProgram( ast::ShaderBuilder & builder )
	{
		sdw::TraditionalGraphicsWriter writer{ builder };

		C3D_Render( writer, 0u, 0u );
		C3D_ColourGrading( writer, 1u, 0u );
		auto c3d_mapHdr = writer.declCombinedImg< FImg2DRgba16 >( "c3d_mapHdr", 2u, 0u );

		auto log10 = writer.implementFunction< sdw::Vec3 >( "log10"
			, [&writer]( sdw::Vec3 const & v )
			{
				writer.returnStmt( log2( v ) / log2( 10.0_f ) );
			}
			, sdw::InVec3{ writer, "v" } );

		writer.implementEntryPointT< c3ds::Uv2FT, c3ds::Colour4FT >( [&writer, &c3d_colourGrading, &c3d_renderData, &c3d_mapHdr, log10]( sdw::FragmentInT< c3ds::Uv2FT > const & in
			, sdw::FragmentOutT< c3ds::Colour4FT > const & out )
			{
				auto hdrColor = writer.declLocale( "hdrColor"
					, c3d_colourGrading.colourGrade( c3d_mapHdr.sample( in.uv() ).rgb() ) );
				hdrColor *= c3d_renderData.exposure();
				auto ld = writer.declLocale( "ld"
					, vec3( 0.002_f ) );
				auto linReference = writer.declLocale( "linReference"
					, 0.18_f );
				auto logReference = writer.declLocale( "logReference"
					, 444.0_f );
				auto logGamma = writer.declLocale( "logGamma"
					, 1.0_f / c3d_renderData.gamma() );

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
	}

	void ToneMapping::accept( c3d::ToneMappingVisitor & visitor )
	{
	}

	c3d::ToneMappingImplUPtr ToneMapping::create( c3d::ToneMapping const & parent, c3d::RenderDevice const & device, c3d::Parameters parameters )
	{
		return c3d::ToneMappingImplUPtr( c3d::makeRawUnique< ToneMapping >( parent, device, c3d::move( parameters ) ).release() );
	}
}
