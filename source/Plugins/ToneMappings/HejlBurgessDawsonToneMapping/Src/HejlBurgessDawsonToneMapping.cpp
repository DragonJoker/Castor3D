#include "HejlBurgessDawsonToneMapping.hpp"

#include <Engine.hpp>
#include <Miscellaneous/Parameter.hpp>
#include <Render/RenderSystem.hpp>
#include <Shader/Ubos/HdrConfigUbo.hpp>

#include <GlslSource.hpp>

using namespace castor;
using namespace castor3d;
using namespace glsl;

namespace HejlBurgessDawson
{
	String ToneMapping::Type = cuT( "hejl" );
	String ToneMapping::Name = cuT( "Hejl Burgess Dawson Tone Mapping" );

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

	glsl::Shader ToneMapping::doCreate()
	{
		glsl::Shader pxl;
		{
			auto writer = getEngine()->getRenderSystem()->createGlslWriter();

			// Shader inputs
			UBO_HDR_CONFIG( writer, 0u, 0u );
			auto c3d_mapDiffuse = writer.declSampler< Sampler2D >( cuT( "c3d_mapDiffuse" ), 1u, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ), 0u );

			// Shader outputs
			auto pxl_rgb = writer.declFragData< Vec4 >( cuT( "pxl_rgb" ), 0 );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				auto hdrColor = writer.declLocale( cuT( "hdrColor" ), texture( c3d_mapDiffuse, vtx_texture ).rgb() );
				hdrColor *= vec3( c3d_exposure );
				auto x = writer.declLocale( cuT( "x" ), max( hdrColor - 0.004_f, 0.0_f ) );
				pxl_rgb = vec4( writer.paren( x * writer.paren( 6.2f * x + 0.5f ) )
					/ writer.paren( x * writer.paren( 6.2f * x + 1.7f ) + 0.06f ), 1.0 );
			} );

			pxl = writer.finalise();
		}

		return pxl;
	}

	void ToneMapping::doDestroy()
	{
	}

	void ToneMapping::doUpdate()
	{
	}
}
