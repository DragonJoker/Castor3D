#include "Uncharted2ToneMapping.hpp"

#include <Engine.hpp>
#include <Miscellaneous/Parameter.hpp>
#include <Render/RenderSystem.hpp>
#include <Shader/Ubos/HdrConfigUbo.hpp>

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
			UBO_HDR_CONFIG( writer, 0u, 0u );
			auto c3d_mapDiffuse = writer.declSampler< Sampler2D >( cuT( "c3d_mapDiffuse" ), 1u, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ), 0u );

			// Shader outputs
			auto pxl_rgb = writer.declFragData< Vec4 >( cuT( "pxl_rgb" ), 0 );

			glsl::Utils utils{ writer };
			utils.declareApplyGamma();

			static float constexpr ShoulderStrength = 0.15f;
			static float constexpr LinearStrength = 0.50f;
			static float constexpr LinearAngle = 0.10f;
			static float constexpr ToeStrength = 0.20f;
			static float constexpr ToeNumerator = 0.02f;
			static float constexpr ToeDenominator = 0.30f;
			static float constexpr LinearWhitePointValue = 11.2f;
			static float constexpr ExposureBias = 2.0f;

			auto uncharted2ToneMap = writer.implementFunction< Vec3 >( cuT( "uncharted2ToneMap" )
				, [&]( Vec3 const & x )
				{
					writer.returnStmt( writer.paren(
							writer.paren(
								x
								* writer.paren( x * ShoulderStrength + LinearAngle * LinearStrength )
								+ ToeStrength * ToeNumerator )
							/ writer.paren(
								x
								* writer.paren( x * ShoulderStrength + LinearStrength )
								+ ToeStrength * ToeDenominator ) )
						- ToeNumerator / ToeDenominator );
				}
				, InVec3{ &writer, cuT( "x" ) } );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				auto hdrColor = writer.declLocale( cuT( "hdrColor" )
					, texture( c3d_mapDiffuse, vtx_texture ).rgb() );
				hdrColor *= vec3( Float( ExposureBias ) ); // Hardcoded Exposure Adjustment.

				auto current = writer.declLocale( cuT( "current" )
					, uncharted2ToneMap( hdrColor * c3d_exposure ) );

				auto whiteScale = writer.declLocale( cuT( "whiteScale" )
					, vec3( 1.0_f ) / uncharted2ToneMap( vec3( Float( LinearWhitePointValue ) ) ) );
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
