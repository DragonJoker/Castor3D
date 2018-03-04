#include "LineariseDepthPass.hpp"

#include "Engine.hpp"
#include "Render/RenderPassTimer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Shader/Ubos/MatrixUbo.hpp"
#include "Technique/Opaque/Ssao/SsaoConfigUbo.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"
#include "Texture/TextureUnit.hpp"

#include <Buffer/GeometryBuffers.hpp>
#include <Buffer/VertexBuffer.hpp>
#include <Command/CommandBuffer.hpp>
#include <Command/Queue.hpp>
#include <Core/Device.hpp>
#include <Descriptor/DescriptorSet.hpp>
#include <Descriptor/DescriptorSetBinding.hpp>
#include <Descriptor/DescriptorSetLayout.hpp>
#include <Descriptor/DescriptorSetLayoutBinding.hpp>
#include <Descriptor/DescriptorSetPool.hpp>
#include <Image/Texture.hpp>
#include <Image/TextureView.hpp>
#include <Pipeline/ColourBlendState.hpp>
#include <Pipeline/DepthStencilState.hpp>
#include <Pipeline/MultisampleState.hpp>
#include <Pipeline/RasterisationState.hpp>
#include <Pipeline/VertexLayout.hpp>
#include <RenderPass/FrameBuffer.hpp>
#include <RenderPass/FrameBufferAttachment.hpp>
#include <RenderPass/RenderPass.hpp>
#include <RenderPass/RenderPassAttachment.hpp>
#include <RenderPass/RenderPassState.hpp>
#include <RenderPass/RenderSubpass.hpp>
#include <RenderPass/RenderSubpassState.hpp>

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

#include "Shader/Shaders/GlslLight.hpp"
#include "Shader/Shaders/GlslShadow.hpp"

#include <random>

using namespace castor;

namespace castor3d
{
	namespace
	{
		glsl::Shader doGetVertexProgram( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			auto position = writer.declAttribute< Vec2 >( cuT( "position" ), 0u );

			// Shader outputs
			auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
				{
					gl_Position = vec4( position, 0.0, 1.0 );
				} );
			return writer.finalise();
		}
		
		glsl::Shader doGetLinearisePixelProgram( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			auto c3d_mapDepth = writer.declSampler< Sampler2D >( cuT( "c3d_mapDepth" ), 0u, 0u );
			auto c3d_clipInfo = writer.declUniform< Vec3 >( cuT( "c3d_clipInfo" ), 1u );
			auto gl_FragCoord = writer.declBuiltin< Vec4 >( cuT( "gl_FragCoord" ) );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Float >( cuT( "pxl_fragColor" ), 0u );

			writer.implementFunction< Void >( cuT( "main" )
				, [&]()
				{
					auto depth = writer.declLocale( cuT( "depth" )
						, texelFetch( c3d_mapDepth, ivec2( gl_FragCoord.xy() ), 0 ).r() );
					pxl_fragColor = c3d_clipInfo[0] / writer.paren( c3d_clipInfo[1] * depth + c3d_clipInfo[2] );
				} );
			return writer.finalise();
		}
		
		glsl::Shader doGetMinifyPixelProgram( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			auto c3d_mapDepth = writer.declSampler< Sampler2D >( cuT( "c3d_mapDepth" ), 0u, 0u );
			auto c3d_previousLevel = writer.declUniform< Int >( cuT( "c3d_previousLevel" ), 1u );
			auto gl_FragCoord = writer.declBuiltin< Vec4 >( cuT( "gl_FragCoord" ) );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Float >( cuT( "pxl_fragColor" ), 0u );

			writer.implementFunction< Void >( cuT( "main" )
				, [&]()
				{
					auto ssPosition = writer.declLocale( cuT( "ssPosition" )
						, ivec2( gl_FragCoord.xy() ) );

					// Rotated grid subsampling to avoid XY directional bias or Z precision bias while downsampling.
					pxl_fragColor = texelFetch( c3d_mapDepth
						, clamp( ssPosition * 2 + ivec2( ssPosition.y() & 1, ssPosition.x() & 1 )
							, ivec2( 0_i )
							, textureSize( c3d_mapDepth, c3d_previousLevel ) - ivec2( 1_i ) )
						, c3d_previousLevel ).r();
				} );
			return writer.finalise();
		}

