#include "ToneMapping.hpp"

#include "Engine.hpp"

#include "RenderToTexture/RenderColourToTexture.hpp"
#include "Render/RenderPassTimer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Shader/Ubos/HdrConfigUbo.hpp"

#include <GlslSource.hpp>

using namespace castor;
using namespace glsl;

namespace castor3d
{
	ToneMapping::ToneMapping( castor::String const & name
		, Engine & engine
		, Parameters const & parameters )
		: OwnedBy< Engine >{ engine }
		, Named{ name }
		, m_matrixUbo{ engine }
		, m_configUbo{ engine }
	{
	}

	ToneMapping::~ToneMapping()
	{
	}

	bool ToneMapping::initialise()
	{
		auto program = getEngine()->getShaderProgramCache().getNewProgram( false );
		bool result = program != nullptr;

		if ( result )
		{
			glsl::Shader vtx;
			{
				auto writer = getEngine()->getRenderSystem()->createGlslWriter();

				UBO_MATRIX( writer );

				// Shader inputs
				auto position = writer.declAttribute< Vec2 >( ShaderProgram::Position );
				auto texture = writer.declAttribute< Vec2 >( ShaderProgram::Texture );

				// Shader outputs
				auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ) );
				auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

				writer.implementFunction< void >( cuT( "main" ), [&]()
				{
					vtx_texture = texture;
					gl_Position = c3d_projection * vec4( position.x(), position.y(), 0.0, 1.0 );
				} );

				vtx = writer.finalise();
			}

			program->createObject( ShaderType::eVertex );
			program->createObject( ShaderType::ePixel );
			auto pxl = doCreate();
			program->setSource( ShaderType::eVertex, vtx );
			program->setSource( ShaderType::ePixel, pxl );
			program->createUniform< UniformType::eSampler >( ShaderProgram::MapDiffuse, ShaderType::ePixel )->setValue( MinTextureIndex );
			result = program->initialise();
		}

		if ( result )
		{
			DepthStencilState dsState;
			dsState.setDepthTest( false );
			dsState.setDepthMask( WritingMask::eZero );
			m_pipeline = getEngine()->getRenderSystem()->createRenderPipeline( std::move( dsState ), RasteriserState{}, BlendState{}, MultisampleState{}, *program, PipelineFlags{} );
			m_pipeline->addUniformBuffer( m_matrixUbo.getUbo() );
			m_pipeline->addUniformBuffer( m_configUbo.getUbo() );

			m_colour = std::make_unique< RenderColourToTexture >( *getEngine()->getRenderSystem()->getMainContext(), m_matrixUbo );
			m_colour->initialise();

			m_timer = std::make_shared< RenderPassTimer >( *getEngine(), cuT( "Tone mapping" ), cuT( "Tone mapping" ) );
		}

		return result;
	}

	void ToneMapping::cleanup()
	{
		m_timer.reset();
		doDestroy();

		if ( m_colour )
		{
			m_colour->cleanup();
			m_colour.reset();
		}

		if ( m_pipeline )
		{
			m_pipeline->cleanup();
			m_pipeline.reset();
		}

		m_configUbo.getUbo().cleanup();
		m_matrixUbo.getUbo().cleanup();
	}

	void ToneMapping::update( HdrConfig const & config )
	{
		m_configUbo.update( config );
		doUpdate();
	}

	void ToneMapping::apply( Size const & size
		, TextureLayout const & texture
		, RenderInfo & info )
	{
		static Position const position;
		m_timer->start();
		m_colour->render( position
			, size
			, texture
			, m_matrixUbo
			, *m_pipeline );
		m_timer->stop();
	}
}
