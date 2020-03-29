#include "Castor3D/Render/Technique/Opaque/Ssao/LineariseDepthPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/SamplerCache.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Shader/GlslToSpv.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslShadow.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Render/Technique/Opaque/Ssao/SsaoConfigUbo.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"

#include <ashespp/Buffer/VertexBuffer.hpp>
#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Command/Queue.hpp>
#include <ashespp/Core/Device.hpp>
#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Descriptor/DescriptorSetPool.hpp>
#include <ashespp/Image/Image.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/Miscellaneous/RendererFeatures.hpp>
#include <ashespp/Pipeline/GraphicsPipeline.hpp>
#include <ashespp/Pipeline/GraphicsPipelineCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineLayout.hpp>
#include <ashespp/Pipeline/PipelineVertexInputStateCreateInfo.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>

#include <random>

using namespace castor;

namespace castor3d
{
	namespace
	{
		static uint32_t constexpr DepthImgIdx = 0u;
		static uint32_t constexpr ClipInfoUboIdx = 1u;
		static uint32_t constexpr PrevLvlUboIdx = 1u;

		ShaderPtr doGetVertexProgram( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace sdw;
			VertexWriter writer;

			// Shader inputs
			auto position = writer.declInput< Vec2 >( "position", 0u );

			// Shader outputs
			auto out = writer.getOut();

			writer.implementFunction< void >( "main"
				, [&]()
				{
					out.vtx.position = vec4( position, 0.0_f, 1.0_f );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
		
		ShaderPtr doGetLinearisePixelProgram( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			Ubo clipInfo{ writer, "ClipInfo", ClipInfoUboIdx, 0u, ast::type::MemoryLayout::eStd140 };
			auto c3d_clipInfo = clipInfo.declMember< Vec3 >( "c3d_clipInfo" );
			clipInfo.end();
			auto c3d_mapDepth = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapDepth", DepthImgIdx, 0u );
			auto in = writer.getIn();

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Float >( "pxl_fragColor", 0u );

			auto reconstructCSZ = writer.implementFunction< Float >( "reconstructCSZ"
				, [&]( Float const & depth
					, Vec3 const & clipInfo )
				{
					writer.returnStmt( clipInfo[0] / ( clipInfo[1] * depth + clipInfo[2] ) );
				}
				, InFloat{ writer, "d" }
				, InVec3{ writer, "clipInfo" } );

			writer.implementFunction< Void >( "main"
				, [&]()
				{
					pxl_fragColor = reconstructCSZ( texelFetch( c3d_mapDepth, ivec2( in.fragCoord.xy() ), 0_i ).r()
						, c3d_clipInfo );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
		
		ShaderPtr doGetMinifyPixelProgram( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			Ubo previousLevel{ writer, "PreviousLevel", PrevLvlUboIdx, 0u, ast::type::MemoryLayout::eStd140 };
			auto c3d_textureSize = previousLevel.declMember< IVec2 >( "c3d_textureSize" );
			auto c3d_previousLevel = previousLevel.declMember< Int >( "c3d_previousLevel" );
			previousLevel.end();
			auto c3d_mapDepth = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapDepth", DepthImgIdx, 0u );
			auto in = writer.getIn();

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Float >( "pxl_fragColor", 0u );

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					auto ssPosition = writer.declLocale( "ssPosition"
						, ivec2( in.fragCoord.xy() ) );

					// Rotated grid subsampling to avoid XY directional bias or Z precision bias while downsampling.
					pxl_fragColor = texelFetch( c3d_mapDepth
						, clamp( ssPosition * 2 + ivec2( ssPosition.y() & 1, ssPosition.x() & 1 )
							, ivec2( 0_i, 0_i )
							, c3d_textureSize - ivec2( 1_i, 1_i ) )
						, c3d_previousLevel ).r();
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		ashes::PipelineShaderStageCreateInfoArray doGetLineariseProgram( Engine & engine
			, ShaderModule & vertexShader
			, ShaderModule & pixelShader )
		{
			auto & device = getCurrentRenderDevice( engine );
			vertexShader.shader = doGetVertexProgram( engine );
			pixelShader.shader = doGetLinearisePixelProgram( engine );
			return ashes::PipelineShaderStageCreateInfoArray
			{
				makeShaderState( device, vertexShader ),
				makeShaderState( device, pixelShader ),
			};
		}

		ashes::PipelineShaderStageCreateInfoArray doGetMinifyProgram( Engine & engine
			, ShaderModule & vertexShader
			, ShaderModule & pixelShader )
		{
			auto & device = getCurrentRenderDevice( engine );
			vertexShader.shader = doGetVertexProgram( engine );
			pixelShader.shader = doGetMinifyPixelProgram( engine );
			return ashes::PipelineShaderStageCreateInfoArray
			{
				makeShaderState( device, vertexShader ),
				makeShaderState( device, pixelShader ),
			};
		}

		SamplerSPtr doCreateSampler( Engine & engine
			, String const & name
			, VkSamplerAddressMode mode )
		{
			SamplerSPtr sampler;

			if ( engine.getSamplerCache().has( name ) )
			{
				sampler = engine.getSamplerCache().find( name );
			}
			else
			{
				sampler = engine.getSamplerCache().add( name );
				sampler->setMinFilter( VK_FILTER_NEAREST );
				sampler->setMagFilter( VK_FILTER_NEAREST );
				sampler->setMipFilter( VK_SAMPLER_MIPMAP_MODE_NEAREST );
				sampler->setWrapS( mode );
				sampler->setWrapT( mode );
				sampler->setMinLod( 0.0 );
				sampler->setMaxLod( float( LineariseDepthPass::MaxMipLevel ) );
			}

			return sampler;
		}

		TextureUnit doCreateTexture( Engine & engine, VkExtent2D const & size )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto sampler = doCreateSampler( engine
				, cuT( "LinearisePass_Result" )
				, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );

			ashes::ImageCreateInfo image
			{
				0u,
				VK_IMAGE_TYPE_2D,
				VK_FORMAT_R32_SFLOAT,
				{ size.width, size.height, 1u },
				LineariseDepthPass::MaxMipLevel + 1u,
				1u,
				VK_SAMPLE_COUNT_1_BIT,
				VK_IMAGE_TILING_OPTIMAL,
				( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
					| VK_IMAGE_USAGE_SAMPLED_BIT
					| VK_IMAGE_USAGE_TRANSFER_DST_BIT
					| VK_IMAGE_USAGE_TRANSFER_SRC_BIT ),
			};
			auto ssaoResult = std::make_shared< TextureLayout >( renderSystem
				, image
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, cuT( "LinearisePass_Result" ) );
			TextureUnit result{ engine };
			result.setTexture( ssaoResult );
			result.setSampler( sampler );
			result.initialise();
			return result;
		}

		ashes::RenderPassPtr doCreateRenderPass( Engine & engine )
		{
			ashes::VkAttachmentDescriptionArray attachments
			{
				{
					0u,
					VK_FORMAT_R32_SFLOAT,
					VK_SAMPLE_COUNT_1_BIT,
					VK_ATTACHMENT_LOAD_OP_CLEAR,
					VK_ATTACHMENT_STORE_OP_STORE,
					VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					VK_ATTACHMENT_STORE_OP_DONT_CARE,
					VK_IMAGE_LAYOUT_UNDEFINED,
					VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				},
			};
			ashes::SubpassDescriptionArray subpasses;
			subpasses.emplace_back( ashes::SubpassDescription
				{
					0u,
					VK_PIPELINE_BIND_POINT_GRAPHICS,
					{},
					{ { 0u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } },
					{},
					ashes::nullopt,
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
				std::move( attachments ),
				std::move( subpasses ),
				std::move( dependencies ),
			};
			auto & renderSystem = *engine.getRenderSystem();
			auto & device = getCurrentRenderDevice( renderSystem );
			auto result = device->createRenderPass( std::move( createInfo ) );
			setDebugObjectName( device, *result, "LineariseDepth" );
			return result;
		}

		ashes::FrameBufferPtr doCreateFrameBuffer( ashes::RenderPass const & renderPass
			, TextureUnit const & texture )
		{
			ashes::ImageViewCRefArray attaches;
			attaches.emplace_back( texture.getTexture()->getDefaultView() );
			auto size = texture.getTexture()->getDimensions();
			return renderPass.createFrameBuffer( VkExtent2D{ size.width, size.height }
				, std::move( attaches ) );
		}

		ashes::VertexBufferPtr< NonTexturedQuad > doCreateVertexBuffer( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto & device = getCurrentRenderDevice( renderSystem );
			auto result = makeVertexBuffer< NonTexturedQuad >( device
				, 1u
				, 0u
				, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				, "LineariseDepthPass" );

			if ( auto buffer = reinterpret_cast< NonTexturedQuad * >( result->getBuffer().lock( 0u
				, ~( 0ull )
				, 0u ) ) )
			{
				*buffer = NonTexturedQuad
				{
					{
						{ Point2f{ -1.0, -1.0 } },
						{ Point2f{ -1.0, +1.0 } },
						{ Point2f{ +1.0, -1.0 } },
						{ Point2f{ +1.0, -1.0 } },
						{ Point2f{ -1.0, +1.0 } },
						{ Point2f{ +1.0, +1.0 } },
					}
				};
				result->getBuffer().flush( 0u, ~( 0ull ) );
				result->getBuffer().unlock();
			}

			return result;
		}

		ashes::PipelineVertexInputStateCreateInfoPtr doCreateVertexLayout( Engine & engine )
		{
			ashes::PipelineVertexInputStateCreateInfo inputState
			{
				0u,
				ashes::VkVertexInputBindingDescriptionArray
				{
					{ 0u, sizeof( castor3d::NonTexturedQuad::Vertex ), VK_VERTEX_INPUT_RATE_VERTEX },
				},
				ashes::VkVertexInputAttributeDescriptionArray
				{
					{ 0u, 0u, VK_FORMAT_R32G32_SFLOAT, offsetof( castor3d::NonTexturedQuad::Vertex, position ) },
				}
			};
			return std::make_unique< ashes::PipelineVertexInputStateCreateInfo >( inputState );
		}
	}

	//*********************************************************************************************

	LineariseDepthPass::LineariseDepthPass( Engine & engine
		, VkExtent2D const & size
		, SsaoConfigUbo & ssaoConfigUbo
		, ashes::ImageView const & depthBuffer )
		: m_engine{ engine }
		, m_ssaoConfigUbo{ ssaoConfigUbo }
		, m_depthBuffer{ depthBuffer }
		, m_size{ size }
		, m_result{ doCreateTexture( m_engine, m_size ) }
		, m_timer{ std::make_shared< RenderPassTimer >( m_engine
			, cuT( "SSAO" )
			, cuT( "Linearise depth" ) ) }
		, m_renderPass{ doCreateRenderPass( m_engine ) }
		, m_vertexBuffer{ doCreateVertexBuffer( m_engine ) }
		, m_vertexLayout{ doCreateVertexLayout( m_engine ) }
		, m_lineariseSampler{ getCurrentRenderDevice( m_engine )->createSampler( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, VK_FILTER_NEAREST
			, VK_FILTER_NEAREST ) }
		, m_minifySampler{ getCurrentRenderDevice( m_engine )->createSampler( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, VK_FILTER_NEAREST
			, VK_FILTER_NEAREST ) }
		, m_commandBuffer{ getCurrentRenderDevice( m_engine ).graphicsCommandPool->createCommandBuffer() }
		, m_finished{ getCurrentRenderDevice( m_engine )->createSemaphore() }
		, m_clipInfo{ makeUniformBuffer< Point3f >( *m_engine.getRenderSystem()
			, 1u
			, 0u
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, "LineariseDepthClipInfo" ) }
		, m_lineariseVertexShader{ VK_SHADER_STAGE_VERTEX_BIT, "LineariseDepth" }
		, m_linearisePixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, "LineariseDepth" }
		, m_lineariseProgram{ doGetLineariseProgram( m_engine
			, m_lineariseVertexShader
			, m_linearisePixelShader ) }
		, m_minifyVertexShader{ VK_SHADER_STAGE_VERTEX_BIT, "Minify" }
		, m_minifyPixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, "Minify" }
		, m_minifyProgram{ doGetMinifyProgram( m_engine
			, m_minifyVertexShader
			, m_minifyPixelShader ) }
	{
		doInitialiseLinearisePass();
		doInitialiseMinifyPass();
	}

	LineariseDepthPass::~LineariseDepthPass()
	{
		doCleanupMinifyPass();
		doCleanupLinearisePass();
		m_renderPass.reset();
		m_timer.reset();
		m_result.cleanup();
	}

	void LineariseDepthPass::update( Viewport const & viewport )
	{
		auto z_f = viewport.getFar();
		auto z_n = viewport.getNear();
		auto clipInfo = std::isinf( z_f )
			? Point3f{ z_n, -1.0f, 1.0f }
			: Point3f{ z_n * z_f, z_n - z_f, z_f };
		// result = clipInfo[0] / ( clipInfo[1] * depth + clipInfo[2] );
		// depth = 0 => result = z_n
		// depth = 1 => result = z_f
		m_clipInfoValue = clipInfo;

		if ( m_clipInfoValue.isDirty() )
		{
			m_clipInfo->getData() = m_clipInfoValue;
			m_clipInfo->upload( 0u );
			doPrepareFrame();
		}
	}

	ashes::Semaphore const & LineariseDepthPass::linearise( ashes::Semaphore const & toWait )const
	{
		auto timerBlock = m_timer->start();
		m_timer->notifyPassRender();
		auto & device = getCurrentRenderDevice( m_engine );
		auto * result = &toWait;

		device.graphicsQueue->submit( *m_commandBuffer
			, *result
			, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
			, *m_finished
			, nullptr );
		result = m_finished.get();

		return *result;
	}

	void LineariseDepthPass::accept( RenderTechniqueVisitor & visitor )
	{
		visitor.visit( cuT( "SSAO - Linearise" )
			, VK_SHADER_STAGE_VERTEX_BIT
			, *m_lineariseVertexShader.shader );
		visitor.visit( cuT( "SSAO - Linearise" )
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, *m_linearisePixelShader.shader );

		visitor.visit( cuT( "SSAO - Minify" )
			, VK_SHADER_STAGE_VERTEX_BIT
			, *m_minifyVertexShader.shader );
		visitor.visit( cuT( "SSAO - Minify" )
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, *m_minifyPixelShader.shader );
	}

	void LineariseDepthPass::doInitialiseLinearisePass()
	{
		auto size = m_result.getTexture()->getDimensions();
		ashes::ImageViewCRefArray attaches;
		m_linearisedView = m_result.getTexture()->getTexture().createView( VK_IMAGE_VIEW_TYPE_2D
			, m_result.getTexture()->getPixelFormat() );
		attaches.emplace_back( m_linearisedView );
		m_lineariseFrameBuffer = m_renderPass->createFrameBuffer( VkExtent2D{ size.width, size.height }
			, std::move( attaches ) );
		auto & renderSystem = *m_engine.getRenderSystem();
		auto & device = getCurrentRenderDevice( renderSystem );
		ashes::VkDescriptorSetLayoutBindingArray bindings
		{
			makeDescriptorSetLayoutBinding( DepthImgIdx
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ),
			makeDescriptorSetLayoutBinding( ClipInfoUboIdx
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, VK_SHADER_STAGE_FRAGMENT_BIT ),
		};
		m_lineariseDescriptorLayout = device->createDescriptorSetLayout( std::move( bindings ) );
		m_linearisePipelineLayout = device->createPipelineLayout( *m_lineariseDescriptorLayout );

		m_lineariseDescriptorPool = m_lineariseDescriptorLayout->createPool( 1u );
		m_lineariseDescriptor = m_lineariseDescriptorPool->createDescriptorSet();
		m_lineariseDescriptor->createBinding( m_lineariseDescriptorLayout->getBinding( 0u )
			, m_depthBuffer
			, *m_lineariseSampler );
		m_lineariseDescriptor->createSizedBinding( m_lineariseDescriptorLayout->getBinding( 1u )
			, *m_clipInfo
			, 0u );
		m_lineariseDescriptor->update();

		m_linearisePipeline = device->createPipeline( ashes::GraphicsPipelineCreateInfo
			{
				0u,
				m_lineariseProgram,
				*m_vertexLayout,
				ashes::PipelineInputAssemblyStateCreateInfo{},
				ashes::nullopt,
				ashes::PipelineViewportStateCreateInfo
				{
					0u,
					{ 1u, ashes::makeViewport( size ) },
					{ 1u, ashes::makeScissor( size ) }
				},
				ashes::PipelineRasterizationStateCreateInfo{},
				ashes::PipelineMultisampleStateCreateInfo{},
				ashes::nullopt,
				ashes::PipelineColorBlendStateCreateInfo{},
				ashes::nullopt,
				*m_linearisePipelineLayout,
				*m_renderPass,
			} );
	}

	void LineariseDepthPass::doInitialiseMinifyPass()
	{
		auto & renderSystem = *m_engine.getRenderSystem();
		auto & device = getCurrentRenderDevice( renderSystem );
		auto size = m_result.getTexture()->getDimensions();
		ashes::VkDescriptorSetLayoutBindingArray bindings
		{
			makeDescriptorSetLayoutBinding( DepthImgIdx
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ),
			makeDescriptorSetLayoutBinding( PrevLvlUboIdx
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, VK_SHADER_STAGE_FRAGMENT_BIT ),
		};
		m_minifyDescriptorLayout = device->createDescriptorSetLayout( std::move( bindings ) );
		m_minifyPipelineLayout = device->createPipelineLayout( *m_minifyDescriptorLayout );

		m_minifyDescriptorPool = m_minifyDescriptorLayout->createPool( MaxMipLevel );
		uint32_t index = 0u;
		auto * sourceView = &m_linearisedView;
		ashes::ImageViewCreateInfo viewInfo
		{
			0u,
			VK_NULL_HANDLE,
			VK_IMAGE_VIEW_TYPE_2D,
			m_result.getTexture()->getPixelFormat(),
			VkComponentMapping{},
			VkImageSubresourceRange
			{
				VK_IMAGE_ASPECT_COLOR_BIT,
				0u,
				1u,
				0u,
				1u,
			},
		};

		m_previousLevel = makeUniformBuffer< MinifyConfiguration >( *m_engine.getRenderSystem()
			, MaxMipLevel
			, 0u
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, "LineariseDepthPreviousLevel" );

		for ( auto & pipeline : m_minifyPipelines )
		{
			auto & data = m_previousLevel->getData( index );
			data.previousLevel = index;
			data.textureSize = Point2i{ size.width, size.height };
			size.width >>= 1;
			size.height >>= 1;
			pipeline.sourceView = sourceView;
			viewInfo->subresourceRange.baseMipLevel = index + 1u;
			viewInfo->image = m_result.getTexture()->getTexture();
			pipeline.targetView = m_result.getTexture()->getTexture().createView( viewInfo );
			pipeline.descriptor = m_minifyDescriptorPool->createDescriptorSet();
			pipeline.descriptor->createBinding( m_minifyDescriptorLayout->getBinding( 0u )
				, *pipeline.sourceView
				, *m_minifySampler );
			pipeline.descriptor->createSizedBinding( m_minifyDescriptorLayout->getBinding( 1u )
				, m_previousLevel->getBuffer()
				, index );
			pipeline.descriptor->update();
			ashes::ImageViewCRefArray attaches;
			attaches.emplace_back( pipeline.targetView );
			pipeline.frameBuffer = m_renderPass->createFrameBuffer( VkExtent2D{ size.width, size.height }
				, std::move( attaches ) );

			pipeline.pipeline = device->createPipeline( ashes::GraphicsPipelineCreateInfo
				{
					0u,
					m_minifyProgram,
					*m_vertexLayout,
					ashes::PipelineInputAssemblyStateCreateInfo{},
					ashes::nullopt,
					ashes::PipelineViewportStateCreateInfo
					{
						0u,
						{ 1u, VkViewport{ 0.0f, 0.0f, float( size.width ), float( size.height ), 0.0f, 1.0f } },
						{ 1u, VkRect2D{ 0, 0, size.width, size.height } }
					},
					ashes::PipelineRasterizationStateCreateInfo{},
					ashes::PipelineMultisampleStateCreateInfo{},
					ashes::nullopt,
					ashes::PipelineColorBlendStateCreateInfo{},
					ashes::nullopt,
					*m_minifyPipelineLayout,
					*m_renderPass,
				} );
			sourceView = &pipeline.targetView;
			++index;
		}

		m_previousLevel->upload( 0u, MaxMipLevel );
	}

	void LineariseDepthPass::doCleanupLinearisePass()
	{
		m_linearisePipeline.reset();
		m_linearisePipelineLayout.reset();
		m_lineariseDescriptor.reset();
		m_lineariseDescriptorPool.reset();
		m_lineariseDescriptorLayout.reset();
		m_lineariseFrameBuffer.reset();
	}

	void LineariseDepthPass::doCleanupMinifyPass()
	{
		for ( auto & pipeline : m_minifyPipelines )
		{
			pipeline.pipeline.reset();
			pipeline.frameBuffer.reset();
			pipeline.descriptor.reset();
		}

		m_previousLevel.reset();
		m_minifyDescriptorPool.reset();
		m_minifyPipelineLayout.reset();
		m_minifyDescriptorLayout.reset();
	}

	void LineariseDepthPass::doPrepareFrame()
	{
		m_commandBuffer->begin();
		m_commandBuffer->beginDebugBlock(
			{
				"SSAO - Linearise Depth",
				{
					1.0f,
					1.0f,
					0.4f,
					1.0f,
				},
			} );
		m_timer->beginPass( *m_commandBuffer );

		// Linearisation pass.
		auto subresource = m_depthBuffer->subresourceRange;
		subresource.aspectMask = ashes::getAspectMask( m_depthBuffer.getFormat() );
		m_commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
			, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
			, makeVkType< VkImageMemoryBarrier >( VkAccessFlags( 0u )
				, VkAccessFlags( VK_ACCESS_SHADER_READ_BIT )
				, VK_IMAGE_LAYOUT_UNDEFINED
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				, VK_QUEUE_FAMILY_IGNORED
				, VK_QUEUE_FAMILY_IGNORED
				, *m_depthBuffer.image
				, subresource ) );
		m_commandBuffer->beginRenderPass( *m_renderPass
			, *m_lineariseFrameBuffer
			, { transparentBlackClearColor }
			, VK_SUBPASS_CONTENTS_INLINE );
		m_commandBuffer->bindPipeline( *m_linearisePipeline );
		m_commandBuffer->bindDescriptorSet( *m_lineariseDescriptor, *m_linearisePipelineLayout );
		m_commandBuffer->bindVertexBuffer( 0u, m_vertexBuffer->getBuffer(), 0u );
		m_commandBuffer->draw( 6u );
		m_commandBuffer->endRenderPass();
		m_commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
			, VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT
			, makeVkType< VkImageMemoryBarrier >( VkAccessFlags( VK_ACCESS_SHADER_READ_BIT )
				, VkAccessFlags( VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT )
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
				, VK_QUEUE_FAMILY_IGNORED
				, VK_QUEUE_FAMILY_IGNORED
				, *m_depthBuffer.image
				, subresource ) );
		m_commandBuffer->endDebugBlock();
		m_commandBuffer->beginDebugBlock(
			{
				"SSAO - Minify",
				{
					1.0f,
					1.0f,
					0.6f,
					1.0f,
				},
			} );

		// Minification passes.
		for ( auto & pipeline : m_minifyPipelines )
		{
			m_commandBuffer->beginRenderPass( *m_renderPass
				, *pipeline.frameBuffer
				, { transparentBlackClearColor }
				, VK_SUBPASS_CONTENTS_INLINE );
			m_commandBuffer->bindPipeline( *pipeline.pipeline );
			m_commandBuffer->bindDescriptorSet( *pipeline.descriptor, *m_minifyPipelineLayout );
			m_commandBuffer->bindVertexBuffer( 0u, m_vertexBuffer->getBuffer(), 0u );
			m_commandBuffer->draw( 6u );
			m_commandBuffer->endRenderPass();
		}

		m_timer->endPass( *m_commandBuffer );
		m_commandBuffer->endDebugBlock();
		m_commandBuffer->end();
	}
}
