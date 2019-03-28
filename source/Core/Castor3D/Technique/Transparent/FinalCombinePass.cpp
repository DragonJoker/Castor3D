#include "Castor3D/Technique/Transparent/FinalCombinePass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Fog.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Texture/Sampler.hpp"
#include "Castor3D/Texture/TextureLayout.hpp"
#include "Castor3D/Texture/TextureUnit.hpp"

#include <Ashes/Buffer/VertexBuffer.hpp>
#include <Ashes/Command/CommandBufferInheritanceInfo.hpp>
#include <Ashes/Pipeline/ColourBlendState.hpp>
#include <Ashes/Pipeline/DepthStencilState.hpp>
#include <Ashes/Pipeline/MultisampleState.hpp>
#include <Ashes/Pipeline/RasterisationState.hpp>
#include <Ashes/RenderPass/FrameBuffer.hpp>
#include <Ashes/RenderPass/FrameBufferAttachment.hpp>
#include <Ashes/RenderPass/RenderPassCreateInfo.hpp>
#include <Ashes/RenderPass/SubpassDependency.hpp>
#include "Castor3D/Shader/GlslToSpv.hpp"

#include <ShaderWriter/Source.hpp>
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include "Castor3D/Shader/Shaders/GlslFog.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslShadow.hpp"

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

		ashes::VertexLayoutPtr doCreateVertexLayout( Engine & engine )
		{
			ashes::VertexLayoutPtr result = ashes::makeLayout< TexturedQuad::Vertex >( 0u );
			result->createAttribute( 0u, ashes::Format::eR32G32_SFLOAT, offsetof( TexturedQuad::Vertex, position ) );
			result->createAttribute( 1u, ashes::Format::eR32G32_SFLOAT, offsetof( TexturedQuad::Vertex, texture ) );
			return result;
		}

		ashes::VertexBufferPtr< TexturedQuad > doCreateVbo( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto & device = getCurrentDevice( renderSystem );
			auto result = ashes::makeVertexBuffer< TexturedQuad >( device
				, 1u
				, ashes::BufferTarget::eTransferDst
				, ashes::MemoryPropertyFlag::eHostVisible );
			device.debugMarkerSetObjectName(
				{
					ashes::DebugReportObjectType::eBuffer,
					&result->getBuffer(),
					"FinalCombinePassVbo"
				} );

			if ( auto buffer = reinterpret_cast< TexturedQuad * >( result->getBuffer().lock( 0u
				, ~( 0ull )
				, ashes::MemoryMapFlag::eWrite | ashes::MemoryMapFlag::eInvalidateRange ) ) )
			{
				*buffer = TexturedQuad
				{
					{
						{ Point2f{ -1.0, -1.0 }, Point2f{ 0.0, 0.0 } },
						{ Point2f{ -1.0, +1.0 }, Point2f{ 0.0, 1.0 } },
						{ Point2f{ +1.0, -1.0 }, Point2f{ 1.0, 0.0 } },
						{ Point2f{ +1.0, -1.0 }, Point2f{ 1.0, 0.0 } },
						{ Point2f{ -1.0, +1.0 }, Point2f{ 0.0, 1.0 } },
						{ Point2f{ +1.0, +1.0 }, Point2f{ 1.0, 1.0 } },
					}
				};
				result->getBuffer().flush( 0u, ~( 0ull ) );
				result->getBuffer().unlock();
			}

			return result;
		}

		ashes::DescriptorSetLayoutPtr doCreateUboDescriptorLayout( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto & device = getCurrentDevice( renderSystem );
			ashes::DescriptorSetLayoutBindingArray bindings
			{
				{ 0u, ashes::DescriptorType::eUniformBuffer, ashes::ShaderStageFlag::eFragment },
				{ 1u, ashes::DescriptorType::eUniformBuffer, ashes::ShaderStageFlag::eFragment },
				{ 2u, ashes::DescriptorType::eUniformBuffer, ashes::ShaderStageFlag::eFragment },
			};
			return device.createDescriptorSetLayout( std::move( bindings ) );
		}

		ashes::DescriptorSetPtr doCreateUboDescriptorSet( ashes::DescriptorSetPool & pool
			, SceneUbo const & sceneUbo
			, GpInfoUbo const & gpInfoUbo
			, HdrConfigUbo const & hdrConfigUbo )
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
			result->createBinding( layout.getBinding( 2u )
				, hdrConfigUbo.getUbo()
				, 0u
				, 1u );
			result->update();
			return result;
		}

		ashes::DescriptorSetLayoutPtr doCreateTexDescriptorLayout( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto & device = getCurrentDevice( renderSystem );
			ashes::DescriptorSetLayoutBindingArray bindings
			{
				{ 0u, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment },
				{ 1u, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment },
				{ 2u, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment },
			};
			return device.createDescriptorSetLayout( std::move( bindings ) );
		}

		ashes::DescriptorSetPtr doCreateTexDescriptorSet( ashes::DescriptorSetPool & pool
			, ashes::TextureViewCRef const & depth
			, ashes::TextureViewCRef const & accumulation
			, ashes::TextureViewCRef const & revealage
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

		ShaderPtr doGetVertexProgram( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace sdw;
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
					out.gl_out.gl_Position = vec4( position, 0.0, 1.0 );
				} );
			return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}
		
		ShaderPtr doGetPixelProgram( Engine & engine
			, FogType fogType )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			UBO_SCENE( writer, 0u, 0u );
			UBO_GPINFO( writer, 1u, 0u );
			UBO_HDR_CONFIG( writer, 2u, 0u );
			auto c3d_mapDepth = writer.declSampledImage< FImg2DRgba32 >( getTextureName( WbTexture::eDepth ), 0u, 1u );
			auto c3d_mapAccumulation = writer.declSampledImage< FImg2DRgba32 >( getTextureName( WbTexture::eAccumulation ), 1u, 1u );
			auto c3d_mapRevealage = writer.declSampledImage< FImg2DRgba32 >( getTextureName( WbTexture::eRevealage ), 2u, 1u );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ), 0u );
			auto in = writer.getIn();

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( cuT( "pxl_fragColor" ), 0u );

			shader::Utils utils{ writer };
			utils.declareInvertVec2Y();
			utils.declareRemoveGamma();
			utils.declareCalcVSPosition();

			shader::Fog fog{ fogType, writer };

			auto maxComponent = writer.implementFunction< Float >( cuT( "maxComponent" )
				, [&]( Vec3 const & v )
				{
					writer.returnStmt( max( max( v.x(), v.y() ), v.z() ) );
				}
				, InVec3{ writer, cuT( "v" ) } );

			writer.implementFunction< sdw::Void >( cuT( "main" )
				, [&]()
				{
					auto coord = writer.declLocale( cuT( "coord" )
						, ivec2( in.gl_FragCoord.xy() ) );
					auto revealage = writer.declLocale( cuT( "revealage" )
						, texelFetch( c3d_mapRevealage, coord, 0_i ).r() );

					IF( writer, revealage == 1.0_f )
					{
						// Save the blending and color texture fetch cost
						writer.discard();
					}
					FI;

					auto accum = writer.declLocale( cuT( "accum" )
						, texelFetch( c3d_mapAccumulation, coord, 0_i ) );

					// Suppress overflow
					IF( writer, sdw::isinf( maxComponent( sdw::abs( accum.rgb() ) ) ) )
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
							, utils.bottomUpToTopDown( in.gl_FragCoord.xy() ) );
						auto position = writer.declLocale( cuT( "position" )
							, utils.calcVSPosition( texCoord
								, texture( c3d_mapDepth, texCoord ).r()
								, c3d_mtxInvProj ) );
						pxl_fragColor = fog.apply( vec4( utils.removeGamma( c3d_gamma, c3d_backgroundColour.rgb() ), c3d_backgroundColour.a() )
							, pxl_fragColor
							, length( position )
							, position.z() );
					}
				} );
			return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}
		
		ashes::ShaderStageStateArray doCreateProgram( Engine & engine
			, FogType fogType
			, ShaderModule & vertexShader
			, ShaderModule & pixelShader )
		{
			auto & device = getCurrentDevice( engine );
			vertexShader.shader = doGetVertexProgram( engine );
			pixelShader.shader = doGetPixelProgram( engine, fogType );
			ashes::ShaderStageStateArray program
			{
				{ device.createShaderModule( ashes::ShaderStageFlag::eVertex ) },
				{ device.createShaderModule( ashes::ShaderStageFlag::eFragment ) },
			};
			program[0].module->loadShader( engine.getRenderSystem()->compileShader( vertexShader ) );
			program[1].module->loadShader( engine.getRenderSystem()->compileShader( pixelShader ) );
			return program;
		}

		ashes::PipelineLayoutPtr doCreateRenderPipelineLayout( Engine & engine
			, ashes::DescriptorSetLayout const & uboLayout
			, ashes::DescriptorSetLayout const & texLayout )
		{
			return getCurrentDevice( engine ).createPipelineLayout( { uboLayout, texLayout } );
		}

		ashes::PipelinePtr doCreateRenderPipeline( ashes::PipelineLayout const & pipelineLayout
			, ashes::ShaderStageStateArray const & program
			, ashes::RenderPass const & renderPass
			, ashes::VertexLayout const & vtxLayout )
		{
			ashes::DepthStencilState dsstate
			{
				0u,
				false,
				false
			};
			ashes::RasterisationState rsstate
			{
				0u,
				false,
				false,
				ashes::PolygonMode::eFill,
				ashes::CullModeFlag::eNone
			};
			ashes::ColourBlendState bdState;
			bdState.attachs.push_back( {
				true,
				ashes::BlendFactor::eSrcAlpha,
				ashes::BlendFactor::eInvSrcAlpha,
				ashes::BlendOp::eAdd,
				ashes::BlendFactor::eSrcAlpha,
				ashes::BlendFactor::eInvSrcAlpha,
				ashes::BlendOp::eAdd
			} );
			return pipelineLayout.createPipeline( {
				program,
				renderPass,
				ashes::VertexInputState::create( vtxLayout ),
				ashes::InputAssemblyState{ ashes::PrimitiveTopology::eTriangleStrip },
				rsstate,
				ashes::MultisampleState{},
				bdState,
				{ ashes::DynamicStateEnable::eViewport, ashes::DynamicStateEnable::eScissor },
				dsstate,
			} );
		}

		ashes::RenderPassPtr doCreateRenderPass( Engine & engine
			, ashes::TextureView const & colourView )
		{
			ashes::RenderPassCreateInfo renderPass{};
			renderPass.flags = 0u;

			renderPass.attachments.resize( 1u );
			renderPass.attachments[0].format = colourView.getFormat();
			renderPass.attachments[0].samples = ashes::SampleCountFlag::e1;
			renderPass.attachments[0].loadOp = ashes::AttachmentLoadOp::eLoad;
			renderPass.attachments[0].storeOp = ashes::AttachmentStoreOp::eStore;
			renderPass.attachments[0].stencilLoadOp = ashes::AttachmentLoadOp::eDontCare;
			renderPass.attachments[0].stencilStoreOp = ashes::AttachmentStoreOp::eDontCare;
			renderPass.attachments[0].initialLayout = ashes::ImageLayout::eColourAttachmentOptimal;
			renderPass.attachments[0].finalLayout = ashes::ImageLayout::eColourAttachmentOptimal;

			ashes::AttachmentReference colourReference;
			colourReference.attachment = 0u;
			colourReference.layout = ashes::ImageLayout::eColourAttachmentOptimal;

			renderPass.subpasses.resize( 1u );
			renderPass.subpasses[0].flags = 0u;
			renderPass.subpasses[0].colorAttachments = { { 0u, ashes::ImageLayout::eColourAttachmentOptimal } };

			renderPass.dependencies.resize( 2u );
			renderPass.dependencies[0].srcSubpass = ashes::ExternalSubpass;
			renderPass.dependencies[0].dstSubpass = 0u;
			renderPass.dependencies[0].srcStageMask = ashes::PipelineStageFlag::eColourAttachmentOutput;
			renderPass.dependencies[0].dstStageMask = ashes::PipelineStageFlag::eColourAttachmentOutput;
			renderPass.dependencies[0].srcAccessMask = ashes::AccessFlag::eColourAttachmentWrite;
			renderPass.dependencies[0].dstAccessMask = ashes::AccessFlag::eColourAttachmentWrite;
			renderPass.dependencies[0].dependencyFlags = ashes::DependencyFlag::eByRegion;

			renderPass.dependencies[1].srcSubpass = 0u;
			renderPass.dependencies[1].dstSubpass = ashes::ExternalSubpass;
			renderPass.dependencies[1].srcStageMask = ashes::PipelineStageFlag::eColourAttachmentOutput;
			renderPass.dependencies[1].dstStageMask = ashes::PipelineStageFlag::eFragmentShader;
			renderPass.dependencies[1].srcAccessMask = ashes::AccessFlag::eColourAttachmentWrite;
			renderPass.dependencies[1].dstAccessMask = ashes::AccessFlag::eShaderRead;
			renderPass.dependencies[1].dependencyFlags = ashes::DependencyFlag::eByRegion;

			return getCurrentDevice( engine ).createRenderPass( renderPass );
		}

		ashes::FrameBufferPtr doCreateFrameBuffer( ashes::RenderPass const & renderPass
			, Size const & size
			, ashes::TextureView const & colourView )
		{
			ashes::FrameBufferAttachmentArray attaches
			{
				{ *( renderPass.getAttachments().begin() + 0u ), colourView },
			};
			return renderPass.createFrameBuffer( ashes::Extent2D{ size.getWidth(), size.getHeight() }
			, std::move( attaches ) );
		}
	}

	//*********************************************************************************************

	FinalCombineProgram::FinalCombineProgram( Engine & engine
		, ashes::RenderPass const & renderPass
		, RenderPassTimer & timer
		, ashes::DescriptorSetLayout const & uboLayout
		, ashes::DescriptorSetLayout const & texLayout
		, ashes::VertexLayout const & vtxLayout
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
		, m_vertexShader{ ashes::ShaderStageFlag::eVertex, "FinalCombine" }
		, m_pixelShader{ ashes::ShaderStageFlag::eFragment, "FinalCombine" }
	{
	}

	FinalCombineProgram::~FinalCombineProgram()
	{
		m_pipeline.reset();
	}

	void FinalCombineProgram::prepare( ashes::FrameBuffer const & frameBuffer
		, ashes::DescriptorSet const & uboDescriptorSet
		, ashes::DescriptorSet const & texDescriptorSet
		, ashes::BufferBase const & vbo )
	{
		m_commandBuffer->begin();
		m_timer.beginPass( *m_commandBuffer );
		m_commandBuffer->beginRenderPass( m_renderPass
			, frameBuffer
			, { ashes::ClearColorValue{} }
			, ashes::SubpassContents::eInline );
		m_commandBuffer->bindPipeline( *m_pipeline );
		m_commandBuffer->setViewport( { ashes::Offset2D{}, frameBuffer.getDimensions() } );
		m_commandBuffer->setScissor( { ashes::Offset2D{}, frameBuffer.getDimensions() } );
		m_commandBuffer->bindDescriptorSets( { uboDescriptorSet, texDescriptorSet }, *m_pipelineLayout );
		m_commandBuffer->bindVertexBuffer( 0u, vbo, 0u );
		m_commandBuffer->draw( 6u );
		m_commandBuffer->endRenderPass();
		m_timer.endPass( *m_commandBuffer );
		m_commandBuffer->end();
	}

	void FinalCombineProgram::accept( RenderTechniqueVisitor & visitor )
	{
		visitor.visit( cuT( "Combine" )
			, ashes::ShaderStageFlag::eVertex
			, *m_vertexShader.shader );
		visitor.visit( cuT( "Combine" )
			, ashes::ShaderStageFlag::eFragment
			, *m_pixelShader.shader );
	}

	//*********************************************************************************************

	FinalCombinePass::FinalCombinePass( Engine & engine
		, Size const & size
		, SceneUbo & sceneUbo
		, HdrConfigUbo & hdrConfigUbo
		, WeightedBlendTextures const & wbResult
		, ashes::TextureView const & colourView )
		: m_size{ size }
		, m_engine{ engine }
		, m_sceneUbo{ sceneUbo }
		, m_gpInfo{ m_engine }
		, m_sampler{ doCreateSampler( m_engine ) }
		, m_vertexBuffer{ doCreateVbo( m_engine ) }
		, m_vertexLayout{ doCreateVertexLayout( m_engine ) }
		, m_uboDescriptorLayout{ doCreateUboDescriptorLayout( m_engine ) }
		, m_uboDescriptorPool{ m_uboDescriptorLayout->createPool( uint32_t( FogType::eCount ) ) }
		, m_uboDescriptorSet{ doCreateUboDescriptorSet( *m_uboDescriptorPool, m_sceneUbo, m_gpInfo, hdrConfigUbo ) }
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

	ashes::Semaphore const & FinalCombinePass::render( FogType fogType
		, ashes::Semaphore const & toWait )
	{
		auto & program = m_programs[size_t( fogType )];
		auto timerBlock = m_timer->start();
		m_timer->notifyPassRender();
		auto * result = &toWait;

		getCurrentDevice( m_engine ).getGraphicsQueue().submit( program.getCommandBuffer()
			, *result
			, ashes::PipelineStageFlag::eColourAttachmentOutput
			, *m_semaphore
			, nullptr );
		result = m_semaphore.get();

		return *result;
	}

	void FinalCombinePass::accept( RenderTechniqueVisitor & visitor )
	{
		m_programs[size_t( getFogType( visitor.getFlags().sceneFlags ) )].accept( visitor );
	}

	//*********************************************************************************************
}
