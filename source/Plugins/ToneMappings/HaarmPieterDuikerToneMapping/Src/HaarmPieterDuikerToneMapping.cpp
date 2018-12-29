#include "HaarmPieterDuikerToneMapping.hpp"

#include <Engine.hpp>
#include <Miscellaneous/Parameter.hpp>
#include <Render/RenderSystem.hpp>
#include <Shader/Ubos/HdrConfigUbo.hpp>

#include <ShaderWriter/Source.hpp>

using namespace castor;
using namespace castor3d;
using namespace sdw;

namespace HaarmPieterDuiker
{
	String ToneMapping::Type = cuT( "haarm" );
	String ToneMapping::Name = cuT( "Haarm Pieter Duiker Tone Mapping" );

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
		, castor3d::HdrConfig & hdrConfig
		, Parameters const & parameters )
	{
		return std::make_shared< ToneMapping >( engine
			, hdrConfig
			, parameters );
	}

	castor3d::ShaderPtr ToneMapping::doCreate()
	{
		FragmentWriter writer;

		// Shader inputs
		UBO_HDR_CONFIG( writer, 0u, 0u );
		auto c3d_mapDiffuse = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapDiffuse", 1u, 0u );
		auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

		// Shader outputs
		auto pxl_rgb = writer.declOutput< Vec4 >( "pxl_rgb", 0 );

		auto log10 = writer.implementFunction< Vec3 >( "log10"
			, [&]( Vec3 const & v )
			{
				writer.returnStmt( log2( v ) / log2( 10.0_f ) );
			}
			, InVec3{ writer, "in" } );

		writer.implementFunction< sdw::Void >( "main"
			, [&]()
			{
				auto hdrColor = writer.declLocale( "hdrColor"
					, texture( c3d_mapDiffuse, vtx_texture ).rgb() );
				hdrColor *= c3d_exposure;
				auto ld = writer.declLocale( "ld"
					, vec3( 0.002_f ) );
				auto linReference = writer.declLocale( "linReference"
					, 0.18_f );
				auto logReference = writer.declLocale( "logReference"
					, 444.0_f );
				auto logGamma = writer.declLocale( "logGamma"
					, 1.0_f / c3d_gamma );

				auto logColor = writer.declLocale( "logColor"
					, writer.paren( log10( vec3( 0.4_f ) * hdrColor.rgb() / linReference )
						/ ld * logGamma + 444.0_f ) / 1023.0_f );
				logColor = clamp( logColor, vec3( 0.0_f ), vec3( 1.0_f ) );

				auto filmLutWidth = writer.declLocale( "filmLutWidth", 256.0_f );
				auto padding = writer.declLocale( "padding", 0.5_f / filmLutWidth );

				//  apply response lookup and color grading for target display
				pxl_rgb.r() = mix( padding, 1.0f - padding, logColor.r() );
				pxl_rgb.g() = mix( padding, 1.0f - padding, logColor.g() );
				pxl_rgb.b() = mix( padding, 1.0f - padding, logColor.b() );
				pxl_rgb.a() = 1.0f;
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
