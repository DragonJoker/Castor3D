#include "LinearToneMapping.hpp"

#include <Engine.hpp>
#include <Miscellaneous/Parameter.hpp>
#include <Render/RenderSystem.hpp>
#include <Shader/Ubos/HdrConfigUbo.hpp>

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

using namespace castor;
using namespace glsl;
using namespace castor3d;

namespace Linear
{
	String ToneMapping::Type = cuT( "linear" );
	String ToneMapping::Name = cuT( "Linear Tone Mapping" );

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

			glsl::Utils utils{ writer };
			utils.declareApplyGamma();

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				auto hdrColor = writer.declLocale( cuT( "hdrColor" ), texture( c3d_mapDiffuse, vtx_texture ).rgb() );
				hdrColor *= vec3( c3d_exposure );
				pxl_rgb = vec4( utils.applyGamma( c3d_gamma, hdrColor ), 1.0 );
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
