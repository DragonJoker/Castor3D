#include "ToneMapping.hpp"

#include "Engine.hpp"

#include "HDR/HdrConfig.hpp"
#include "RenderToTexture/RenderQuad.hpp"
#include "Render/RenderPassTimer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Shader/Ubos/HdrConfigUbo.hpp"
#include "Texture/TextureLayout.hpp"

#include <Descriptor/DescriptorSet.hpp>
#include <Descriptor/DescriptorSetLayout.hpp>

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
		, RenderQuad{ *engine.getRenderSystem(), true, false }
		, m_configUbo{ engine }
	{
	}

	ToneMapping::~ToneMapping()
	{
	}

	bool ToneMapping::initialise( Size const & size
		, TextureLayout const & source
		, renderer::RenderPass const & renderPass )
	{
		auto & renderSystem = *getEngine()->getRenderSystem();
		auto & program = getEngine()->getShaderProgramCache().getNewProgram( false );

		glsl::Shader vtx;
		{
			auto writer = renderSystem.createGlslWriter();

			UBO_MATRIX( writer, 0 );

			// Shader inputs
			auto position = writer.declAttribute< Vec2 >( cuT( "position" ) );
			auto texture = writer.declAttribute< Vec2 >( cuT( "texcoord" ) );

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

		auto pxl = doCreate();
		program.createModule( vtx.getSource(), renderer::ShaderStageFlag::eVertex );
		program.createModule( pxl.getSource(), renderer::ShaderStageFlag::eFragment );
		program.link();
		renderer::DescriptorSetLayoutBindingArray bindings
		{
			{ 0u, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eFragment },
		};
		createPipeline( size
			, Position{}
			, program
			, source.getView()
			, renderPass
			, bindings );
		m_timer = std::make_shared< RenderPassTimer >( *getEngine(), cuT( "Tone mapping" ), cuT( "Tone mapping" ) );

		prepareFrame();
		return true;
	}

	void ToneMapping::cleanup()
	{
		m_timer.reset();
		doDestroy();
		m_configUbo.getUbo().cleanup();
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
			, *m_pipeline );
		m_timer->stop();
	}

	void ToneMapping::doFillDescriptorSet( renderer::DescriptorSetLayout & descriptorSetLayout
		, renderer::DescriptorSet & descriptorSet )
	{
		descriptorSet.createBinding( descriptorSetLayout.getBinding( 0u )
			, m_configUbo.getUbo()
			, 0u );
	}
}