		renderer::ShaderProgram & doGetLineariseProgram( Engine & engine )
		{
			auto vtx = doGetVertexProgram( engine );
			auto pxl = doGetLinearisePixelProgram( engine );
			auto & program = engine.getShaderProgramCache().getNewProgram( false );
			program.createModule( vtx.getSource(), renderer::ShaderStageFlag::eVertex );
			program.createModule( pxl.getSource(), renderer::ShaderStageFlag::eFragment );
			return program;
		}

		renderer::ShaderProgram & doGetMinifyProgram( Engine & engine )
		{
			auto vtx = doGetVertexProgram( engine );
			auto pxl = doGetMinifyPixelProgram( engine );
			auto & program = engine.getShaderProgramCache().getNewProgram( false );
			program.createModule( vtx.getSource(), renderer::ShaderStageFlag::eVertex );
			program.createModule( pxl.getSource(), renderer::ShaderStageFlag::eFragment );
			return program;
		}

		RenderPipelineUPtr doCreatePipeline( Engine & engine
			, renderer::ShaderProgram & program )
		{
			renderer::DepthStencilState dsstate
			{
				0u,
				false,
				false
			};
			renderer::RasterisationState rsstate
			{
				0u,
				false,
				false,
				renderer::PolygonMode::eFill,
				renderer::CullModeFlag::eNone
			};
			auto bdstate = renderer::ColourBlendState::createDefault();
			return std::make_unique< RenderPipeline >( *engine.getRenderSystem()
				, std::move( dsstate )
				, std::move( rsstate )
				, std::move( bdstate )
				, renderer::MultisampleState{}
				, program
				, PipelineFlags{} );
		}

		SamplerSPtr doCreateSampler( Engine & engine
			, String const & name
			, renderer::WrapMode mode )
		{
			SamplerSPtr sampler;

			if ( engine.getSamplerCache().has( name ) )
			{
				sampler = engine.getSamplerCache().find( name );
			}
			else
			{
				sampler = engine.getSamplerCache().add( name );
				sampler->setMinFilter( renderer::Filter::eNearest );
				sampler->setMagFilter( renderer::Filter::eNearest );
				sampler->setWrapS( mode );
				sampler->setWrapT( mode );
			}

			return sampler;
		}

