#include "FinalCombinePass.hpp"

#include <Engine.hpp>
#include <Render/RenderPassTimer.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Scene/Camera.hpp>
#include <Scene/Fog.hpp>
#include <Texture/Sampler.hpp>
#include <Texture/TextureLayout.hpp>
#include <Texture/TextureUnit.hpp>

#include <Buffer/VertexBuffer.hpp>
#include <Command/CommandBufferInheritanceInfo.hpp>
#include <Pipeline/ColourBlendState.hpp>
#include <Pipeline/DepthStencilState.hpp>
#include <Pipeline/MultisampleState.hpp>
#include <Pipeline/RasterisationState.hpp>
#include <RenderPass/FrameBuffer.hpp>
#include <RenderPass/FrameBufferAttachment.hpp>
#include <Shader/ShaderProgram.hpp>

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

#include "Shader/Shaders/GlslFog.hpp"
#include "Shader/Shaders/GlslLight.hpp"
#include "Shader/Shaders/GlslShadow.hpp"

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		SamplerSPtr doCreateSampler( Engine & engine )
		{
			String const name = cuT( "FinalCombinePass" );
			SamplerSPtr result;

			if ( engine.getSamplerCache().has( name ) )
			{
				result = engine.getSamplerCache().find( name );
			}
			else
			{
				result = engine.getSamplerCache().add( name );
				result->initialise();
			}

			return result;
		}

		renderer::VertexLayoutPtr doCreateVertexLayout( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto & device = getCurrentDevice( renderSystem );
			renderer::VertexLayoutPtr result = renderer::makeLayout< TexturedQuad::Vertex >( 0u );
			result->createAttribute( 0u, renderer::Format::eR32G32_SFLOAT, offsetof( TexturedQuad::Vertex, position ) );
			result->createAttribute( 1u, renderer::Format::eR32G32_SFLOAT, offsetof( TexturedQuad::Vertex, texture ) );
			return result;
		}

		renderer::VertexBufferPtr< TexturedQuad > doCreateVbo( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto & device = getCurrentDevice( renderSystem );
			auto result = renderer::makeVertexBuffer< TexturedQuad >( device
				, 1u
				, renderer::BufferTarget::eTransferDst
				, renderer::MemoryPropertyFlag::eHostVisible );

			if ( auto buffer = result->lock( 0u
				, 1u
				, renderer::MemoryMapFlag::eWrite | renderer::MemoryMapFlag::eInvalidateRange ) )
			{
				*buffer = TexturedQuad
				{
					{
						{ Point2f{ -1.0, -1.0 }, Point2f{ 0.0, renderSystem.isTopDown() ? 0.0 : 1.0 } },
						{ Point2f{ -1.0, +1.0 }, Point2f{ 0.0, renderSystem.isTopDown() ? 1.0 : 0.0 } },
						{ Point2f{ +1.0, -1.0 }, Point2f{ 1.0, renderSystem.isTopDown() ? 0.0 : 1.0 } },
						{ Point2f{ +1.0, -1.0 }, Point2f{ 1.0, renderSystem.isTopDown() ? 0.0 : 1.0 } },
						{ Point2f{ -1.0, +1.0 }, Point2f{ 0.0, renderSystem.isTopDown() ? 1.0 : 0.0 } },
						{ Point2f{ +1.0, +1.0 }, Point2f{ 1.0, renderSystem.isTopDown() ? 1.0 : 0.0 } },
					}
				};
				result->flush( 0u, 1u );
				result->unlock();
			}

			return result;
		}

		renderer::DescriptorSetLayoutPtr doCreateUboDescriptorLayout( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto & device = getCurrentDevice( renderSystem );
			renderer::DescriptorSetLayoutBindingArray bindings
			{
				{ 0u, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eFragment },
				{ 1u, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eFragment },
			};
			return device.createDescriptorSetLayout( std::move( bindings ) );
		}

		renderer::DescriptorSetPtr doCreateUboDescriptorSet( renderer::DescriptorSetPool & pool
			, SceneUbo const & sceneUbo
			, GpInfoUbo const & gpInfoUbo )
		{
			auto & layout = pool.getLayout();
			auto result = pool.createDescriptorSet( 0u );
			result->createBinding( layout.getBinding( 0u )
				, sceneUbo.getUbo()
				, 0u
				, 1u );
			result->createBinding( layout.getBinding( 1u )
				, gpInfoUbo.getUbo()
				, 0u
				, 1u );
			result->update();
			return result;
		}

		renderer::DescriptorSetLayoutPtr doCreateTexDescriptorLayout( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto & device = getCurrentDevice( renderSystem );
			renderer::DescriptorSetLayoutBindingArray bindings
			{
				{ 0u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment },
				{ 1u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment },
				{ 2u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment },
			};
			return device.createDescriptorSetLayout( std::move( bindings ) );
		}

		renderer::DescriptorSetPtr doCreateTexDescriptorSet( renderer::DescriptorSetPool & pool
			, renderer::TextureViewCRef const & depth
			, renderer::TextureViewCRef const & accumulation
			, renderer::TextureViewCRef const & revealage
			, Sampler const & sampler )
		{
			auto & layout = pool.getLayout();
			auto result = pool.createDescriptorSet( 1u );
			result->createBinding( layout.getBinding( 0u )
				, depth.get()
				, sampler.getSampler() );
			result->createBinding( layout.getBinding( 1u )
				, accumulation.get()
				, sampler.getSampler() );
			result->createBinding( layout.getBinding( 2u )
				, revealage.get()
				, sampler.getSampler() );
			result->update();
			return result;
		}

		glsl::Shader doGetVertexProgram( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			auto position = writer.declAttribute< Vec2 >( cuT( "position" ), 0u );
			auto texture = writer.declAttribute< Vec2 >( cuT( "texcoord" ), 1u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ), 0u );
			auto out = gl_PerVertex{ writer };

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
				{
					vtx_texture = texture;
					out.gl_Position() = vec4( position, 0.0, 1.0 );
				} );
			return writer.finalise();
		}
		
		glsl::Shader doGetPixelProgram( Engine & engine
			, FogType fogType )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			UBO_SCENE( writer, 0u, 0u );
			UBO_GPINFO( writer, 1u, 0u );
			auto c3d_mapDepth = writer.declSampler< Sampler2D >( getTextureName( WbTexture::eDepth ), 0u, 1u );
			auto c3d_mapAccumulation = writer.declSampler< Sampler2D >( getTextureName( WbTexture::eAccumulation ), 1u, 1u );
			auto c3d_mapRevealage = writer.declSampler< Sampler2D >( getTextureName( WbTexture::eRevealage ), 2u, 1u );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ), 0u );
			auto gl_FragCoord = writer.declBuiltin< Vec4 >( cuT( "gl_FragCoord" ) );

			// Shader outputs
			auto pxl_fragColor = writer.declFragData< Vec4 >( cuT( "pxl_fragColor" ), 0u );

			glsl::Utils utils{ writer };
			utils.declareCalcVSPosition();

			shader::Fog fog{ fogType, writer };

			auto maxComponent = writer.implementFunction< Float >( cuT( "maxComponent" )
				, [&]( Vec3 const & v )
				{
					writer.returnStmt( max( max( v.x(), v.y() ), v.z() ) );
				}
				, InVec3{ &writer, cuT( "v" ) } );

			writer.implementFunction< Void >( cuT( "main" )
				, [&]()
				{
					auto coord = writer.declLocale( cuT( "coord" )
						, ivec2( gl_FragCoord.xy() ) );
					auto revealage = writer.declLocale( cuT( "revealage" )
						, texelFetch( c3d_mapRevealage, coord, 0 ).r() );

					IF( writer, revealage == 1.0_f )
					{
						// Save the blending and color texture fetch cost
						writer.discard();
					}
					FI;

					auto accum = writer.declLocale( cuT( "accum" )
						, texelFetch( c3d_mapAccumulation, coord, 0 ) );

					// Suppress overflow
					IF( writer, glsl::isinf( maxComponent( glsl::abs( accum.rgb() ) ) ) )
					{
						accum.rgb() = vec3( accum.a() );
					}
					FI;

					auto averageColor = writer.declLocale( cuT( "averageColor" )
						, accum.rgb() / max( accum.a(), 0.00001_f ) );

					pxl_fragColor = vec4( averageColor.rgb(), 1.0_f - revealage );

					if ( fogType != FogType::eDisabled )
					{
						auto texCoord = writer.declLocale( cuT( "texCoord" )
							, ( writer.isTopDown()
								? vtx_texture
								: vec2( vtx_texture.x(), 1.0 - vtx_texture.y() ) ) );
						auto position = writer.declLocale( cuT( "position" )
							, utils.calcVSPosition( vtx_texture
								, texture( c3d_mapDepth, texCoord ).r()
								, c3d_mtxInvProj ) );
						fog.applyFog( pxl_fragColor, length( position ), position.z() );
					}
				} );
			return writer.finalise();
		}
		
		renderer::ShaderStageStateArray doCreateProgram( Engine & engine
			, FogType fogType
			, glsl::Shader & vertexShader
			, glsl::Shader & pixelShader )
		{
			auto & device = getCurrentDevice( engine );
			vertexShader = doGetVertexProgram( engine );
			pixelShader = doGetPixelProgram( engine, fogType );
			renderer::ShaderStageStateArray program
			{
				{ device.createShaderModule( renderer::ShaderStageFlag::eVertex ) },
				{ device.createShaderModule( renderer::ShaderStageFlag::eFragment ) },
			};
			program[0].module->loadShader( vertexShader.getSource() );
			program[1].module->loadShader( pixelShader.getSource() );
			return program;
		}

		renderer::PipelineLayoutPtr doCreateRenderPipelineLayout( Engine & engine
			, renderer::DescriptorSetLayout const & uboLayout
			, renderer::DescriptorSetLayout const & texLayout )
		{
			return getCurrentDevice( engine ).createPipelineLayout( { uboLayout, texLayout } );
		}

		renderer::PipelinePtr doCreateRenderPipeline( renderer::PipelineLayout const & pipelineLayout
			, renderer::ShaderStageStateArray const & program
			, renderer::RenderPass const & renderPass
			, renderer::VertexLayout const & vtxLayout )
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
			renderer::ColourBlendState bdState;
			bdState.attachs.push_back( {
				true,
				renderer::BlendFactor::eSrcAlpha,
				renderer::BlendFactor::eInvSrcAlpha,
				renderer::BlendOp::eAdd,
				renderer::BlendFactor::eSrcAlpha,
				renderer::BlendFactor::eInvSrcAlpha,
				renderer::BlendOp::eAdd
			} );
			return pipelineLayout.createPipeline( {
				program,
				renderPass,
				renderer::VertexInputState::create( vtxLayout ),
				renderer::InputAssemblyState{ renderer::PrimitiveTopology::eTriangleStrip },
				rsstate,
				renderer::MultisampleState{},
				bdState,
				{ renderer::DynamicState::eViewport, renderer::DynamicState::eScissor },
				dsstate,
			} );
		}

		renderer::RenderPassPtr doCreateRenderPass( Engine & engine
			, renderer::TextureView const & colourView )
		{
			renderer::RenderPassCreateInfo renderPass{};
			renderPass.flags = 0u;

			renderPass.attachments.resize( 1u );
			renderPass.attachments[0].format = colourView.getFormat();
			renderPass.attachments[0].samples = renderer::SampleCountFlag::e1;
			renderPass.attachments[0].loadOp = renderer::AttachmentLoadOp::eLoad;
			renderPass.attachments[0].storeOp = renderer::AttachmentStoreOp::eStore;
			renderPass.attachments[0].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
			renderPass.attachments[0].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
			renderPass.attachments[0].initialLayout = renderer::ImageLayout::eColourAttachmentOptimal;
			renderPass.attachments[0].finalLayout = renderer::ImageLayout::eColourAttachmentOptimal;

			renderer::AttachmentReference colourReference;
			colourReference.attachment = 0u;
			colourReference.layout = renderer::ImageLayout::eColourAttachmentOptimal;

			renderPass.subpasses.resize( 1u );
			renderPass.subpasses[0].flags = 0u;
			renderPass.subpasses[0].colorAttachments = { { 0u, renderer::ImageLayout::eColourAttachmentOptimal } };

			renderPass.dependencies.resize( 2u );
			renderPass.dependencies[0].srcSubpass = renderer::ExternalSubpass;
			renderPass.dependencies[0].dstSubpass = 0u;
			renderPass.dependencies[0].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
			renderPass.dependencies[0].dstStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
			renderPass.dependencies[0].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
			renderPass.dependencies[0].dstAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
			renderPass.dependencies[0].dependencyFlags = renderer::DependencyFlag::eByRegion;

			renderPass.dependencies[1].srcSubpass = 0u;
			renderPass.dependencies[1].dstSubpass = renderer::ExternalSubpass;
			renderPass.dependencies[1].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
			renderPass.dependencies[1].dstStageMask = renderer::PipelineStageFlag::eFragmentShader;
			renderPass.dependencies[1].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
			renderPass.dependencies[1].dstAccessMask = renderer::AccessFlag::eShaderRead;
			renderPass.dependencies[1].dependencyFlags = renderer::DependencyFlag::eByRegion;

			return getCurrentDevice( engine ).createRenderPass( renderPass );
		}

		renderer::FrameBufferPtr doCreateFrameBuffer( renderer::RenderPass const & renderPass
			, Size const & size
			, renderer::TextureView const & colourView )
		{
			renderer::FrameBufferAttachmentArray attaches
			{
				{ *( renderPass.getAttachments().begin() + 0u ), colourView },
			};
			return renderPass.createFrameBuffer( renderer::Extent2D{ size.getWidth(), size.getHeight() }
			, std::move( attaches ) );
		}
	}

	//*********************************************************************************************

	FinalCombineProgram::FinalCombineProgram( Engine & engine
		, renderer::RenderPass const & renderPass
		, RenderPassTimer & timer
		, renderer::DescriptorSetLayout const & uboLayout
		, renderer::DescriptorSetLayout const & texLayout
		, renderer::VertexLayout const & vtxLayout
		, FogType fogType )
		: m_timer{ timer }
		, m_renderPass{ renderPass }
		, m_pipelineLayout{ doCreateRenderPipelineLayout( engine
			, uboLayout
			, texLayout ) }
		, m_pipeline{ doCreateRenderPipeline( *m_pipelineLayout
			, doCreateProgram( engine, fogType, m_vertexShader, m_pixelShader )
			, renderPass
			, vtxLayout ) }
		, m_commandBuffer{ getCurrentDevice( engine ).getGraphicsCommandPool().createCommandBuffer( true ) }
	{
	}

	FinalCombineProgram::~FinalCombineProgram()
	{
		m_pipeline.reset();
	}

	void FinalCombineProgram::prepare( renderer::FrameBuffer const & frameBuffer
		, renderer::DescriptorSet const & uboDescriptorSet
		, renderer::DescriptorSet const & texDescriptorSet
		, renderer::BufferBase const & vbo )
	{
		if ( m_commandBuffer->begin() )
		{
			m_timer.beginPass( *m_commandBuffer );
			m_commandBuffer->beginRenderPass( m_renderPass
				, frameBuffer
				, { renderer::ClearColorValue{} }
				, renderer::SubpassContents::eInline );
			m_commandBuffer->bindPipeline( *m_pipeline );
			m_commandBuffer->setViewport( { renderer::Offset2D{}, frameBuffer.getDimensions() } );
			m_commandBuffer->setScissor( { renderer::Offset2D{}, frameBuffer.getDimensions() } );
			m_commandBuffer->bindDescriptorSets( { uboDescriptorSet, texDescriptorSet }, *m_pipelineLayout );
			m_commandBuffer->bindVertexBuffer( 0u, vbo, 0u );
			m_commandBuffer->draw( 6u );
			m_commandBuffer->endRenderPass();
			m_timer.endPass( *m_commandBuffer );
			m_commandBuffer->end();
		}
	}

	void FinalCombineProgram::accept( RenderTechniqueVisitor & visitor )
	{
		visitor.visit( cuT( "Combine" )
			, renderer::ShaderStageFlag::eVertex
			, m_vertexShader );
		visitor.visit( cuT( "Combine" )
			, renderer::ShaderStageFlag::eFragment
			, m_pixelShader );
	}

	//*********************************************************************************************

	FinalCombinePass::FinalCombinePass( Engine & engine
		, Size const & size
		, SceneUbo & sceneUbo
		, WeightedBlendTextures const & wbResult
		, renderer::TextureView const & colourView )
		: m_size{ size }
		, m_engine{ engine }
		, m_sceneUbo{ sceneUbo }
		, m_gpInfo{ m_engine }
		, m_sampler{ doCreateSampler( m_engine ) }
		, m_vertexBuffer{ doCreateVbo( m_engine ) }
		, m_vertexLayout{ doCreateVertexLayout( m_engine ) }
		, m_uboDescriptorLayout{ doCreateUboDescriptorLayout( m_engine ) }
		, m_uboDescriptorPool{ m_uboDescriptorLayout->createPool( uint32_t( FogType::eCount ) ) }
		, m_uboDescriptorSet{ doCreateUboDescriptorSet( *m_uboDescriptorPool, m_sceneUbo, m_gpInfo ) }
		, m_texDescriptorLayout{ doCreateTexDescriptorLayout( m_engine ) }
		, m_texDescriptorPool{ m_texDescriptorLayout->createPool( uint32_t( FogType::eCount ) ) }
		, m_texDescriptorSet{ doCreateTexDescriptorSet( *m_texDescriptorPool, wbResult[0], wbResult[1], wbResult[2], *m_sampler ) }
		, m_renderPass{ doCreateRenderPass( m_engine, colourView ) }
		, m_timer{ std::make_shared< RenderPassTimer >( m_engine, cuT( "Transparent" ), cuT( "Resolve" ) ) }
		, m_programs
		{
			FinalCombineProgram{ m_engine, *m_renderPass, *m_timer, *m_uboDescriptorLayout, *m_texDescriptorLayout, *m_vertexLayout, FogType::eDisabled },
			FinalCombineProgram{ m_engine, *m_renderPass, *m_timer, *m_uboDescriptorLayout, *m_texDescriptorLayout, *m_vertexLayout, FogType::eLinear },
			FinalCombineProgram{ m_engine, *m_renderPass, *m_timer, *m_uboDescriptorLayout, *m_texDescriptorLayout, *m_vertexLayout, FogType::eExponential },
			FinalCombineProgram{ m_engine, *m_renderPass, *m_timer, *m_uboDescriptorLayout, *m_texDescriptorLayout, *m_vertexLayout, FogType::eSquaredExponential }
		}
		, m_frameBuffer{ doCreateFrameBuffer( *m_renderPass, m_size, colourView ) }
		, m_semaphore{ getCurrentDevice( m_engine ).createSemaphore() }
	{
		for ( auto & program : m_programs )
		{
			program.prepare( *m_frameBuffer
				, *m_uboDescriptorSet
				, *m_texDescriptorSet
				, m_vertexBuffer->getBuffer() );
		}
	}

	FinalCombinePass::~FinalCombinePass()
	{
		m_gpInfo.cleanup();
	}

	void FinalCombinePass::update( Camera const & camera
		, Matrix4x4r const & invViewProj
		, Matrix4x4r const & invView
		, Matrix4x4r const & invProj )
	{
		m_gpInfo.update( m_size
			, camera
			, invViewProj
			, invView
			, invProj );
	}

	renderer::Semaphore const & FinalCombinePass::render( FogType fogType
		, renderer::Semaphore const & toWait )
	{
		auto & program = m_programs[size_t( fogType )];
		m_timer->start();
		m_timer->notifyPassRender();
		getCurrentDevice( m_engine ).getGraphicsQueue().submit( program.getCommandBuffer()
			, toWait
			, renderer::PipelineStageFlag::eColourAttachmentOutput
			, *m_semaphore
			, nullptr );
		m_timer->stop();
		return *m_semaphore;
	}

	void FinalCombinePass::accept( RenderTechniqueVisitor & visitor )
	{
		m_programs[size_t( getFogType( visitor.getSceneFlags() ) )].accept( visitor );
	}

	//*********************************************************************************************
}
