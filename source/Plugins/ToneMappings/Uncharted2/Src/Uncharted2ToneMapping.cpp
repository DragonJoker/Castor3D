#include "Uncharted2ToneMapping.hpp"

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

namespace Uncharted2
{
	String ToneMapping::Name = cuT( "uncharted2" );

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
			auto c3d_mapDiffuse = writer.declSampler< Sampler2D >( ShaderProgram::MapDiffuse, MinTextureIndex, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto pxl_rgb = writer.declFragData< Vec4 >( cuT( "pxl_rgb" ), 0 );

			glsl::Utils utils{ writer };
			utils.declareApplyGamma();

			static float constexpr A = 0.15f;
			static float constexpr B = 0.50f;
			static float constexpr C = 0.10f;
			static float constexpr D = 0.20f;
			static float constexpr E = 0.02f;
			static float constexpr F = 0.30f;
			static float constexpr W = 11.2f;

			auto uncharted2ToneMap = writer.implementFunction< Vec3 >( cuT( "uncharted2ToneMap" )
				, [&]( Vec3 const & x )
				{
					writer.returnStmt( writer.paren( writer.paren( x * writer.paren( x * A + C * B ) + D * E )
						/ writer.paren( x * writer.paren( x * A + B ) + D * F ) ) - E / F );
				}
				, InVec3{ &writer, cuT( "x" ) } );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				auto hdrColor = writer.declLocale( cuT( "hdrColor" )
					, texture( c3d_mapDiffuse, vtx_texture ).rgb() );
				hdrColor *= vec3( 16.0_f ); // Hardcoded Exposure Adjustment.

				auto current = writer.declLocale( cuT( "current" )
					, uncharted2ToneMap( hdrColor * c3d_exposure ) );

				auto whiteScale = writer.declLocale( cuT( "whiteScale" )
					, vec3( 1.0_f ) * uncharted2ToneMap( vec3( Float( W ) ) ) );
				auto colour = writer.declLocale( cuT( "colour" )
					, current * whiteScale );

				pxl_rgb = vec4( utils.applyGamma( c3d_gamma, colour ), 1.0 );
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
