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
		String l_param;

		if ( p_parameters.Get( cuT( "Gamma" ), l_param ) )
		{
			m_config.SetGamma( string::to_float( l_param ) );
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

		GLSL::Shader l_pxl;
		{
			auto l_writer = GetEngine()->GetRenderSystem()->CreateGlslWriter();

			// Shader inputs
			Ubo l_config{ l_writer, ShaderProgram::BufferHdrConfig, HdrConfigUbo::BindingPoint };
			auto c3d_fExposure = l_config.DeclMember< Float >( ShaderProgram::Exposure );
			auto c3d_fGamma = l_config.DeclMember< Float >( ShaderProgram::Gamma );
			l_config.End();
			auto c3d_mapDiffuse = l_writer.DeclUniform< Sampler2D >( ShaderProgram::MapDiffuse );
			auto vtx_texture = l_writer.DeclInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto plx_v4FragColor = l_writer.DeclFragData< Vec4 >( cuT( "plx_v4FragColor" ), 0 );

			auto log10 = l_writer.ImplementFunction< Vec3 >( cuT( "log10" )
				, [&]( Vec3 const & p_in )
				{
					l_writer.Return( GLSL::log2( p_in ) / GLSL::log2( 10.0_f ) );
				}, InVec3{ &l_writer, cuT( "p_in" ) } );

			l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				auto l_hdrColor = l_writer.DeclLocale( cuT( "l_hdrColor" ), texture( c3d_mapDiffuse, vtx_texture ).rgb() );
				l_hdrColor *= c3d_fExposure;
				auto ld = l_writer.DeclLocale( cuT( "ld" ), vec3( 0.002_f ) );
				auto linReference = l_writer.DeclLocale( cuT( "linReference" ), 0.18_f );
				auto logReference = l_writer.DeclLocale( cuT( "logReference" ), 444.0_f );
				auto logGamma = l_writer.DeclLocale( cuT( "logGamma" ), 1.0_f / c3d_fGamma );

				auto l_logColor = l_writer.DeclLocale( cuT( "LogColor" )
					, l_writer.Paren( log10( vec3( 0.4_f ) * l_hdrColor.rgb() / linReference )
						/ ld * logGamma + 444.0_f ) / 1023.0f );
				l_logColor = clamp( l_logColor, 0.0, 1.0 );

				auto l_filmLutWidth = l_writer.DeclLocale( cuT( "FilmLutWidth" ), Float( 256 ) );
				auto l_padding = l_writer.DeclLocale( cuT( "Padding" ), Float( 0.5 ) / l_filmLutWidth );

				//  apply response lookup and color grading for target display
				plx_v4FragColor.r() = mix( l_padding, 1.0f - l_padding, l_logColor.r() );
				plx_v4FragColor.g() = mix( l_padding, 1.0f - l_padding, l_logColor.g() );
				plx_v4FragColor.b() = mix( l_padding, 1.0f - l_padding, l_logColor.b() );
				plx_v4FragColor.a() = 1.0f;
			} );

			l_pxl = l_writer.Finalise();
		}

		return l_pxl;
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
