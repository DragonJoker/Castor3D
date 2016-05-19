#include "HaarmPeterDuikerToneMapping.hpp"

#include "Engine.hpp"
#include "ShaderManager.hpp"

#include "Miscellaneous/Parameter.hpp"
#include "Render/Context.hpp"
#include "Render/RenderSystem.hpp"
#include "Shader/FrameVariableBuffer.hpp"
#include "Shader/OneFrameVariable.hpp"
#include "Texture/TextureLayout.hpp"

#include <GlslSource.hpp>

using namespace Castor;
using namespace GLSL;

namespace Castor3D
{
	static const String Gamma = cuT( "c3d_gamma" );

	HaarmPeterDuikerToneMapping::HaarmPeterDuikerToneMapping( Engine & p_engine, Parameters const & p_parameters )
		: ToneMapping{ eTONE_MAPPING_TYPE_HAARM_PETER_DUIKER, p_engine, p_parameters }
		, m_gamma{ 1.0f }
	{
		String l_param;

		if ( p_parameters.Get( cuT( "Gamma" ), l_param ) )
		{
			m_gamma = string::to_float( l_param );
		}
	}

	HaarmPeterDuikerToneMapping::~HaarmPeterDuikerToneMapping()
	{
	}

	ToneMappingSPtr HaarmPeterDuikerToneMapping::Create( Engine & p_engine, Parameters const & p_parameters )
	{
		return std::make_shared< HaarmPeterDuikerToneMapping >( p_engine, p_parameters );
	}

	String HaarmPeterDuikerToneMapping::DoCreate()
	{
		auto l_configBuffer = m_program->FindFrameVariableBuffer( ToneMapping::HdrConfig );
		REQUIRE( l_configBuffer != nullptr );
		l_configBuffer->CreateVariable( *m_program, eFRAME_VARIABLE_TYPE_FLOAT, Gamma );
		l_configBuffer->GetVariable( Gamma, m_gammaVar );

		String l_pxl;
		{
			auto l_writer = GetEngine()->GetRenderSystem()->CreateGlslWriter();

			// Shader inputs
			Ubo l_config = l_writer.GetUbo( ToneMapping::HdrConfig );
			auto c3d_exposure = l_config.GetUniform< Float >( ToneMapping::Exposure );
			auto c3d_gamma = l_config.GetUniform< Float >( Gamma );
			l_config.End();
			auto c3d_mapDiffuse = l_writer.GetUniform< Sampler2D >( ShaderProgram::MapDiffuse );
			auto vtx_texture = l_writer.GetInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto plx_v4FragColor = l_writer.GetFragData< Vec4 >( cuT( "plx_v4FragColor" ), 0 );

			l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				LOCALE_ASSIGN( l_writer, Vec3, l_hdrColor, texture2D( c3d_mapDiffuse, vtx_texture ).RGB );
				l_hdrColor *= c3d_exposure;

				auto LogColor = l_writer.GetLocale < Vec3 >( cuT( "LogColor" ) );
				LogColor.RGB = l_writer.Paren( l_writer.Paren( GLSL::log2( vec3( Float( 0.4 ) ) * l_hdrColor.RGB / vec3( Float( 0.18 ) ) ) / GLSL::log2( vec3( Float( 10 ) ) ) ) / vec3( Float( 0.002 ) ) * vec3( Float( 1.0 ) / c3d_gamma ) + vec3( Float( 444 ) ) ) / vec3( Float( 1023.0f ) );
				LogColor.RGB = clamp( LogColor.RGB, 0.0, 1.0 );

				LOCALE_ASSIGN( l_writer, Float, FilmLutWidth, Float( 256 ) );
				LOCALE_ASSIGN( l_writer, Float, Padding, Float( 0.5 ) / FilmLutWidth );

				//  apply response lookup and color grading for target display
				plx_v4FragColor.R = mix( Padding, 1.0f - Padding, LogColor.R );
				plx_v4FragColor.G = mix( Padding, 1.0f - Padding, LogColor.G );
				plx_v4FragColor.B = mix( Padding, 1.0f - Padding, LogColor.B );
				plx_v4FragColor.A = 1.0f;
			} );

			l_pxl = l_writer.Finalise();
		}

		return l_pxl;
	}

	void HaarmPeterDuikerToneMapping::DoDestroy()
	{
		m_gammaVar.reset();
	}

	void HaarmPeterDuikerToneMapping::DoUpdate()
	{
		m_gammaVar->SetValue( m_gamma );
	}
}
