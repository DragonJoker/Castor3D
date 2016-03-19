#include "ReinhardToneMapping.hpp"

#include "Context.hpp"
#include "Engine.hpp"
#include "FrameVariableBuffer.hpp"
#include "OneFrameVariable.hpp"
#include "Parameter.hpp"
#include "RenderSystem.hpp"
#include "ShaderManager.hpp"
#include "Texture.hpp"

#include <GlslSource.hpp>

using namespace Castor;
using namespace GLSL;

namespace Castor3D
{
	String const Gamma = cuT( "c3d_gamma" );

	ReinhardToneMapping::ReinhardToneMapping( Engine & p_engine, Parameters const & p_parameters )
		: ToneMapping{ eTONE_MAPPING_TYPE_REINHARD, p_engine, p_parameters }
		, m_gamma{ 1.0f }
	{
		String l_param;

		if ( p_parameters.Get( cuT( "Gamma" ), l_param ) )
		{
			m_gamma = string::to_float( l_param );
		}
	}

	ReinhardToneMapping::~ReinhardToneMapping()
	{
	}

	ToneMappingSPtr ReinhardToneMapping::Create( Engine & p_engine, Parameters const & p_parameters )
	{
		return std::make_shared< ReinhardToneMapping >( p_engine, p_parameters );
	}

	String ReinhardToneMapping::DoCreate()
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
				// Exposure tone mapping
				LOCALE_ASSIGN( l_writer, Vec3, l_mapped, vec3( Float( 1.0f ) ) - exp( -l_hdrColor * c3d_exposure ) );
				// Gamma correction 
				l_mapped = pow( l_mapped, vec3( 1.0f / c3d_gamma ) );
				plx_v4FragColor = vec4( l_mapped, 1.0 );
			} );

			l_pxl = l_writer.Finalise();
		}

		return l_pxl;
	}

	void ReinhardToneMapping::DoDestroy()
	{
		m_gammaVar.reset();
	}

	void ReinhardToneMapping::DoUpdate()
	{
		m_gammaVar->SetValue( m_gamma );
	}
}
