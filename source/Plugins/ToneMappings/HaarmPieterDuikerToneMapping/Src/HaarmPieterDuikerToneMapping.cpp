#include "HaarmPieterDuikerToneMapping.hpp"

#include <Engine.hpp>
#include <Cache/ShaderCache.hpp>

#include <Miscellaneous/Parameter.hpp>
#include <Render/Context.hpp>
#include <Render/RenderSystem.hpp>
#include <Shader/HdrConfigUbo.hpp>
#include <Shader/UniformBuffer.hpp>
#include <Shader/ShaderProgram.hpp>
#include <Texture/TextureLayout.hpp>

#include <GlslSource.hpp>

using namespace Castor;
using namespace Castor3D;
using namespace GLSL;

namespace HaarmPieterDuiker
{
	String ToneMapping::Name = cuT( "haarm" );

	ToneMapping::ToneMapping( Engine & p_engine, Parameters const & p_parameters )
		: Castor3D::ToneMapping{ Name, p_engine, p_parameters }
	{
		String param;

		if ( p_parameters.Get( cuT( "Gamma" ), param ) )
		{
			m_config.SetGamma( string::to_float( param ) );
		}
	}

	ToneMapping::~ToneMapping()
	{
	}

	ToneMappingSPtr ToneMapping::Create( Engine & p_engine, Parameters const & p_parameters )
	{
		return std::make_shared< ToneMapping >( p_engine, p_parameters );
	}

	GLSL::Shader ToneMapping::DoCreate()
	{
		m_gammaVar = m_configUbo.CreateUniform< UniformType::eFloat >( ShaderProgram::Gamma );

		GLSL::Shader pxl;
		{
			auto writer = GetEngine()->GetRenderSystem()->CreateGlslWriter();

			// Shader inputs
			Ubo config{ writer, ShaderProgram::BufferHdrConfig, HdrConfigUbo::BindingPoint };
			auto c3d_fExposure = config.DeclMember< Float >( ShaderProgram::Exposure );
			auto c3d_fGamma = config.DeclMember< Float >( ShaderProgram::Gamma );
			config.End();
			auto c3d_mapDiffuse = writer.DeclUniform< Sampler2D >( ShaderProgram::MapDiffuse );
			auto vtx_texture = writer.DeclInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto plx_v4FragColor = writer.DeclFragData< Vec4 >( cuT( "plx_v4FragColor" ), 0 );

			auto log10 = writer.ImplementFunction< Vec3 >( cuT( "log10" )
				, [&]( Vec3 const & p_in )
				{
					writer.Return( GLSL::log2( p_in ) / GLSL::log2( 10.0_f ) );
				}, InVec3{ &writer, cuT( "p_in" ) } );

			writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				auto hdrColor = writer.DeclLocale( cuT( "hdrColor" ), texture( c3d_mapDiffuse, vtx_texture ).rgb() );
				hdrColor *= c3d_fExposure;
				auto ld = writer.DeclLocale( cuT( "ld" ), vec3( 0.002_f ) );
				auto linReference = writer.DeclLocale( cuT( "linReference" ), 0.18_f );
				auto logReference = writer.DeclLocale( cuT( "logReference" ), 444.0_f );
				auto logGamma = writer.DeclLocale( cuT( "logGamma" ), 1.0_f / c3d_fGamma );

				auto logColor = writer.DeclLocale( cuT( "LogColor" )
					, writer.Paren( log10( vec3( 0.4_f ) * hdrColor.rgb() / linReference )
						/ ld * logGamma + 444.0_f ) / 1023.0f );
				logColor = clamp( logColor, 0.0, 1.0 );

				auto filmLutWidth = writer.DeclLocale( cuT( "FilmLutWidth" ), Float( 256 ) );
				auto padding = writer.DeclLocale( cuT( "Padding" ), Float( 0.5 ) / filmLutWidth );

				//  apply response lookup and color grading for target display
				plx_v4FragColor.r() = mix( padding, 1.0f - padding, logColor.r() );
				plx_v4FragColor.g() = mix( padding, 1.0f - padding, logColor.g() );
				plx_v4FragColor.b() = mix( padding, 1.0f - padding, logColor.b() );
				plx_v4FragColor.a() = 1.0f;
			} );

			pxl = writer.Finalise();
		}

		return pxl;
	}

	void ToneMapping::DoDestroy()
	{
		m_gammaVar.reset();
	}

	void ToneMapping::DoUpdate()
	{
		m_gammaVar->SetValue( m_config.GetGamma() );
	}

	bool ToneMapping::DoWriteInto( TextFile & p_file )
	{
		return p_file.WriteText( cuT( " -Gamma=" ) + string::to_string( m_config.GetGamma() ) ) > 0;
	}
}
