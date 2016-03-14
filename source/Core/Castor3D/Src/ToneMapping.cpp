#include "ToneMapping.hpp"

#include "Context.hpp"
#include "Engine.hpp"
#include "FrameVariableBuffer.hpp"
#include "RenderSystem.hpp"
#include "ShaderManager.hpp"
#include "Texture.hpp"

#include <GlslSource.hpp>

using namespace Castor;
using namespace GLSL;

namespace Castor3D
{
	static const String HdrConfig = cuT( "HdrConfig" );
	static const String Exposure = cuT( "c3d_exposure" );
	static const String Gamma = cuT( "c3d_gamma" );

	ToneMapping::ToneMapping( Engine & p_engine )
		: OwnedBy< Engine >{ p_engine }
		, m_exposure{ 1.0f }
		, m_gamma{ 1.5f }
	{
	}

	ToneMapping::~ToneMapping()
	{
	}

	bool ToneMapping::Initialise()
	{
		bool l_return = false;

		m_program = GetEngine()->GetShaderManager().GetNewProgram();

		if ( m_program )
		{
			GetEngine()->GetShaderManager().CreateMatrixBuffer( *m_program, MASK_SHADER_TYPE_VERTEX );
			auto l_configBuffer = GetEngine()->GetRenderSystem()->CreateFrameVariableBuffer( HdrConfig );
			l_configBuffer->CreateVariable( *m_program, eFRAME_VARIABLE_TYPE_FLOAT, Exposure );
			l_configBuffer->CreateVariable( *m_program, eFRAME_VARIABLE_TYPE_FLOAT, Gamma );
			m_program->AddFrameVariableBuffer( l_configBuffer, MASK_SHADER_TYPE_PIXEL );
			l_configBuffer->GetVariable( Exposure, m_exposureVar );
			l_configBuffer->GetVariable( Gamma, m_gammaVar );

			String l_vtx;
			{
				auto l_writer = GetEngine()->GetRenderSystem()->CreateGlslWriter();

				UBO_MATRIX( l_writer );

				// Shader inputs
				auto position = l_writer.GetAttribute< Vec2 >( ShaderProgram::Position );
				auto texture = l_writer.GetAttribute< Vec2 >( ShaderProgram::Texture );

				// Shader outputs
				auto vtx_texture = l_writer.GetOutput< Vec2 >( cuT( "vtx_texture" ) );
				auto gl_Position = l_writer.GetBuiltin< Vec4 >( cuT( "gl_Position" ) );

				l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
				{
					vtx_texture = texture;
					gl_Position = c3d_mtxProjection * vec4( position.X, position.Y, 0.0, 1.0 );
				} );

				l_vtx = l_writer.Finalise();
			}

			String l_pxl;
			{
				auto l_writer = GetEngine()->GetRenderSystem()->CreateGlslWriter();

				// Shader inputs
				Ubo l_config = l_writer.GetUbo( HdrConfig );
				auto c3d_exposure = l_config.GetUniform< Float >( Exposure );
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

			auto l_model = GetEngine()->GetRenderSystem()->GetMaxShaderModel();
			m_program->SetSource( eSHADER_TYPE_VERTEX, l_model, l_vtx );
			m_program->SetSource( eSHADER_TYPE_PIXEL, l_model, l_pxl );
			l_return = m_program->Initialise();
		}

		return l_return;
	}

	void ToneMapping::Cleanup()
	{
		m_program->Cleanup();
		m_program.reset();
	}

	void ToneMapping::Apply( Size const & p_size, Texture const & p_texture )
	{
		m_exposureVar->SetValue( m_exposure );
		m_gammaVar->SetValue( m_gamma );
		GetEngine()->GetRenderSystem()->GetCurrentContext()->RenderTexture( p_size, p_texture, m_program );
	}
}
