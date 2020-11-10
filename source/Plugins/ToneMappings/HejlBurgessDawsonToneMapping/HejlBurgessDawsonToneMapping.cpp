#include "HejlBurgessDawsonToneMapping/HejlBurgessDawsonToneMapping.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Shader/Ubos/HdrConfigUbo.hpp>

#include <ShaderWriter/Source.hpp>

using namespace castor;
using namespace castor3d;
using namespace sdw;

namespace HejlBurgessDawson
{
	String ToneMapping::Type = cuT( "hejl" );
	String ToneMapping::Name = cuT( "Hejl Burgess Dawson Tone Mapping" );

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
		, HdrConfigUbo & hdrConfigUbo
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

		writer.implementFunction< sdw::Void >( "main"
			, [&]()
			{
				auto hdrColor = writer.declLocale( "hdrColor"
					, c3d_mapHdr.sample( vtx_texture ).rgb() );
				hdrColor *= vec3( c3d_exposure );
				auto x = writer.declLocale( "x"
					, max( hdrColor - 0.004_f, vec3( 0.0_f ) ) );
				pxl_rgb = vec4( ( x * ( 6.2f * x + 0.5f ) )
					/ ( x * ( 6.2f * x + 1.7f ) + 0.06f ), 1.0_f );
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
