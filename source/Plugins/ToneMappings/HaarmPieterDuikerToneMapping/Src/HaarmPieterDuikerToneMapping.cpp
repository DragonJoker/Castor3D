#include "HaarmPieterDuikerToneMapping.hpp"

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

using namespace castor;
using namespace castor3d;
using namespace glsl;

namespace HaarmPieterDuiker
{
	String ToneMapping::Name = cuT( "haarm" );

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
			auto pxl_fragColor = writer.declFragData< Vec4 >( cuT( "pxl_fragColor" ), 0 );

			auto log10 = writer.implementFunction< Vec3 >( cuT( "log10" )
				, [&]( Vec3 const & p_in )
				{
					writer.returnStmt( glsl::log2( p_in ) / glsl::log2( 10.0_f ) );
				}, InVec3{ &writer, cuT( "p_in" ) } );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				auto hdrColor = writer.declLocale( cuT( "hdrColor" ), texture( c3d_mapDiffuse, vtx_texture ).rgb() );
				hdrColor *= c3d_fExposure;
				auto ld = writer.declLocale( cuT( "ld" ), vec3( 0.002_f ) );
				auto linReference = writer.declLocale( cuT( "linReference" ), 0.18_f );
				auto logReference = writer.declLocale( cuT( "logReference" ), 444.0_f );
				auto logGamma = writer.declLocale( cuT( "logGamma" ), 1.0_f / c3d_fGamma );

				auto logColor = writer.declLocale( cuT( "LogColor" )
					, writer.paren( log10( vec3( 0.4_f ) * hdrColor.rgb() / linReference )
						/ ld * logGamma + 444.0_f ) / 1023.0f );
				logColor = clamp( logColor, 0.0, 1.0 );

				auto filmLutWidth = writer.declLocale( cuT( "FilmLutWidth" ), Float( 256 ) );
				auto padding = writer.declLocale( cuT( "Padding" ), Float( 0.5 ) / filmLutWidth );

				//  apply response lookup and color grading for target display
				pxl_fragColor.r() = mix( padding, 1.0f - padding, logColor.r() );
				pxl_fragColor.g() = mix( padding, 1.0f - padding, logColor.g() );
				pxl_fragColor.b() = mix( padding, 1.0f - padding, logColor.b() );
				pxl_fragColor.a() = 1.0f;
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
