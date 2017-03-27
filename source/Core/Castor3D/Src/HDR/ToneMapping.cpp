#include "ToneMapping.hpp"

#include "Engine.hpp"

#include "RenderToTexture/RenderColourToTexture.hpp"
#include "Render/RenderPipeline.hpp"
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
		, m_matrixUbo{ ShaderProgram::BufferMatrix, *p_engine.GetRenderSystem() }
		, m_configUbo{ ToneMapping::HdrConfig, *p_engine.GetRenderSystem() }
	{
		UniformBuffer::FillMatrixBuffer( m_matrixUbo );
		m_exposureVar = m_configUbo.CreateUniform < UniformType::eFloat > (ToneMapping::Exposure);
		
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
		auto l_program = GetEngine()->GetShaderProgramCache().GetNewProgram( false );
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

			l_program->CreateObject( ShaderType::eVertex );
			l_program->CreateObject( ShaderType::ePixel );
			auto l_pxl = DoCreate();
			auto l_model = GetEngine()->GetRenderSystem()->GetGpuInformations().GetMaxShaderModel();
			l_program->SetSource( ShaderType::eVertex, l_model, l_vtx );
			l_program->SetSource( ShaderType::ePixel, l_model, l_pxl );
			l_return = l_program->Initialise();
		}

		if ( l_return )
		{
			DepthStencilState l_dsState;
			l_dsState.SetDepthTest( false );
			l_dsState.SetDepthMask( WritingMask::eZero );
			m_pipeline = GetEngine()->GetRenderSystem()->CreateRenderPipeline( std::move( l_dsState ), RasteriserState{}, BlendState{}, MultisampleState{}, *l_program, PipelineFlags{} );
			m_pipeline->AddUniformBuffer( m_matrixUbo );
			m_pipeline->AddUniformBuffer( m_configUbo );

			m_colour = std::make_unique< RenderColourToTexture >( *GetEngine()->GetRenderSystem()->GetMainContext(), m_matrixUbo );
			m_colour->Initialise();
		}

		return l_return;
	}

	void ToneMapping::Cleanup()
	{
		DoDestroy();

		if ( m_colour )
		{
			m_colour->Cleanup();
			m_colour.reset();
		}

		m_exposureVar.reset();
		m_configUbo.Cleanup();
		m_matrixUbo.Cleanup();

		if ( m_pipeline )
		{
			m_pipeline->Cleanup();
			m_pipeline.reset();
		}
	}

	void ToneMapping::Apply( Size const & p_size, TextureLayout const & p_texture )
	{
		static Position const l_position;
		m_exposureVar->SetValue( m_exposure );
		DoUpdate();
		m_configUbo.Update();
		m_colour->Render( l_position
			, p_size
			, p_texture
			, m_matrixUbo
			, *m_pipeline );
	}

	bool ToneMapping::WriteInto( Castor::TextFile & p_file )
	{
		return p_file.WriteText( cuT( " -Exposure=" ) + string::to_string( m_exposure ) ) > 0
			   && DoWriteInto( p_file );
	}
}
