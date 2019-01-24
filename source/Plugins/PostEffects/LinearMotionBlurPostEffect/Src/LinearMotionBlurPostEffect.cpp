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

#include <ShaderWriter/Source.hpp>

namespace motion_blur
{
	namespace
	{
		std::unique_ptr< sdw::Shader > getVertexProgram( castor3d::RenderSystem * renderSystem )
		{
			using namespace sdw;
			VertexWriter writer;

			// Shader inputs
			Vec2 position = writer.declInput< Vec2 >( cuT( "position" ), 0u );
			Vec2 uv = writer.declInput< Vec2 >( cuT( "uv" ), 1u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ), 0u );
			auto out = writer.getOut();

			writer.implementFunction< sdw::Void >( cuT( "main" ), [&]()
				{
					vtx_texture = uv;
					out.gl_out.gl_Position = vec4( position, 0.0, 1.0 );
				} );
			return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}

		std::unique_ptr< sdw::Shader > getFragmentProgram( castor3d::RenderSystem * renderSystem )
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			Ubo configuration{ writer, cuT( "Configuration" ), 0u, 0u };
			auto c3d_samplesCount = configuration.declMember< UInt >( cuT( "c3d_samplesCount" ) );
			auto c3d_vectorDivider = configuration.declMember< Float >( cuT( "c3d_vectorDivider" ) );
			auto c3d_blurScale = configuration.declMember< Float >( cuT( "c3d_blurScale" ) );
			configuration.end();
			auto c3d_mapVelocity = writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapVelocity" ), 1u, 0u );
			auto c3d_mapDiffuse = writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapDiffuse" ), 2u, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ), 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( cuT( "pxl_fragColor" ), 0u );

			writer.implementFunction< sdw::Void >( cuT( "main" ), [&]()
				{
					auto blurVector = writer.declLocale( cuT( "vector" )
						, writer.paren( texture( c3d_mapVelocity, vtx_texture ).xy() / c3d_vectorDivider ) * c3d_blurScale );
					blurVector.y() = -blurVector.y();
					pxl_fragColor = texture( c3d_mapDiffuse, vtx_texture );

					FOR( writer, UInt, i, 0u, i < c3d_samplesCount, ++i )
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
			return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}
	}

	//*********************************************************************************************

	PostEffect::Quad::Quad( castor3d::RenderSystem & renderSystem
		, castor3d::TextureUnit const & velocity
		, ashes::UniformBuffer< Configuration > const & ubo )
		: castor3d::RenderQuad{ renderSystem, true }
		, m_velocityView{ velocity.getTexture()->getDefaultView() }
		, m_velocitySampler{ velocity.getSampler()->getSampler() }
		, m_ubo{ ubo }
	{
	}

	void PostEffect::Quad::doFillDescriptorSet( ashes::DescriptorSetLayout & descriptorSetLayout
		, ashes::DescriptorSet & descriptorSet )
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
		, m_vertexShader{ ashes::ShaderStageFlag::eVertex, "LinearMotionBlur" }
		, m_pixelShader{ ashes::ShaderStageFlag::eFragment, "LinearMotionBlur" }
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
			, ashes::ShaderStageFlag::eVertex
			, *m_vertexShader.shader );
		visitor.visit( cuT( "MotionBlur" )
			, ashes::ShaderStageFlag::eFragment
			, *m_pixelShader.shader );
	}

	bool PostEffect::doInitialise( castor3d::RenderPassTimer const & timer )
	{
		auto & device = getCurrentDevice( *this );
		ashes::Extent2D size{ m_target->getWidth(), m_target->getHeight() };
		m_vertexShader.shader = getVertexProgram( getRenderSystem() );
		m_pixelShader.shader = getFragmentProgram( getRenderSystem() );
		ashes::ShaderStageStateArray stages;
		stages.push_back( { device.createShaderModule( ashes::ShaderStageFlag::eVertex ) } );
		stages.push_back( { device.createShaderModule( ashes::ShaderStageFlag::eFragment ) } );
		stages[0].module->loadShader( m_target->getRenderSystem()->compileShader( m_vertexShader ) );
		stages[1].module->loadShader( m_target->getRenderSystem()->compileShader( m_pixelShader ) );

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

		m_ubo = ashes::makeUniformBuffer< Configuration >( device
			, 1u
			, 0u
			, ashes::MemoryPropertyFlag::eHostVisible );
		auto & configuration = m_ubo->getData( 0u );
		configuration.samplesCount = m_configuration.samplesCount;
		configuration.vectorDivider = m_configuration.vectorDivider;
		m_ubo->upload( 0u );

		ashes::DescriptorSetLayoutBindingArray bindings
		{
			{ 0u, ashes::DescriptorType::eUniformBuffer, ashes::ShaderStageFlag::eFragment },
			{ 1u, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment },
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

	bool PostEffect::doWriteInto( castor::TextFile & file, castor::String const & tabs )
	{
		static Configuration const ref;
		auto result = file.writeText( cuT( "\n" ) + tabs + Type + cuT( "\n" ) ) > 0
			&& file.writeText( tabs + cuT( "{\n" ) ) > 0;

		if ( result && m_configuration.vectorDivider != ref.vectorDivider )
		{
			result = file.writeText( tabs + cuT( "\tvectorDivider " ) + castor::string::toString( m_configuration.vectorDivider, std::locale{ "C" } ) + cuT( "\n" ) ) > 0;
		}

		if ( result && m_configuration.samplesCount != ref.samplesCount )
		{
			result = file.writeText( tabs + cuT( "\tsamples " ) + castor::string::toString( m_configuration.samplesCount, std::locale{ "C" } ) + cuT( "\n" ) ) > 0;
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
