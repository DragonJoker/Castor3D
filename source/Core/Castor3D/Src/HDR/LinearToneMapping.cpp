#include "LinearToneMapping.hpp"

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
	namespace
	{
		String const Gamma = cuT( "c3d_gamma" );
	}

	String LinearToneMapping::Name = cuT( "linear" );

	LinearToneMapping::LinearToneMapping( Engine & p_engine, Parameters const & p_parameters )
		: ToneMapping{ Name, p_engine, p_parameters }
		, m_gamma{ 1.0f }
	{
		String l_param;

		if ( p_parameters.Get( cuT( "Gamma" ), l_param ) )
		{
			m_gamma = string::to_float( l_param );
		}
	}

	LinearToneMapping::~LinearToneMapping()
	{
	}

	ToneMappingSPtr LinearToneMapping::Create( Engine & p_engine, Parameters const & p_parameters )
	{
		return std::make_shared< LinearToneMapping >( p_engine, p_parameters );
	}

	String LinearToneMapping::DoCreate()
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
				LOCALE_ASSIGN( l_writer, Vec3, l_hdrColor, texture2D( c3d_mapDiffuse, vtx_texture ).SWIZZLE_RGB );
				l_hdrColor *= vec3( c3d_exposure );
				plx_v4FragColor = vec4( pow( l_hdrColor, vec3( 1.0 / c3d_gamma ) ), 1.0 );
			} );

			l_pxl = l_writer.Finalise();
		}

		return l_pxl;
	}

	void LinearToneMapping::DoDestroy()
	{
		m_gammaVar.reset();
	}

	void LinearToneMapping::DoUpdate()
	{
		m_gammaVar->SetValue( m_gamma );
	}

	bool LinearToneMapping::DoWriteInto( TextFile & p_file )
	{
		return p_file.WriteText( cuT( " -Gamma " ) + string::to_string( m_gamma ) ) > 0;
	}
}
