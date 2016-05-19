#include "ToneMapping.hpp"

#include "Engine.hpp"
#include "ShaderManager.hpp"

#include "Miscellaneous/Parameter.hpp"
#include "Render/Context.hpp"
#include "Render/RenderSystem.hpp"
#include "Shader/FrameVariableBuffer.hpp"
#include "Shader/OneFrameVariable.hpp"

#include <GlslSource.hpp>

using namespace Castor;
using namespace GLSL;

namespace Castor3D
{
	String const ToneMapping::HdrConfig = cuT( "HdrConfig" );
	String const ToneMapping::Exposure = cuT( "c3d_exposure" );

	ToneMapping::ToneMapping( eTONE_MAPPING_TYPE p_type, Engine & p_engine, Parameters const & p_parameters )
		: OwnedBy< Engine >{ p_engine }
		, m_type{ p_type }
		, m_exposure{ 1.0f }
	{
		String l_param;

		if ( p_parameters.Get( cuT( "Exposure" ), l_param ) )
		{
			m_exposure = string::to_float( l_param );
		}
	}

	ToneMapping::~ToneMapping()
	{
	}

	bool ToneMapping::Initialise()
	{
		m_program = GetEngine()->GetShaderManager().GetNewProgram();
		bool l_return = m_program != nullptr;

		if ( l_return )
		{
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

			GetEngine()->GetShaderManager().CreateMatrixBuffer( *m_program, MASK_SHADER_TYPE_VERTEX );
			auto l_configBuffer = GetEngine()->GetRenderSystem()->CreateFrameVariableBuffer( ToneMapping::HdrConfig );
			m_program->AddFrameVariableBuffer( l_configBuffer, MASK_SHADER_TYPE_PIXEL );
			l_configBuffer->CreateVariable( *m_program, eFRAME_VARIABLE_TYPE_FLOAT, ToneMapping::Exposure );
			l_configBuffer->GetVariable( Exposure, m_exposureVar );
			String l_pxl = DoCreate();
			auto l_model = GetEngine()->GetRenderSystem()->GetGpuInformations().GetMaxShaderModel();
			m_program->SetSource( eSHADER_TYPE_VERTEX, l_model, l_vtx );
			m_program->SetSource( eSHADER_TYPE_PIXEL, l_model, l_pxl );
			l_return = m_program->Initialise();
		}

		return l_return;
	}

	void ToneMapping::Cleanup()
	{
		DoDestroy();
		m_exposureVar.reset();

		if ( m_program )
		{
			m_program->Cleanup();
			m_program.reset();
		}
	}

	void ToneMapping::Apply( Size const & p_size, Texture const & p_texture )
	{
		m_exposureVar->SetValue( m_exposure );
		DoUpdate();
		GetEngine()->GetRenderSystem()->GetCurrentContext()->RenderTexture( p_size, p_texture, m_program );
	}
}
