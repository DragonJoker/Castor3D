#include "ToneMapping.hpp"

#include "Engine.hpp"

#include "HDR/HdrConfig.hpp"
#include "RenderToTexture/RenderQuad.hpp"
#include "Render/RenderPassTimer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Shader/Program.hpp"
#include "Shader/Ubos/HdrConfigUbo.hpp"
#include "Texture/TextureLayout.hpp"

#include <Ashes/Descriptor/DescriptorSet.hpp>
#include <Ashes/Descriptor/DescriptorSetLayout.hpp>
#include <Shader/GlslToSpv.hpp>

#include <ShaderWriter/Source.hpp>

using namespace castor;
using namespace sdw;

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
		, m_vertexShader{ ashes::ShaderStageFlag::eVertex, "ToneMapping" }
		, m_pixelShader{ ashes::ShaderStageFlag::eFragment, "ToneMapping" }
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
			VertexWriter writer;

			// Shader inputs
			auto position = writer.declInput< Vec2 >( cuT( "position" ), 0u );
			auto uv = writer.declInput< Vec2 >( cuT( "uv" ), 1u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ), 0u );
			auto out = writer.getOut();

			writer.implementFunction< sdw::Void >( cuT( "main" )
				, [&]()
				{
					vtx_texture = uv;
					out.gl_out.gl_Position = vec4( position.x(), position.y(), 0.0, 1.0 );
				} );

			m_vertexShader.shader = std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}

		m_pixelShader.shader = doCreate();
		auto & device = getCurrentDevice( renderSystem );
		ashes::ShaderStageStateArray program
		{
			{ device.createShaderModule( ashes::ShaderStageFlag::eVertex ) },
			{ device.createShaderModule( ashes::ShaderStageFlag::eFragment ) }
		};
		program[0].module->loadShader( getEngine()->getRenderSystem()->compileShader( m_vertexShader ) );
		program[1].module->loadShader( getEngine()->getRenderSystem()->compileShader( m_pixelShader ) );
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
			, *m_vertexShader.shader );
		visitor.visit( cuT( "ToneMapping" )
			, ashes::ShaderStageFlag::eFragment
			, *m_pixelShader.shader );
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
