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

	ToneMapping::ToneMapping( Engine & engine, Parameters const & p_parameters )
		: castor3d::ToneMapping{ Name, engine, p_parameters }
	{
		String param;

		if ( p_parameters.get( cuT( "Gamma" ), param ) )
		{
			m_config.setGamma( string::toFloat( param ) );
		}
	}

	ToneMapping::~ToneMapping()
	{
	}

	ToneMappingSPtr ToneMapping::create( Engine & engine, Parameters const & p_parameters )
	{
		return std::make_shared< ToneMapping >( engine, p_parameters );
	}

	glsl::Shader ToneMapping::doCreate()
	{
		m_gammaVar = m_configUbo.createUniform< UniformType::eFloat >( HdrConfigUbo::Gamma );

		glsl::Shader pxl;
		{
			auto writer = getEngine()->getRenderSystem()->createGlslWriter();

			// Shader inputs
			Ubo config{ writer, HdrConfigUbo::BufferHdrConfig, HdrConfigUbo::BindingPoint };
			auto c3d_fExposure = config.declMember< Float >( HdrConfigUbo::Exposure );
			auto c3d_fGamma = config.declMember< Float >( HdrConfigUbo::Gamma );
			config.end();
			auto c3d_mapDiffuse = writer.declSampler< Sampler2D >( ShaderProgram::MapDiffuse, MinTextureIndex );
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
					, uncharted2ToneMap( hdrColor * c3d_fExposure ) );

				auto whiteScale = writer.declLocale( cuT( "whiteScale" )
					, vec3( 1.0_f ) * uncharted2ToneMap( vec3( Float( W ) ) ) );
				auto colour = writer.declLocale( cuT( "colour" )
					, current * whiteScale );

				pxl_rgb = vec4( utils.applyGamma( c3d_fGamma, colour ), 1.0 );
			} );

			pxl = writer.finalise();
		}

		return pxl;
	}

	void ToneMapping::doDestroy()
	{
		m_gammaVar.reset();
	}

	void ToneMapping::doUpdate()
	{
		m_gammaVar->setValue( m_config.getGamma() );
	}

	bool ToneMapping::doWriteInto( TextFile & p_file )
	{
		return p_file.writeText( cuT( " -Gamma=" ) + string::toString( m_config.getGamma() ) ) > 0;
	}
}
