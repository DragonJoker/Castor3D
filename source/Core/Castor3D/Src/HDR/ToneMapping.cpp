#include "ToneMapping.hpp"

#include "Engine.hpp"
#include "ShaderCache.hpp"

#include "Miscellaneous/Parameter.hpp"
#include "Render/Context.hpp"
#include "Render/Pipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Shader/FrameVariableBuffer.hpp"
#include "Shader/OneFrameVariable.hpp"
#include "Shader/ShaderProgram.hpp"

#include <GlslSource.hpp>

using namespace Castor;
using namespace GLSL;

namespace Castor3D
{
	String const ToneMapping::HdrConfig = cuT( "HdrConfig" );
	String const ToneMapping::Exposure = cuT( "c3d_exposure" );

	ToneMapping::ToneMapping( Castor::String const & p_name, Engine & p_engine, Parameters const & p_parameters )
		: OwnedBy< Engine >{ p_engine }
		, Named{ p_name }
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
		auto l_program = GetEngine()->GetShaderProgramCache().GetNewProgram();
		bool l_return = l_program != nullptr;

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
					gl_Position = c3d_mtxProjection * vec4( position.x(), position.y(), 0.0, 1.0 );
				} );

				l_vtx = l_writer.Finalise();
			}

			GetEngine()->GetShaderProgramCache().CreateMatrixBuffer( *l_program, 0u, MASK_SHADER_TYPE_VERTEX );
			auto & l_configBuffer = l_program->CreateFrameVariableBuffer( ToneMapping::HdrConfig, MASK_SHADER_TYPE_PIXEL );
			l_configBuffer.CreateVariable( FrameVariableType::Float, ToneMapping::Exposure );
			l_configBuffer.GetVariable( Exposure, m_exposureVar );
			auto l_pxl = DoCreate( l_configBuffer );
			auto l_model = GetEngine()->GetRenderSystem()->GetGpuInformations().GetMaxShaderModel();
			l_program->SetSource( ShaderType::Vertex, l_model, l_vtx );
			l_program->SetSource( ShaderType::Pixel, l_model, l_pxl );
			l_return = l_program->Initialise();
		}

		if ( l_return )
		{
			DepthStencilState l_dsState;
			l_dsState.SetDepthTest( false );
			m_pipeline = GetEngine()->GetRenderSystem()->CreatePipeline( std::move( l_dsState ), RasteriserState{}, BlendState{}, MultisampleState{}, *l_program, PipelineFlags{} );
		}

		return l_return;
	}

	void ToneMapping::Cleanup()
	{
		DoDestroy();
		m_exposureVar.reset();

		if ( m_pipeline )
		{
			m_pipeline->Cleanup();
			m_pipeline.reset();
		}
	}

	void ToneMapping::Apply( Size const & p_size, TextureLayout const & p_texture )
	{
		m_exposureVar->SetValue( m_exposure );
		DoUpdate();
		GetEngine()->GetRenderSystem()->GetCurrentContext()->RenderTexture( p_size, p_texture, *m_pipeline );
	}

	bool ToneMapping::WriteInto( Castor::TextFile & p_file )
	{
		return p_file.WriteText( cuT( " -Exposure " ) + string::to_string( m_exposure ) ) > 0
			   && DoWriteInto( p_file );
	}
}
