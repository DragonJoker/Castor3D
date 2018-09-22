#include "GrayScalePostEffect.hpp"

#include <Engine.hpp>
#include <Cache/SamplerCache.hpp>
#include <Cache/ShaderCache.hpp>

#include <Mesh/Vertex.hpp>
#include <Scene/ParticleSystem/ParticleDeclaration.hpp>
#include <Scene/ParticleSystem/ParticleElementDeclaration.hpp>
#include <Miscellaneous/Parameter.hpp>
#include <Render/RenderPassTimer.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Render/RenderTarget.hpp>
#include <Render/RenderWindow.hpp>
#include <Render/Viewport.hpp>
#include <Shader/GlslToSpv.hpp>
#include <Texture/Sampler.hpp>
#include <Texture/TextureLayout.hpp>
#include <Texture/TextureUnit.hpp>

#include <Buffer/VertexBuffer.hpp>
#include <Buffer/UniformBuffer.hpp>
#include <Pipeline/ColourBlendState.hpp>
#include <Pipeline/DepthStencilState.hpp>
#include <Pipeline/Pipeline.hpp>
#include <Pipeline/PipelineLayout.hpp>
#include <Pipeline/RasterisationState.hpp>
#include <Pipeline/ShaderStageState.hpp>
#include <RenderPass/FrameBuffer.hpp>
#include <RenderPass/FrameBufferAttachment.hpp>
#include <RenderPass/RenderPass.hpp>
#include <RenderPass/RenderPassCreateInfo.hpp>
#include <Sync/ImageMemoryBarrier.hpp>

#include <numeric>

#include <GlslSource.hpp>

namespace GrayScale
{
	namespace
	{
		glsl::Shader getVertexProgram( castor3d::RenderSystem * renderSystem )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem->createGlslWriter();

			// Shader inputs
			Vec2 position = writer.declAttribute< Vec2 >( cuT( "position" ), 0u );
			Vec2 texcoord = writer.declAttribute< Vec2 >( cuT( "texcoord" ), 1u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ), 0u );
			auto out = gl_PerVertex{ writer };

