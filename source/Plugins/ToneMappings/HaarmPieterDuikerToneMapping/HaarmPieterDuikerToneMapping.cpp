#include "HaarmPieterDuikerToneMapping/HaarmPieterDuikerToneMapping.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Shader/Ubos/HdrConfigUbo.hpp>

#include <ShaderWriter/Source.hpp>

using namespace castor;
using namespace castor3d;
using namespace sdw;

namespace HaarmPieterDuiker
{
	String ToneMapping::Type = cuT( "haarm" );
	String ToneMapping::Name = cuT( "Haarm Pieter Duiker Tone Mapping" );

	ToneMapping::ToneMapping( Engine & engine
		, castor3d::RenderDevice const & device
		, HdrConfigUbo & hdrConfigUbo
		, Parameters const & parameters )
		: castor3d::ToneMapping{ Type, Name, engine, device, hdrConfigUbo, parameters }
	{
	}

	ToneMapping::~ToneMapping()
	{
	}

	ToneMappingSPtr ToneMapping::create( Engine & engine
		, castor3d::RenderDevice const & device
		, castor3d::HdrConfigUbo & hdrConfigUbo
		, Parameters const & parameters )
	{
		return std::make_shared< ToneMapping >( engine
			, device
			, hdrConfigUbo
			, parameters );
	}

	castor3d::ShaderPtr ToneMapping::doCreate()
	{
		FragmentWriter writer;

		// Shader inputs
		UBO_HDR_CONFIG( writer, 0u, 0u );
		auto c3d_mapHdr = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapHdr", 1u, 0u );
		auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

		// Shader outputs
		auto pxl_rgb = writer.declOutput< Vec4 >( "pxl_rgb", 0 );

		auto log10 = writer.implementFunction< Vec3 >( "log10"
			, [&]( Vec3 const & v )
			{
				writer.returnStmt( log2( v ) / log2( 10.0_f ) );
			}
			, InVec3{ writer, "v" } );

		writer.implementFunction< sdw::Void >( "main"
			, [&]()
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
				pxl_rgb.r() = mix( padding, 1.0f - padding, logColor.r() );
				pxl_rgb.g() = mix( padding, 1.0f - padding, logColor.g() );
				pxl_rgb.b() = mix( padding, 1.0f - padding, logColor.b() );
				pxl_rgb.a() = 1.0f;
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	void ToneMapping::doDestroy()
	{
	}

	void ToneMapping::doCpuUpdate()
	{
	}
}
