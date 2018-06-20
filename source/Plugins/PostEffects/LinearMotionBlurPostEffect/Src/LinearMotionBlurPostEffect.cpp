#include "LinearMotionBlurPostEffect.hpp"

#include <Engine.hpp>
#include <Cache/SamplerCache.hpp>
#include <Cache/ShaderCache.hpp>
#include <Mesh/Vertex.hpp>
#include <Scene/ParticleSystem/ParticleDeclaration.hpp>
#include <Scene/ParticleSystem/ParticleElementDeclaration.hpp>
#include <Miscellaneous/Parameter.hpp>
#include <Render/RenderLoop.hpp>
#include <Render/RenderPass.hpp>
#include <Render/RenderPassTimer.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Render/RenderTarget.hpp>
#include <Render/RenderWindow.hpp>
#include <Render/Viewport.hpp>
#include <Shader/ShaderProgram.hpp>
#include <Technique/RenderTechnique.hpp>
#include <Technique/RenderTechniquePass.hpp>
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

namespace motion_blur
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
			Ubo configuration{ writer, cuT( "Configuration" ), 0u, 0u };
			auto c3d_samplesCount = configuration.declMember< UInt >( cuT( "c3d_samplesCount" ) );
			auto c3d_vectorDivider = configuration.declMember< Float >( cuT( "c3d_vectorDivider" ) );
			auto c3d_blurScale = configuration.declMember< Float >( cuT( "c3d_blurScale" ) );
			configuration.end();
			auto c3d_mapVelocity = writer.declSampler< Sampler2D >( cuT( "c3d_mapVelocity" ), 1u, 0u );
			auto c3d_mapDiffuse = writer.declSampler< Sampler2D >( cuT( "c3d_mapDiffuse" ), 2u, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ), 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declFragData< Vec4 >( cuT( "pxl_fragColor" ), 0u );

			writer.implementFunction< void >( cuT( "main" ), [&]()
				{
					auto blurVector = writer.declLocale( cuT( "vector" )
						, writer.paren( texture( c3d_mapVelocity, vtx_texture ).xy() / c3d_vectorDivider ) * c3d_blurScale );
					blurVector.y() = -blurVector.y();
					pxl_fragColor = texture( c3d_mapDiffuse, vtx_texture );

					FOR( writer, UInt, i, 0u, "i < c3d_samplesCount", "++i" )
					{
						auto offset = writer.declLocale( cuT( "offset" )
							, blurVector * writer.paren( writer.cast< Float >( i ) / writer.cast< Float >( c3d_samplesCount - 1u ) - 0.5f ) );
						pxl_fragColor += texture( c3d_mapDiffuse, vtx_texture + offset );
					}
					ROF;

					pxl_fragColor /= writer.cast< Float >( c3d_samplesCount );
					//IF( writer, vector == vec2( 0.0_f ) )
					//{
					//	pxl_fragColor = vec4( 1.0_f );
					//}
					//ELSE
					//{
					//	pxl_fragColor = vec4( vector, 0.0, 1.0 );
					//}
					//FI;
				} );
			return writer.finalise();
		}
	}

	//*********************************************************************************************

	PostEffect::Quad::Quad( castor3d::RenderSystem & renderSystem
		, castor3d::TextureUnit const & velocity
		, renderer::UniformBuffer< Configuration > const & ubo )
		: castor3d::RenderQuad{ renderSystem, true }
		, m_velocityView{ velocity.getTexture()->getDefaultView() }
		, m_velocitySampler{ velocity.getSampler()->getSampler() }
		, m_ubo{ ubo }
	{
	}

	void PostEffect::Quad::doFillDescriptorSet( renderer::DescriptorSetLayout & descriptorSetLayout
		, renderer::DescriptorSet & descriptorSet )
	{
		descriptorSet.createBinding( descriptorSetLayout.getBinding( 0u )
			, m_ubo
			, 0u );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( 1u )
			, m_velocityView
			, m_velocitySampler );
	}

	//*********************************************************************************************

	castor::String PostEffect::Type = cuT( "linear_motion_blur" );
	castor::String PostEffect::Name = cuT( "LinearMotionBlur PostEffect" );

	PostEffect::PostEffect( castor3d::RenderTarget & renderTarget
		, castor3d::RenderSystem & renderSystem
		, castor3d::Parameters const & parameters )
		: castor3d::PostEffect{ PostEffect::Type
			, PostEffect::Name
			, renderTarget
			, renderSystem
			, parameters
			, false }
		, m_surface{ *renderSystem.getEngine() }
	{
		parameters.get( cuT( "vectorDivider" ), m_configuration.vectorDivider );
		parameters.get( cuT( "samplesCount" ), m_configuration.samplesCount );
		parameters.get( cuT( "fpsScale" ), m_fpsScale );
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
		if ( m_fpsScale )
		{
			auto current = Clock::now();
			auto duration = std::chrono::duration_cast< std::chrono::milliseconds >( current - m_saved );
			auto fps = 1000.0f / duration.count();
			auto & configuration = m_ubo->getData( 0u );
			configuration.samplesCount = m_configuration.samplesCount;
			configuration.vectorDivider = m_configuration.vectorDivider;
			configuration.blurScale = m_fpsScale
				? fps / getRenderSystem()->getEngine()->getRenderLoop().getWantedFps()
				: 1.0f;
			m_ubo->upload( 0u );
			m_saved = current;
		}
	}

	void PostEffect::accept( castor3d::PipelineVisitorBase & visitor )
	{
		visitor.visit( cuT( "MotionBlur" )
			, renderer::ShaderStageFlag::eVertex
			, m_vertexShader );
		visitor.visit( cuT( "MotionBlur" )
			, renderer::ShaderStageFlag::eFragment
			, m_pixelShader );
	}

	bool PostEffect::doInitialise( castor3d::RenderPassTimer const & timer )
	{
		auto & device = getCurrentDevice( *this );
		renderer::Extent2D size{ m_target->getWidth(), m_target->getHeight() };
		m_vertexShader = getVertexProgram( getRenderSystem() );
		m_pixelShader = getFragmentProgram( getRenderSystem() );
		renderer::ShaderStageStateArray stages;
		stages.push_back( { device.createShaderModule( renderer::ShaderStageFlag::eVertex ) } );
		stages.push_back( { device.createShaderModule( renderer::ShaderStageFlag::eFragment ) } );
		stages[0].module->loadShader( m_vertexShader.getSource() );
		stages[1].module->loadShader( m_pixelShader.getSource() );

		// Create the render pass.
		renderer::RenderPassCreateInfo renderPass;
		renderPass.flags = 0u;

		renderPass.attachments.resize( 1u );
		renderPass.attachments[0].format = m_target->getPixelFormat();
		renderPass.attachments[0].loadOp = renderer::AttachmentLoadOp::eDontCare;
		renderPass.attachments[0].storeOp = renderer::AttachmentStoreOp::eStore;
		renderPass.attachments[0].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
		renderPass.attachments[0].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
		renderPass.attachments[0].samples = renderer::SampleCountFlag::e1;
		renderPass.attachments[0].initialLayout = renderer::ImageLayout::eUndefined;
		renderPass.attachments[0].finalLayout = renderer::ImageLayout::eColourAttachmentOptimal;

		renderPass.subpasses.resize( 1u );
		renderPass.subpasses[0].flags = 0u;
		renderPass.subpasses[0].pipelineBindPoint = renderer::PipelineBindPoint::eGraphics;
		renderPass.subpasses[0].colorAttachments.push_back( { 0u, renderer::ImageLayout::eColourAttachmentOptimal } );

		renderPass.dependencies.resize( 2u );
		renderPass.dependencies[0].srcSubpass = renderer::ExternalSubpass;
		renderPass.dependencies[0].dstSubpass = 0u;
		renderPass.dependencies[0].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[0].dstAccessMask = renderer::AccessFlag::eShaderRead;
		renderPass.dependencies[0].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[0].dstStageMask = renderer::PipelineStageFlag::eFragmentShader;
		renderPass.dependencies[0].dependencyFlags = renderer::DependencyFlag::eByRegion;

		renderPass.dependencies[1].srcSubpass = 0u;
		renderPass.dependencies[1].dstSubpass = renderer::ExternalSubpass;
		renderPass.dependencies[1].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[1].dstAccessMask = renderer::AccessFlag::eShaderRead;
		renderPass.dependencies[1].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[1].dstStageMask = renderer::PipelineStageFlag::eFragmentShader;
		renderPass.dependencies[1].dependencyFlags = renderer::DependencyFlag::eByRegion;

		m_renderPass = device.createRenderPass( renderPass );

		m_ubo = renderer::makeUniformBuffer< Configuration >( device
			, 1u
			, 0u
			, renderer::MemoryPropertyFlag::eHostVisible );
		auto & configuration = m_ubo->getData( 0u );
		configuration.samplesCount = m_configuration.samplesCount;
		configuration.vectorDivider = m_configuration.vectorDivider;
		m_ubo->upload( 0u );

		renderer::DescriptorSetLayoutBindingArray bindings
		{
			{ 0u, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eFragment },
			{ 1u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment },
		};
		m_quad = std::make_unique< Quad >( *getRenderSystem()
			, m_renderTarget.getVelocity()
			, *m_ubo );
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

		if ( result && cmd.begin() )
		{
			timer.beginPass( cmd );

			// Put target image in shader input layout.
			cmd.memoryBarrier( renderer::PipelineStageFlag::eColourAttachmentOutput
				, renderer::PipelineStageFlag::eFragmentShader
				, m_target->getDefaultView().makeShaderInputResource( renderer::ImageLayout::eUndefined, 0u ) );

			cmd.beginRenderPass( *m_renderPass
				, *m_surface.frameBuffer
				, { renderer::ClearColorValue{} }
				, renderer::SubpassContents::eInline );
			m_quad->registerFrame( cmd );
			cmd.endRenderPass();

			timer.endPass( cmd );
			cmd.end();
			m_commands.emplace_back( std::move( commands ) );
		}

		m_result = m_surface.colourTexture.get();
		m_saved = Clock::now();
		return result;
	}

	void PostEffect::doCleanup()
	{
		m_quad.reset();
		m_ubo.reset();
		m_renderPass.reset();
		m_surface.cleanup();
	}

	bool PostEffect::doWriteInto( castor::TextFile & file, String const & tabs )
	{
		static Configuration const ref;
		auto result = file.writeText( cuT( "\n" ) + tabs + Type + cuT( "\n" ) ) > 0
			&& file.writeText( tabs + cuT( "{\n" ) ) > 0;

		if ( result && m_configuration.vectorDivider != ref.vectorDivider )
		{
			result = file.writeText( tabs + cuT( "\tvectorDivider " ) + string::toString( m_configuration.vectorDivider, std::locale{ "C" } ) + cuT( "\n" ) ) > 0;
		}

		if ( result && m_configuration.samplesCount != ref.samplesCount )
		{
			result = file.writeText( tabs + cuT( "\tsamples " ) + string::toString( m_configuration.samplesCount, std::locale{ "C" } ) + cuT( "\n" ) ) > 0;
		}

		if ( result && !m_fpsScale )
		{
			result = file.writeText( tabs + cuT( "\tfpsScale false\n" ) ) > 0;
		}

		if ( result )
		{
			result = file.writeText( tabs + cuT( "}\n" ) ) > 0;
		}

		return result;
	}

	//*********************************************************************************************
}