			writer.implementFunction< void >( cuT( "main" ), [&]()
				{
					vtx_texture = texcoord;
					out.gl_Position() = vec4( position, 0.0, 1.0 );
				} );
			return writer.finalise();
		}

		glsl::Shader getFragmentProgram( castor3d::RenderSystem * renderSystem )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem->createGlslWriter();

			// Shader inputs
			auto configUbo = glsl::Ubo{ writer, cuT( "Configuration" ), 0u, 0u };
			auto c3d_factors = configUbo.declMember< Vec3 >( cuT( "c3d_factors" ) );
			configUbo.end();
			auto c3d_mapDiffuse = writer.declSampler< Sampler2D >( cuT( "c3d_mapDiffuse" ), 1u, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ), 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declFragData< Vec4 >( cuT( "pxl_fragColor" ), 0u );

			writer.implementFunction< void >( cuT( "main" ), [&]()
				{
					auto colour = writer.declLocale( cuT( "colour" )
						, texture( c3d_mapDiffuse, vtx_texture ).xyz() );
					pxl_fragColor = vec4( vec3( dot( c3d_factors, colour ) ), 1.0 );
				} );
			return writer.finalise();
		}
	}

	//*********************************************************************************************

	PostEffect::Quad::Quad( castor3d::RenderSystem & renderSystem
		, ashes::UniformBuffer< castor::Point3f > const & configUbo )
		: castor3d::RenderQuad{ renderSystem, true }
		, m_configUbo{ configUbo }
	{
	}

	void PostEffect::Quad::doFillDescriptorSet( ashes::DescriptorSetLayout & descriptorSetLayout
		, ashes::DescriptorSet & descriptorSet )
	{
		descriptorSet.createBinding( descriptorSetLayout.getBinding( 0u )
			, m_configUbo
			, 0u );
	}

	//*********************************************************************************************

	castor::String PostEffect::Type = cuT( "grayscale" );
	castor::String PostEffect::Name = cuT( "GrayScale PostEffect" );

	PostEffect::PostEffect( castor3d::RenderTarget & renderTarget
		, castor3d::RenderSystem & renderSystem
		, castor3d::Parameters const & params )
		: castor3d::PostEffect{ PostEffect::Type
			, PostEffect::Name
			, renderTarget
			, renderSystem
			, params
			, false }
		, m_surface{ *renderSystem.getEngine() }
	{
	}

	PostEffect::~PostEffect()
	{
	}

	castor3d::PostEffectSPtr PostEffect::create( castor3d::RenderTarget & renderTarget
		, castor3d::RenderSystem & renderSystem
		, castor3d::Parameters const & params )
	{
		return std::make_shared< PostEffect >( renderTarget, renderSystem, params );
	}

	void PostEffect::update( castor::Nanoseconds const & elapsedTime )
	{
		if ( m_factors.isDirty() )
		{
			m_configUbo->getData() = m_factors.value();
			m_configUbo->upload();
			m_factors.reset();
		}
	}

	void PostEffect::accept( castor3d::PipelineVisitorBase & visitor )
	{
		visitor.visit( cuT( "GrayScale" )
			, ashes::ShaderStageFlag::eVertex
			, m_vertexShader );
		visitor.visit( cuT( "GrayScale" )
			, ashes::ShaderStageFlag::eFragment
			, m_pixelShader );
		visitor.visit( cuT( "GrayScale" )
			, ashes::ShaderStageFlag::eFragment
			, cuT( "Configuration" )
			, cuT( "Factors" )
			, m_factors );
	}

	bool PostEffect::doInitialise( castor3d::RenderPassTimer const & timer )
	{
		auto & device = getCurrentDevice( *this );
		ashes::Extent2D size{ m_target->getWidth(), m_target->getHeight() };
		m_vertexShader = getVertexProgram( getRenderSystem() );
		m_pixelShader = getFragmentProgram( getRenderSystem() );
		ashes::ShaderStageStateArray stages;
		stages.push_back( { device.createShaderModule( ashes::ShaderStageFlag::eVertex ) } );
		stages.push_back( { device.createShaderModule( ashes::ShaderStageFlag::eFragment ) } );
		stages[0].module->loadShader( castor3d::compileGlslToSpv( device
			, ashes::ShaderStageFlag::eVertex
			, m_vertexShader.getSource() ) );
		stages[1].module->loadShader( castor3d::compileGlslToSpv( device
			, ashes::ShaderStageFlag::eFragment
			, m_pixelShader.getSource() ) );

		m_configUbo = ashes::makeUniformBuffer< castor::Point3f >( device
			, 1u
			, 0u
			, ashes::MemoryPropertyFlag::eHostVisible );
		m_configUbo->getData() = m_factors.value();
		m_configUbo->upload();
		m_factors.reset();

		// Create the render pass.
		ashes::RenderPassCreateInfo renderPass;
		renderPass.flags = 0u;

		renderPass.attachments.resize( 1u );
		renderPass.attachments[0].format = m_target->getPixelFormat();
		renderPass.attachments[0].loadOp = ashes::AttachmentLoadOp::eDontCare;
		renderPass.attachments[0].storeOp = ashes::AttachmentStoreOp::eStore;
		renderPass.attachments[0].stencilLoadOp = ashes::AttachmentLoadOp::eDontCare;
		renderPass.attachments[0].stencilStoreOp = ashes::AttachmentStoreOp::eDontCare;
		renderPass.attachments[0].samples = ashes::SampleCountFlag::e1;
		renderPass.attachments[0].initialLayout = ashes::ImageLayout::eUndefined;
		renderPass.attachments[0].finalLayout = ashes::ImageLayout::eColourAttachmentOptimal;

		renderPass.subpasses.resize( 1u );
		renderPass.subpasses[0].flags = 0u;
		renderPass.subpasses[0].pipelineBindPoint = ashes::PipelineBindPoint::eGraphics;
		renderPass.subpasses[0].colorAttachments.push_back( { 0u, ashes::ImageLayout::eColourAttachmentOptimal } );

		renderPass.dependencies.resize( 2u );
		renderPass.dependencies[0].srcSubpass = ashes::ExternalSubpass;
		renderPass.dependencies[0].dstSubpass = 0u;
		renderPass.dependencies[0].srcAccessMask = ashes::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[0].dstAccessMask = ashes::AccessFlag::eShaderRead;
		renderPass.dependencies[0].srcStageMask = ashes::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[0].dstStageMask = ashes::PipelineStageFlag::eFragmentShader;
		renderPass.dependencies[0].dependencyFlags = ashes::DependencyFlag::eByRegion;

		renderPass.dependencies[1].srcSubpass = 0u;
		renderPass.dependencies[1].dstSubpass = ashes::ExternalSubpass;
		renderPass.dependencies[1].srcAccessMask = ashes::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[1].dstAccessMask = ashes::AccessFlag::eShaderRead;
		renderPass.dependencies[1].srcStageMask = ashes::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[1].dstStageMask = ashes::PipelineStageFlag::eFragmentShader;
		renderPass.dependencies[1].dependencyFlags = ashes::DependencyFlag::eByRegion;

		m_renderPass = device.createRenderPass( renderPass );

		ashes::DescriptorSetLayoutBindingArray bindings
		{
			{ 0u, ashes::DescriptorType::eUniformBuffer, ashes::ShaderStageFlag::eFragment }
		};
		m_quad = std::make_unique< Quad >( *getRenderSystem(), *m_configUbo );
		m_quad->createPipeline( size
			, castor::Position{}
			, stages
			, m_target->getDefaultView()
			, *m_renderPass
			, bindings
			, {} );

		auto result = m_surface.initialise( *m_renderPass
			, castor::Size{ m_target->getWidth(), m_target->getHeight() }
			, m_target->getPixelFormat() );
		castor3d::CommandsSemaphore commands
		{
			device.getGraphicsCommandPool().createCommandBuffer(),
			device.createSemaphore()
		};
		auto & cmd = *commands.commandBuffer;

		if ( result )
		{
			cmd.begin();
			timer.beginPass( cmd );

			// Put target image in shader input layout.
			cmd.memoryBarrier( ashes::PipelineStageFlag::eColourAttachmentOutput
				, ashes::PipelineStageFlag::eFragmentShader
				, m_target->getDefaultView().makeShaderInputResource( ashes::ImageLayout::eUndefined, 0u ) );

			cmd.beginRenderPass( *m_renderPass
				, *m_surface.frameBuffer
				, { ashes::ClearColorValue{} }
				, ashes::SubpassContents::eInline );
			m_quad->registerFrame( cmd );
			cmd.endRenderPass();

			timer.endPass( cmd );
			cmd.end();
			m_commands.emplace_back( std::move( commands ) );
		}

		m_result = m_surface.colourTexture.get();
		return result;
	}

	void PostEffect::doCleanup()
	{
		m_quad.reset();
		m_renderPass.reset();
		m_surface.cleanup();
		m_configUbo.reset();
	}

	bool PostEffect::doWriteInto( castor::TextFile & file, castor::String const & tabs )
	{
		return file.writeText( tabs + cuT( "postfx \"" ) + Type + cuT( "\"" ) ) > 0;
	}

	//*********************************************************************************************
}
