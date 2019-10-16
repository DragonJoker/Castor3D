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

#include <ashespp/Buffer/VertexBuffer.hpp>
#include <ashespp/Command/CommandBufferInheritanceInfo.hpp>
#include <ashespp/Pipeline/GraphicsPipelineCreateInfo.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

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

		ashes::PipelineVertexInputStateCreateInfoPtr doCreateVertexLayout( Engine & engine )
		{
			return std::make_unique< ashes::PipelineVertexInputStateCreateInfo >( 0u
				, ashes::VkVertexInputBindingDescriptionArray
				{
					{ 0u, sizeof( TexturedQuad::Vertex ), VK_VERTEX_INPUT_RATE_VERTEX },
				}
				, ashes::VkVertexInputAttributeDescriptionArray
				{
					{ 0u, 0u, VK_FORMAT_R32G32_SFLOAT, offsetof( TexturedQuad::Vertex, position ) },
					{ 1u, 0u, VK_FORMAT_R32G32_SFLOAT, offsetof( TexturedQuad::Vertex, texture ) },
				} );
		}

		ashes::VertexBufferPtr< TexturedQuad > doCreateVbo( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto & device = getCurrentRenderDevice( renderSystem );
			auto result = makeVertexBuffer< TexturedQuad >( device
				, 1u
				, VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				, "FinalCombinePass" );

			if ( auto buffer = reinterpret_cast< TexturedQuad * >( result->getBuffer().lock( 0u
				, ~( 0ull )
				, 0u ) ) )
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
			ashes::VkDescriptorSetLayoutBindingArray bindings
			{
				makeDescriptorSetLayoutBinding( 0u
					, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
					, VK_SHADER_STAGE_FRAGMENT_BIT ),
				makeDescriptorSetLayoutBinding( 1u
					, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
					, VK_SHADER_STAGE_FRAGMENT_BIT ),
				makeDescriptorSetLayoutBinding( 2u
					, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
					, VK_SHADER_STAGE_FRAGMENT_BIT ),
			};
			auto & device = getCurrentRenderDevice( engine );
			return device->createDescriptorSetLayout( std::move( bindings ) );
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
			ashes::VkDescriptorSetLayoutBindingArray bindings
			{
				makeDescriptorSetLayoutBinding( 0u
					, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
					, VK_SHADER_STAGE_FRAGMENT_BIT ),
				makeDescriptorSetLayoutBinding( 1u
					, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
					, VK_SHADER_STAGE_FRAGMENT_BIT ),
				makeDescriptorSetLayoutBinding( 2u
					, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
					, VK_SHADER_STAGE_FRAGMENT_BIT ),
			};
			auto & device = getCurrentRenderDevice( engine );
			return device->createDescriptorSetLayout( std::move( bindings ) );
		}

		ashes::DescriptorSetPtr doCreateTexDescriptorSet( ashes::DescriptorSetPool & pool
			, ashes::ImageView const & depth
			, ashes::ImageView const & accumulation
			, ashes::ImageView const & revealage
			, Sampler const & sampler )
		{
			auto & layout = pool.getLayout();
			auto result = pool.createDescriptorSet( 1u );
			result->createBinding( layout.getBinding( 0u )
				, depth
				, sampler.getSampler() );
			result->createBinding( layout.getBinding( 1u )
				, accumulation
				, sampler.getSampler() );
			result->createBinding( layout.getBinding( 2u )
				, revealage
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
		
		ashes::PipelineShaderStageCreateInfoArray doCreateProgram( Engine & engine
			, FogType fogType
			, ShaderModule & vertexShader
			, ShaderModule & pixelShader )
		{
			auto & device = getCurrentRenderDevice( engine );
			vertexShader.shader = doGetVertexProgram( engine );
			pixelShader.shader = doGetPixelProgram( engine, fogType );

			return ashes::PipelineShaderStageCreateInfoArray
			{
				makeShaderState( device, vertexShader ),
				makeShaderState( device, pixelShader ),
			};
		}

		ashes::PipelineLayoutPtr doCreateRenderPipelineLayout( Engine & engine
			, ashes::DescriptorSetLayout const & uboLayout
			, ashes::DescriptorSetLayout const & texLayout )
		{
			return getCurrentRenderDevice( engine )->createPipelineLayout( { uboLayout, texLayout } );
		}

		ashes::GraphicsPipelinePtr doCreateRenderPipeline( Engine & engine
			, ashes::PipelineLayout const & pipelineLayout
			, ashes::PipelineShaderStageCreateInfoArray const & program
			, ashes::RenderPass const & renderPass
			, ashes::PipelineVertexInputStateCreateInfo const & vtxLayout )
		{
			ashes::VkPipelineColorBlendAttachmentStateArray attachments
			{
				{
					VK_TRUE,
					VK_BLEND_FACTOR_SRC_ALPHA,
					VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
					VK_BLEND_OP_ADD,
					VK_BLEND_FACTOR_SRC_ALPHA,
					VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
					VK_BLEND_OP_ADD,
					defaultColorWriteMask,
				}
			};
			ashes::PipelineColorBlendStateCreateInfo bdState
			{
				0u,
				VK_FALSE,
				VK_LOGIC_OP_COPY,
				std::move( attachments )
			};

			auto & device = getCurrentRenderDevice( engine );
			return device->createPipeline( ashes::GraphicsPipelineCreateInfo
				{
					0u,
					program,
					vtxLayout,
					ashes::PipelineInputAssemblyStateCreateInfo{ 0u, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP },
					std::nullopt,
					ashes::PipelineViewportStateCreateInfo{},
					ashes::PipelineRasterizationStateCreateInfo{ 0u, VK_FALSE, VK_FALSE, VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE },
					ashes::PipelineMultisampleStateCreateInfo{},
					ashes::PipelineDepthStencilStateCreateInfo{ 0u, VK_FALSE, VK_FALSE },
					std::move( bdState ),
					ashes::PipelineDynamicStateCreateInfo{ 0u, { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR } },
					pipelineLayout,
					renderPass,
				} );
		}

		ashes::RenderPassPtr doCreateRenderPass( Engine & engine
			, ashes::ImageView const & colourView )
		{
			ashes::VkAttachmentDescriptionArray attaches
			{
				{
					0u,
					colourView.getFormat(),
					VK_SAMPLE_COUNT_1_BIT,
					VK_ATTACHMENT_LOAD_OP_LOAD,
					VK_ATTACHMENT_STORE_OP_STORE,
					VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					VK_ATTACHMENT_STORE_OP_DONT_CARE,
					VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
					VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				}
			};
			ashes::SubpassDescriptionArray subpasses;
			subpasses.emplace_back( ashes::SubpassDescription
				{
					0u,
					VK_PIPELINE_BIND_POINT_GRAPHICS,
					{},
					{ { 0u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } },
					{},
					std::nullopt,
					{},
				} );
			ashes::VkSubpassDependencyArray dependencies
			{
				{
					VK_SUBPASS_EXTERNAL,
					0u,
					VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
					VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
					VK_DEPENDENCY_BY_REGION_BIT,
				},
				{
					0u,
					VK_SUBPASS_EXTERNAL,
					VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
					VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
					VK_ACCESS_SHADER_READ_BIT,
					VK_DEPENDENCY_BY_REGION_BIT,
				},
			};
			ashes::RenderPassCreateInfo createInfo
			{
				0u,
				std::move( attaches ),
				std::move( subpasses ),
				std::move( dependencies ),
			};
			auto & renderSystem = *engine.getRenderSystem();
			auto & device = getCurrentRenderDevice( renderSystem );
			auto result = device->createRenderPass( std::move( createInfo ) );
			setDebugObjectName( device, *result, "FinalCombinePass" );
			return result;
		}

		ashes::FrameBufferPtr doCreateFrameBuffer( ashes::RenderPass const & renderPass
			, Size const & size
			, ashes::ImageView const & colourView )
		{
			ashes::ImageViewCRefArray attaches
			{
				colourView,
			};
			return renderPass.createFrameBuffer( VkExtent2D{ size.getWidth(), size.getHeight() }
			, std::move( attaches ) );
		}
	}

	//*********************************************************************************************

	FinalCombineProgram::FinalCombineProgram( Engine & engine
		, ashes::RenderPass const & renderPass
		, RenderPassTimer & timer
		, ashes::DescriptorSetLayout const & uboLayout
		, ashes::DescriptorSetLayout const & texLayout
		, ashes::PipelineVertexInputStateCreateInfo const & vtxLayout
		, FogType fogType )
		: m_timer{ timer }
		, m_renderPass{ renderPass }
		, m_pipelineLayout{ doCreateRenderPipelineLayout( engine
			, uboLayout
			, texLayout ) }
		, m_pipeline{ doCreateRenderPipeline( engine
			, *m_pipelineLayout
			, doCreateProgram( engine, fogType, m_vertexShader, m_pixelShader )
			, renderPass
			, vtxLayout ) }
		, m_commandBuffer{ getCurrentRenderDevice( engine ).graphicsCommandPool->createCommandBuffer( true ) }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, "FinalCombine" }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, "FinalCombine" }
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
			, { ashes::makeClearValue( VkClearColorValue{} ) }
			, VK_SUBPASS_CONTENTS_INLINE );
		m_commandBuffer->bindPipeline( *m_pipeline );
		m_commandBuffer->setViewport( ashes::makeViewport( frameBuffer.getDimensions() ) );
		m_commandBuffer->setScissor( ashes::makeScissor( frameBuffer.getDimensions() ) );
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
			, VK_SHADER_STAGE_VERTEX_BIT
			, *m_vertexShader.shader );
		visitor.visit( cuT( "Combine" )
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, *m_pixelShader.shader );
	}

	//*********************************************************************************************

	FinalCombinePass::FinalCombinePass( Engine & engine
		, Size const & size
		, SceneUbo & sceneUbo
		, HdrConfigUbo & hdrConfigUbo
		, WeightedBlendTextures const & wbResult
		, ashes::ImageView const & colourView )
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
		, m_semaphore{ getCurrentRenderDevice( m_engine )->createSemaphore() }
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

		getCurrentRenderDevice( m_engine ).graphicsQueue->submit( program.getCommandBuffer()
			, *result
			, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
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