		TextureUnit doCreateTexture( Engine & engine, Size const & size )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto sampler = doCreateSampler( engine, cuT( "LinearisePass_Result" ), renderer::WrapMode::eClampToEdge );
			auto ssaoResult = std::make_shared< TextureLayout >( renderSystem
				, renderer::TextureType::e2D
				, renderer::ImageUsageFlag::eColourAttachment | renderer::ImageUsageFlag::eSampled
				, renderer::MemoryPropertyFlag::eDeviceLocal
				, PixelFormat::eL32F
				, size
				, LineariseDepthPass::MaxMipLevel + 1u );
			TextureUnit result{ engine };
			result.setTexture( ssaoResult );
			result.setSampler( sampler );
			result.initialise();
			return result;
		}

		renderer::RenderPassPtr doCreateRenderPass( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto & device = *renderSystem.getCurrentDevice();
			std::vector< renderer::PixelFormat > formats
			{
				PixelFormat::eL32F
			};
			renderer::RenderPassAttachmentArray attaches
			{
				renderer::RenderPassAttachment::createColourAttachment( 0u, PixelFormat::eL32F, true ),
			};
			renderer::ImageLayoutArray const initialLayouts
			{
				renderer::ImageLayout::eColourAttachmentOptimal,
			};
			renderer::ImageLayoutArray const finalLayouts
			{
				renderer::ImageLayout::eColourAttachmentOptimal,
			};
			renderer::RenderSubpassPtrArray subpasses;
			subpasses.emplace_back( device.createRenderSubpass( attaches
				, { renderer::PipelineStageFlag::eColourAttachmentOutput, renderer::AccessFlag::eColourAttachmentWrite } ) );
			return device.createRenderPass( attaches
				, std::move( subpasses )
				, renderer::RenderPassState{ renderer::PipelineStageFlag::eColourAttachmentOutput
					, renderer::AccessFlag::eColourAttachmentWrite
					, initialLayouts }
				, renderer::RenderPassState{ renderer::PipelineStageFlag::eColourAttachmentOutput
					, renderer::AccessFlag::eColourAttachmentWrite
					, finalLayouts } );
		}

		renderer::FrameBufferPtr doCreateFrameBuffer( renderer::RenderPass const & renderPass
			, TextureUnit const & texture )
		{
			renderer::FrameBufferAttachmentArray attaches;
			attaches.emplace_back( *( renderPass.begin() ), texture.getTexture()->getView() );
			auto size = texture.getTexture()->getDimensions();
			return renderPass.createFrameBuffer( renderer::UIVec2{ size.getWidth(), size.getHeight() }
				, std::move( attaches ) );
		}

		renderer::VertexBufferPtr< NonTexturedQuad > doCreateVertexBuffer( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto & device = *renderSystem.getCurrentDevice();
			auto result = renderer::makeVertexBuffer< NonTexturedQuad >( device
				, 1u
				, 0u
				, renderer::MemoryPropertyFlag::eHostVisible );

			if ( auto buffer = result->lock( 0u
				, 1u
				, renderer::MemoryMapFlag::eInvalidateRange | renderer::MemoryMapFlag::eWrite ) )
			{
				*buffer = NonTexturedQuad
				{
					{
						{ Point2f{ -1.0, -1.0 } },
						{ Point2f{ -1.0, +1.0 } },
						{ Point2f{ +1.0, -1.0 } },
						{ Point2f{ +1.0, +1.0 } },
					}
				};
				result->unlock( 1u, true );
			}

			return result;
		}

		renderer::VertexLayoutPtr doCreateVertexLayout( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto & device = *renderSystem.getCurrentDevice();
			auto result = renderer::makeLayout< NonTexturedQuad >( device
				, 0u );
			createVertexAttribute( result
				, NonTexturedQuad::Vertex
				, position
				, 0u );
			return result;
		}
	}

	//*********************************************************************************************

	LineariseDepthPass::LineariseDepthPass( Engine & engine
		, Size const & size
		, SsaoConfigUbo & ssaoConfigUbo
		, TextureUnit const & depthBuffer
		, Viewport const & viewport )
		: m_engine{ engine }
		, m_ssaoConfigUbo{ ssaoConfigUbo }
		, m_depthBuffer{ depthBuffer }
		, m_size{ size }
		, m_result{ doCreateTexture( m_engine, m_size ) }
		, m_timer{ std::make_shared< RenderPassTimer >( m_engine, cuT( "SSAO" ), cuT( "Linearise depth" ) ) }
		, m_renderPass{ doCreateRenderPass( m_engine ) }
		, m_vertexBuffer{ doCreateVertexBuffer( m_engine ) }
		, m_vertexLayout{ doCreateVertexLayout( m_engine ) }
		, m_geometryBuffers{ m_engine.getRenderSystem()->getCurrentDevice()->createGeometryBuffers( *m_vertexBuffer
			, 0u
			, *m_vertexLayout ) }
		, m_sampler{ m_engine.getRenderSystem()->getCurrentDevice()->createSampler( renderer::WrapMode::eClampToEdge
			, renderer::WrapMode::eClampToEdge
			, renderer::WrapMode::eClampToEdge
			, renderer::Filter::eNearest
			, renderer::Filter::eNearest ) }
		, m_commandBuffer{ m_engine.getRenderSystem()->getCurrentDevice()->getGraphicsCommandPool().createCommandBuffer() }
		, m_clipInfo{ renderer::ShaderStageFlag::eFragment, { { 1u, 0u, renderer::AttributeFormat::eVec3f } } }
		, m_linearisePushConstants{ renderer::ShaderStageFlag::eFragment, 0u, renderer::getSize( renderer::AttributeFormat::eVec3f ) }
		, m_lineariseProgram{ doGetLineariseProgram( m_engine ) }
		, m_minifyProgram{ doGetMinifyProgram( m_engine ) }
	{
		auto z_f = viewport.getFar();
		auto z_n = viewport.getNear();
		auto clipInfo = std::isinf( z_f )
			? Point3f{ z_n, -1.0f, 1.0f }
			: Point3f{ z_n * z_f, z_n - z_f, z_f };
		// result = clipInfo[0] / ( clipInfo[1] * depth + clipInfo[2] );
		// depth = 0 => result = z_n
		// depth = 1 => result = z_f
		*m_clipInfo.getData() = clipInfo;

		doInitialiseLinearisePass();
		doInitialiseMinifyPass();
		doPrepareFrame();
	}

	LineariseDepthPass::~LineariseDepthPass()
	{
		doCleanupMinifyPass();
		doCleanupLinearisePass();
		m_renderPass.reset();
		m_timer.reset();
		m_result.cleanup();
	}

	void LineariseDepthPass::linearise()
	{
		m_timer->start();
		auto & renderSystem = *m_engine.getRenderSystem();
		auto & device = *renderSystem.getCurrentDevice();
		device.getGraphicsQueue().submit( *m_commandBuffer, nullptr );
		m_timer->stop();
	}

	void LineariseDepthPass::doInitialiseLinearisePass()
	{
		auto size = m_result.getTexture()->getDimensions();
		renderer::FrameBufferAttachmentArray attaches;
		attaches.emplace_back( *( m_renderPass->begin() ), m_result.getTexture()->getView() );
		m_lineariseFrameBuffer = m_renderPass->createFrameBuffer( renderer::UIVec2{ size[0], size[1] }
			, std::move( attaches ) );
		auto & renderSystem = *m_engine.getRenderSystem();
		auto & device = *renderSystem.getCurrentDevice();
		renderer::DescriptorSetLayoutBindingArray bindings
		{
			{ 0u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment }
		};
		m_lineariseDescriptorLayout = device.createDescriptorSetLayout( std::move( bindings ) );
		m_lineariseDescriptorPool = m_lineariseDescriptorLayout->createPool( 1u );
		m_lineariseDescriptor = m_lineariseDescriptorPool->createDescriptorSet();
		m_lineariseDescriptor->createBinding( m_lineariseDescriptorLayout->getBinding( 0u )
			, m_result.getTexture()->getView()
			, *m_sampler );
		m_linearisePipeline = doCreatePipeline( m_engine, m_lineariseProgram );
		m_linearisePipeline->setDescriptorSetLayouts( { *m_lineariseDescriptorLayout } );
		m_linearisePipeline->setPushConstantRanges( { m_linearisePushConstants } );
		m_linearisePipeline->setVertexLayouts( { *m_vertexLayout } );
		m_linearisePipeline->setViewport( { size[0], size[1], 0, 0 } );
		m_linearisePipeline->setScissor( { 0, 0, size[0], size[1] } );
		m_linearisePipeline->initialise( *m_renderPass, renderer::PrimitiveTopology::eTriangleStrip );
	}

	void LineariseDepthPass::doInitialiseMinifyPass()
	{
		auto & renderSystem = *m_engine.getRenderSystem();
		auto & device = *renderSystem.getCurrentDevice();
		auto size = m_result.getTexture()->getDimensions();
		renderer::DescriptorSetLayoutBindingArray bindings
		{
			{ 0u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment }
		};
		m_minifyDescriptorLayout = device.createDescriptorSetLayout( std::move( bindings ) );
		m_minifyDescriptorPool = m_minifyDescriptorLayout->createPool( MaxMipLevel );
		uint32_t index = 1u;

		for ( auto & pipeline : m_minifyPipelines )
		{
			pipeline.view = m_result.getTexture()->getTexture().createView( renderer::TextureType::e2D
				, m_result.getTexture()->getPixelFormat()
				, index );
			pipeline.descriptor = m_minifyDescriptorPool->createDescriptorSet();
			pipeline.descriptor->createBinding( m_minifyDescriptorLayout->getBinding( 0u )
				, *pipeline.view
				, *m_sampler );
			renderer::FrameBufferAttachmentArray attaches;
			attaches.emplace_back( *( m_renderPass->begin() ), m_result.getTexture()->getView() );
			pipeline.frameBuffer = m_renderPass->createFrameBuffer( renderer::UIVec2{ size[0], size[1] }
				, std::move( attaches ) );
			pipeline.previousLevel = std::make_unique< renderer::PushConstantsBuffer< int > >( renderer::ShaderStageFlag::eFragment
				, renderer::PushConstantArray{ { 1u, 0u, renderer::AttributeFormat::eInt } } );
			pipeline.pushConstants = { renderer::ShaderStageFlag::eFragment, 0u, renderer::getSize( renderer::AttributeFormat::eInt ) };
			pipeline.pipeline = doCreatePipeline( m_engine, m_minifyProgram );
			pipeline.pipeline->setDescriptorSetLayouts( { *m_minifyDescriptorLayout } );
			pipeline.pipeline->setPushConstantRanges( { pipeline.pushConstants } );
			pipeline.pipeline->setVertexLayouts( { *m_vertexLayout } );
			pipeline.pipeline->setViewport( { size[0], size[1], 0, 0 } );
			pipeline.pipeline->setScissor( { 0, 0, size[0], size[1] } );
			pipeline.pipeline->initialise( *m_renderPass, renderer::PrimitiveTopology::eTriangleStrip );
			++index;
		}
	}

	void LineariseDepthPass::doCleanupLinearisePass()
	{
		m_linearisePipeline->cleanup();
		m_linearisePipeline.reset();
		m_lineariseDescriptor.reset();
		m_lineariseDescriptorPool.reset();
		m_lineariseDescriptorLayout.reset();
		m_lineariseFrameBuffer.reset();
	}

	void LineariseDepthPass::doCleanupMinifyPass()
	{
		for ( auto & pipeline : m_minifyPipelines )
		{
			pipeline.pipeline->cleanup();
			pipeline.pipeline.reset();
			pipeline.previousLevel.reset();
			pipeline.frameBuffer.reset();
			pipeline.descriptor.reset();
			pipeline.view.reset();
		}

		m_minifyDescriptorPool.reset();
		m_minifyDescriptorLayout.reset();
	}

	void LineariseDepthPass::doPrepareFrame()
	{
		static RgbaColour const colour{ 0.0, 0.0, 0.0, 0.0 };

		if ( m_commandBuffer->begin( renderer::CommandBufferUsageFlag::eSimultaneousUse ) )
		{
			m_commandBuffer->resetQueryPool( m_timer->getQuery(), 0u, 2u );
			m_commandBuffer->writeTimestamp( renderer::PipelineStageFlag::eTopOfPipe
				, m_timer->getQuery()
				, 0u );

			// Linearisation pass.
			m_commandBuffer->beginRenderPass( *m_renderPass
				, *m_lineariseFrameBuffer
				, { colour }
				, renderer::SubpassContents::eInline );
			m_commandBuffer->bindPipeline( m_linearisePipeline->getPipeline() );
			m_commandBuffer->bindDescriptorSet( *m_lineariseDescriptor
				, m_linearisePipeline->getPipelineLayout() );
			m_commandBuffer->pushConstants( m_linearisePipeline->getPipelineLayout()
				, m_clipInfo );
			m_commandBuffer->bindGeometryBuffers( *m_geometryBuffers );
			m_commandBuffer->draw( 4u );
			m_commandBuffer->endRenderPass();

			// Minification passes.
			for ( auto & pipeline : m_minifyPipelines )
			{
				m_commandBuffer->beginRenderPass( *m_renderPass
					, *pipeline.frameBuffer
					, { colour }
					, renderer::SubpassContents::eInline );
				m_commandBuffer->bindPipeline( pipeline.pipeline->getPipeline() );
				m_commandBuffer->bindDescriptorSet( *pipeline.descriptor
					, pipeline.pipeline->getPipelineLayout() );
				m_commandBuffer->pushConstants( pipeline.pipeline->getPipelineLayout()
					, *pipeline.previousLevel );
				m_commandBuffer->bindGeometryBuffers( *m_geometryBuffers );
				m_commandBuffer->draw( 4u );
				m_commandBuffer->endRenderPass();
			}

			m_commandBuffer->writeTimestamp( renderer::PipelineStageFlag::eBottomOfPipe
				, m_timer->getQuery()
				, 1u );
			m_commandBuffer->end();
		}
	}
}
