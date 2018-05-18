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
		, castor::String const & fullName
		, Engine & engine
		, HdrConfig & config
		, Parameters const & parameters )
		: OwnedBy< Engine >{ engine }
		, Named{ name }
		, RenderQuad{ *engine.getRenderSystem(), true, false }
		, m_config{ config }
		, m_fullName{ fullName }
		, m_hdrConfigUbo{ engine }
	{
	}

	ToneMapping::~ToneMapping()
	{
	}

	bool ToneMapping::initialise( Size const & size
		, TextureLayout const & source
		, renderer::RenderPass const & renderPass )
	{
		m_hdrConfigUbo.initialise();
		auto & renderSystem = *getEngine()->getRenderSystem();
		m_signalFinished = renderSystem.getCurrentDevice()->createSemaphore();

		{
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			auto position = writer.declAttribute< Vec2 >( cuT( "position" ), 0u );
			auto texcoord = writer.declAttribute< Vec2 >( cuT( "texcoord" ), 1u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ), 0u );
			auto out = gl_PerVertex{ writer };

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				vtx_texture = texcoord;
				out.gl_Position() = vec4( position.x(), position.y(), 0.0, 1.0 );
			} );

			m_vertexShader = writer.finalise();
		}

		m_pixelShader = doCreate();
		renderer::ShaderStageStateArray program
		{
			{ renderSystem.getCurrentDevice()->createShaderModule( renderer::ShaderStageFlag::eVertex ) },
			{ renderSystem.getCurrentDevice()->createShaderModule( renderer::ShaderStageFlag::eFragment ) }
		};
		program[0].module->loadShader( m_vertexShader.getSource() );
		program[1].module->loadShader( m_pixelShader.getSource() );
		renderer::DescriptorSetLayoutBindingArray bindings
		{
			{ 0u, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eFragment },
		};
		createPipeline( { size[0], size[1] }
			, Position{}
			, program
			, source.getDefaultView()
			, renderPass
			, bindings
			, {} );
		m_timer = std::make_shared< RenderPassTimer >( *getEngine(), cuT( "Tone mapping" ), cuT( "Tone mapping" ) );
		return true;
	}

	void ToneMapping::cleanup()
	{
		m_timer.reset();
		doDestroy();
		m_hdrConfigUbo.cleanup();
	}

	void ToneMapping::update()
	{
		m_hdrConfigUbo.update( m_config );
		doUpdate();
	}

	void ToneMapping::accept( ToneMappingVisitor & visitor )
	{
		visitor.visit( cuT( "ToneMapping" )
			, renderer::ShaderStageFlag::eVertex
			, m_vertexShader );
		visitor.visit( cuT( "ToneMapping" )
			, renderer::ShaderStageFlag::eFragment
			, m_pixelShader );
		visitor.visit( cuT( "ToneMapping" )
			, renderer::ShaderStageFlag::eFragment
			, m_config );
	}

	void ToneMapping::doFillDescriptorSet( renderer::DescriptorSetLayout & descriptorSetLayout
		, renderer::DescriptorSet & descriptorSet )
	{
		descriptorSet.createBinding( descriptorSetLayout.getBinding( 0u )
			, m_hdrConfigUbo.getUbo()
			, 0u
			, 1u );
	}
}
