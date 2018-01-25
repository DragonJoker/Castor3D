#include "ReinhardToneMapping.hpp"

#include <Engine.hpp>
#include <Cache/ShaderCache.hpp>
#include <Miscellaneous/Parameter.hpp>
#include <Render/Context.hpp>
#include <Render/RenderSystem.hpp>
#include <Shader/Ubos/HdrConfigUbo.hpp>
#include <Shader/UniformBuffer.hpp>
#include <Shader/ShaderProgram.hpp>
#include <Texture/TextureLayout.hpp>

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

using namespace castor;
using namespace glsl;
using namespace castor3d;

namespace Reinhard
{
	String ToneMapping::Name = cuT( "reinhard" );

	ToneMapping::ToneMapping( Engine & engine
		, Parameters const & parameters )
		: castor3d::ToneMapping{ Name, engine, parameters }
	{
	}

	ToneMapping::~ToneMapping()
	{
	}

	ToneMappingSPtr ToneMapping::create( Engine & engine
		, Parameters const & parameters )
	{
		return std::make_shared< ToneMapping >( engine, parameters );
	}

	glsl::Shader ToneMapping::doCreate()
	{
		glsl::Shader pxl;
		{
			auto writer = getEngine()->getRenderSystem()->createGlslWriter();

			// Shader inputs
			UBO_HDR_CONFIG( writer, 0u );
			auto c3d_mapDiffuse = writer.declSampler< Sampler2D >( cuT( "c3d_mapDiffuse" ), MinTextureIndex, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto pxl_rgb = writer.declFragData< Vec4 >( cuT( "pxl_rgb" ), 0 );

			glsl::Utils utils{ writer };
			utils.declareApplyGamma();

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				auto hdrColor = writer.declLocale( cuT( "hdrColor" ), texture( c3d_mapDiffuse, vtx_texture ).rgb() );
				// Exposure tone mapping
				auto mapped = writer.declLocale( cuT( "mapped" ), vec3( Float( 1.0f ) ) - exp( -hdrColor * c3d_exposure ) );
				// Gamma correction
				pxl_rgb = vec4( utils.applyGamma( c3d_gamma, mapped ), 1.0 );
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
