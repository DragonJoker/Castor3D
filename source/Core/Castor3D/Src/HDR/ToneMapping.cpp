#include "ToneMapping.hpp"

#include "Engine.hpp"

#include "RenderToTexture/RenderColourToTexture.hpp"
#include "Render/RenderPassTimer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Shader/HdrConfigUbo.hpp"

#include <GlslSource.hpp>

using namespace Castor;
using namespace GLSL;

namespace Castor3D
{
	String const ToneMapping::HdrConfigUbo = cuT( "HdrConfig" );

	ToneMapping::ToneMapping( Castor::String const & name
		, Engine & engine, Parameters const & parameters )
		: OwnedBy< Engine >{ engine }
		, Named{ name }
		, m_matrixUbo{ engine }
		, m_configUbo{ ToneMapping::HdrConfigUbo
			, *engine.GetRenderSystem()
			, HdrConfigUbo::BindingPoint }
	{
		m_exposureVar = m_configUbo.CreateUniform< UniformType::eFloat >( ShaderProgram::Exposure );
		
		String param;

		if ( parameters.Get( cuT( "Exposure" ), param ) )
		{
			m_config.SetExposure( string::to_float( param ) );
		}
	}

	ToneMapping::~ToneMapping()
	{
	}

	bool ToneMapping::Initialise()
	{
		auto program = GetEngine()->GetShaderProgramCache().GetNewProgram( false );
		bool result = program != nullptr;

		if ( result )
		{
			GLSL::Shader vtx;
			{
				auto writer = GetEngine()->GetRenderSystem()->CreateGlslWriter();

				UBO_MATRIX( writer );

				// Shader inputs
				auto position = writer.DeclAttribute< Vec2 >( ShaderProgram::Position );
				auto texture = writer.DeclAttribute< Vec2 >( ShaderProgram::Texture );

				// Shader outputs
				auto vtx_texture = writer.DeclOutput< Vec2 >( cuT( "vtx_texture" ) );
				auto gl_Position = writer.DeclBuiltin< Vec4 >( cuT( "gl_Position" ) );

				writer.ImplementFunction< void >( cuT( "main" ), [&]()
				{
					vtx_texture = texture;
					gl_Position = c3d_mtxProjection * vec4( position.x(), position.y(), 0.0, 1.0 );
				} );

				vtx = writer.Finalise();
			}

			program->CreateObject( ShaderType::eVertex );
			program->CreateObject( ShaderType::ePixel );
			auto pxl = DoCreate();
			program->SetSource( ShaderType::eVertex, vtx );
			program->SetSource( ShaderType::ePixel, pxl );
			result = program->Initialise();
		}

		if ( result )
		{
			DepthStencilState dsState;
			dsState.SetDepthTest( false );
			dsState.SetDepthMask( WritingMask::eZero );
			m_pipeline = GetEngine()->GetRenderSystem()->CreateRenderPipeline( std::move( dsState ), RasteriserState{}, BlendState{}, MultisampleState{}, *program, PipelineFlags{} );
			m_pipeline->AddUniformBuffer( m_matrixUbo.GetUbo() );
			m_pipeline->AddUniformBuffer( m_configUbo );

			m_colour = std::make_unique< RenderColourToTexture >( *GetEngine()->GetRenderSystem()->GetMainContext(), m_matrixUbo );
			m_colour->Initialise();

			m_timer = std::make_shared< RenderPassTimer >( *GetEngine(), cuT( "Tone mapping" ) );
		}

		return result;
	}

	void ToneMapping::Cleanup()
	{
		m_timer.reset();
		DoDestroy();

		if ( m_colour )
		{
			m_colour->Cleanup();
			m_colour.reset();
		}

		m_exposureVar.reset();
		m_configUbo.Cleanup();
		m_matrixUbo.GetUbo().Cleanup();

		if ( m_pipeline )
		{
			m_pipeline->Cleanup();
			m_pipeline.reset();
		}
	}

	void ToneMapping::Apply( Size const & size
		, TextureLayout const & texture
		, RenderInfo & info )
	{
		static Position const position;
		m_timer->Start();
		m_exposureVar->SetValue( m_config.GetExposure() );
		DoUpdate();
		m_configUbo.Update();
		m_configUbo.BindTo( HdrConfigUbo::BindingPoint );
		m_colour->Render( position
			, size
			, texture
			, m_matrixUbo
			, *m_pipeline );
		m_timer->Stop();
	}

	bool ToneMapping::WriteInto( Castor::TextFile & file )
	{
		return file.WriteText( cuT( " -Exposure=" ) + string::to_string( m_config.GetExposure() ) ) > 0
			   && DoWriteInto( file );
	}

	void ToneMapping::SetConfig( HdrConfig const & config )
	{
		m_config = config;
	}
}
