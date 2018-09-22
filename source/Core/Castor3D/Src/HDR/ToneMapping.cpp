#include "ToneMapping.hpp"

#include "Engine.hpp"

#include "HDR/HdrConfig.hpp"
#include "RenderToTexture/RenderQuad.hpp"
#include "Render/RenderPassTimer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Shader/Program.hpp"
#include "Shader/Ubos/HdrConfigUbo.hpp"
#include "Texture/TextureLayout.hpp"

#include <Descriptor/DescriptorSet.hpp>
#include <Descriptor/DescriptorSetLayout.hpp>
#include <Shader/GlslToSpv.hpp>

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
		, ashes::RenderPass const & renderPass )
	{
		m_hdrConfigUbo.initialise();
		auto & renderSystem = *getEngine()->getRenderSystem();
		m_signalFinished = getCurrentDevice( renderSystem ).createSemaphore();

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
		auto & device = getCurrentDevice( renderSystem );
		ashes::ShaderStageStateArray program
		{
			{ device.createShaderModule( ashes::ShaderStageFlag::eVertex ) },
			{ device.createShaderModule( ashes::ShaderStageFlag::eFragment ) }
		};
		program[0].module->loadShader( compileGlslToSpv( device
			, ashes::ShaderStageFlag::eVertex
			, m_vertexShader.getSource() ) );
		program[1].module->loadShader( compileGlslToSpv( device
			, ashes::ShaderStageFlag::eFragment
			, m_pixelShader.getSource() ) );
		ashes::DescriptorSetLayoutBindingArray bindings
		{
			{ 0u, ashes::DescriptorType::eUniformBuffer, ashes::ShaderStageFlag::eFragment },
		};
		createPipeline( { size[0], size[1] }
			, Position{}
			, program
			, source.getDefaultView()
			, renderPass
			, bindings
			, {} );
		return true;
	}

	void ToneMapping::cleanup()
	{
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
			, ashes::ShaderStageFlag::eVertex
			, m_vertexShader );
		visitor.visit( cuT( "ToneMapping" )
			, ashes::ShaderStageFlag::eFragment
			, m_pixelShader );
		visitor.visit( cuT( "ToneMapping" )
			, ashes::ShaderStageFlag::eFragment
			, m_config );
	}

	void ToneMapping::doFillDescriptorSet( ashes::DescriptorSetLayout & descriptorSetLayout
		, ashes::DescriptorSet & descriptorSet )
	{
		descriptorSet.createBinding( descriptorSetLayout.getBinding( 0u )
			, m_hdrConfigUbo.getUbo()
			, 0u
			, 1u );
	}
}
