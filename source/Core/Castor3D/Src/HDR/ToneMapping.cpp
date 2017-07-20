#include "ToneMapping.hpp"

#include "Engine.hpp"

#include "RenderToTexture/RenderColourToTexture.hpp"
#include "Render/RenderPipeline.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Shader/HdrConfigUbo.hpp"

#include <GlslSource.hpp>

using namespace Castor;
using namespace GLSL;

namespace Castor3D
{
	String const ToneMapping::HdrConfigUbo = cuT( "HdrConfig" );

	ToneMapping::ToneMapping( Castor::String const & p_name, Engine & p_engine, Parameters const & p_parameters )
		: OwnedBy< Engine >{ p_engine }
		, Named{ p_name }
		, m_matrixUbo{ p_engine }
		, m_configUbo{ ToneMapping::HdrConfigUbo, *p_engine.GetRenderSystem() }
	{
		m_exposureVar = m_configUbo.CreateUniform< UniformType::eFloat >( ShaderProgram::Exposure );
		
		String param;

		if ( p_parameters.Get( cuT( "Exposure" ), param ) )
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
		}

		return result;
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
		m_matrixUbo.GetUbo().Cleanup();

		if ( m_pipeline )
		{
			m_pipeline->Cleanup();
			m_pipeline.reset();
		}
	}

	void ToneMapping::Apply( Size const & p_size, TextureLayout const & p_texture )
	{
		static Position const position;
		m_exposureVar->SetValue( m_config.GetExposure() );
		DoUpdate();
		m_configUbo.Update();
		m_configUbo.BindTo( HdrConfigUbo::BindingPoint );
		m_colour->Render( position
			, p_size
			, p_texture
			, m_matrixUbo
			, *m_pipeline );
	}

	bool ToneMapping::WriteInto( Castor::TextFile & p_file )
	{
		return p_file.WriteText( cuT( " -Exposure=" ) + string::to_string( m_config.GetExposure() ) ) > 0
			   && DoWriteInto( p_file );
	}

	void ToneMapping::SetConfig( HdrConfig const & p_config )
	{
		m_config = p_config;
	}
}
