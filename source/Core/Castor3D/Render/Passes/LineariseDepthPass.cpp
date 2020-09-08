#include "Castor3D/Render/Passes/LineariseDepthPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
#include "Castor3D/Cache/SamplerCache.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/Viewport.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"
#include "Castor3D/Render/Ssao/SsaoConfigUbo.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Shader/GlslToSpv.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"

#include <CastorUtils/Graphics/RgbaColour.hpp>

#include <ashespp/Buffer/VertexBuffer.hpp>
#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Sync/Queue.hpp>
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

		ashes::ImageView doCreateImageView( Engine & engine, ashes::ImageView const & srcView )
		{
			auto createInfos = srcView.createInfo;
			createInfos.subresourceRange.aspectMask = ashes::getAspectMask( srcView->format );
			auto result = srcView.image->createView( "LineariseDepth"
				, createInfos );
			return result;
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
			auto result = device->createRenderPass( "LineariseDepthPass"
				, std::move( createInfo ) );
			return result;
		}

		ashes::FrameBufferPtr doCreateFrameBuffer( ashes::RenderPass const & renderPass
			, TextureUnit const & texture )
		{
			ashes::ImageViewCRefArray attaches;
			attaches.emplace_back( texture.getTexture()->getDefaultView().getTargetView() );
			auto size = texture.getTexture()->getDimensions();
			return renderPass.createFrameBuffer( "LineariseDepthPass"
				, VkExtent2D{ size.width, size.height }
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
		, String const & prefix
		, castor::Size const & size
		, ashes::ImageView const & depthBuffer )
		: m_engine{ engine }
		, m_srcDepthBuffer{ depthBuffer }
		, m_depthBuffer{ doCreateImageView( engine, m_srcDepthBuffer ) }
		, m_prefix{ prefix }
		, m_size{ makeExtent2D( size ) }
		, m_result{ doCreateTexture( m_engine, m_size ) }
		, m_vertexLayout{ doCreateVertexLayout( m_engine ) }
		, m_lineariseVertexShader{ VK_SHADER_STAGE_VERTEX_BIT, m_prefix + "LineariseDepth" }
		, m_linearisePixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, m_prefix + "LineariseDepth" }
		, m_minifyVertexShader{ VK_SHADER_STAGE_VERTEX_BIT, m_prefix + "MinifyDepth" }
		, m_minifyPixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, m_prefix + "MinifyDepth" }
	{
	}

	void LineariseDepthPass::initialise()
	{
		m_result.initialise();
		m_timer = std::make_shared< RenderPassTimer >( m_engine
			, m_prefix
			, cuT( "Linearise depth" ) );
		m_renderPass = doCreateRenderPass( m_engine );
		m_vertexBuffer = doCreateVertexBuffer( m_engine );
		m_lineariseSampler = getCurrentRenderDevice( m_engine )->createSampler( m_prefix + "LineariseDepthLinearise"
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, VK_FILTER_NEAREST
			, VK_FILTER_NEAREST );
		m_minifySampler = getCurrentRenderDevice( m_engine )->createSampler( m_prefix + "MinifyDepth"
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, VK_FILTER_NEAREST
			, VK_FILTER_NEAREST );
		m_commandBuffer = getCurrentRenderDevice( m_engine ).graphicsCommandPool->createCommandBuffer( m_prefix + "LineariseDepth" );
		m_finished = getCurrentRenderDevice( m_engine )->createSemaphore( m_prefix + "LineariseDepth" );
		m_clipInfo = m_engine.getUboPools().getBuffer< Point3f >( 0u );
		doInitialiseLinearisePass();
		doInitialiseMinifyPass();
	}

	void LineariseDepthPass::cleanup()
	{
		doCleanupMinifyPass();
		doCleanupLinearisePass();
		m_engine.getUboPools().putBuffer( m_clipInfo );
		m_finished.reset();
		m_commandBuffer.reset();
		m_minifySampler.reset();
		m_lineariseSampler.reset();
		m_vertexBuffer.reset();
		m_renderPass.reset();
		m_timer.reset();
		m_result.cleanup();
	}

	void LineariseDepthPass::cpuUpdate( Viewport const & viewport )
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
			m_clipInfo.getData() = m_clipInfoValue;
		}
	}

	void LineariseDepthPass::gpuUpdate( ashes::CommandBuffer * cb )
	{
		if ( cb
			&& m_clipInfoValue.isDirty() )
		{
			doPrepareFrame( *cb, *m_timer, 0u );
		}

		m_clipInfoValue.reset();
	}
	
	void LineariseDepthPass::gpuUpdate()
	{
		gpuUpdate( m_commandBuffer.get() );
	}

	ashes::Semaphore const & LineariseDepthPass::linearise( ashes::Semaphore const & toWait )const
	{
		RenderPassTimerBlock timerBlock{ m_timer->start() };
		timerBlock->notifyPassRender();
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

	CommandsSemaphore LineariseDepthPass::getCommands( RenderPassTimer const & timer
		, uint32_t index )const
	{
		auto & device = getCurrentRenderDevice( *m_engine.getRenderSystem() );
		castor3d::CommandsSemaphore commands
		{
			device.graphicsCommandPool->createCommandBuffer( m_prefix + "LineariseDepthPass" ),
			device->createSemaphore( m_prefix + "LineariseDepthPass" )
		};
		auto & cmd = *commands.commandBuffer;
		doPrepareFrame( cmd, timer, index );
		return commands;
	}

	void LineariseDepthPass::accept( PipelineVisitorBase & visitor )
	{
		uint32_t index = 0u;

		for ( auto & layer : getResult().getTexture()->getArray2D().layers )
		{
			visitor.visit( "Linearised Depth " + string::toString( index++ )
				, layer.view->getSampledView() );
		}

		visitor.visit( m_lineariseVertexShader );
		visitor.visit( m_linearisePixelShader );

		visitor.visit( m_minifyVertexShader );
		visitor.visit( m_minifyPixelShader );
	}

	void LineariseDepthPass::doInitialiseLinearisePass()
	{
		auto size = m_result.getTexture()->getDimensions();
		auto lineariseProgram = doGetLineariseProgram( m_engine
			, m_lineariseVertexShader
			, m_linearisePixelShader );
		ashes::ImageViewCRefArray attaches;
		m_linearisedView = m_result.getTexture()->getTexture().createView( "LinearisedDepth"
			, VK_IMAGE_VIEW_TYPE_2D
			, m_result.getTexture()->getPixelFormat() );
		attaches.emplace_back( m_linearisedView );
		m_lineariseFrameBuffer = m_renderPass->createFrameBuffer( "LineariseDepthPass"
			, VkExtent2D{ size.width, size.height }
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
		m_lineariseLayout.descriptorLayout = device->createDescriptorSetLayout( "LineariseDepthPass"
			, std::move( bindings ) );
		m_lineariseLayout.pipelineLayout = device->createPipelineLayout( "LineariseDepthPass"
			, *m_lineariseLayout.descriptorLayout );

		m_lineariseLayout.descriptorPool = m_lineariseLayout.descriptorLayout->createPool( "LineariseDepthPass"
			, 1u );
		m_lineariseDescriptor = m_lineariseLayout.descriptorPool->createDescriptorSet( "LineariseDepthPass" );
		m_lineariseDescriptor->createBinding( m_lineariseLayout.descriptorLayout->getBinding( 0u )
			, m_srcDepthBuffer
			, *m_lineariseSampler );
		m_clipInfo.createSizedBinding( *m_lineariseDescriptor
			, m_lineariseLayout.descriptorLayout->getBinding( 1u ) );
		m_lineariseDescriptor->update();

		m_linearisePipeline = device->createPipeline( "LineariseDepthPass"
			, ashes::GraphicsPipelineCreateInfo
			{
				0u,
				lineariseProgram,
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
				*m_lineariseLayout.pipelineLayout,
				*m_renderPass,
			} );
	}

	void LineariseDepthPass::doInitialiseMinifyPass()
	{
		auto & renderSystem = *m_engine.getRenderSystem();
		auto & device = getCurrentRenderDevice( renderSystem );
		auto size = m_result.getTexture()->getDimensions();
		auto minifyProgram = doGetMinifyProgram( m_engine
			, m_minifyVertexShader
			, m_minifyPixelShader );
		ashes::VkDescriptorSetLayoutBindingArray bindings
		{
			makeDescriptorSetLayoutBinding( DepthImgIdx
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ),
			makeDescriptorSetLayoutBinding( PrevLvlUboIdx
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, VK_SHADER_STAGE_FRAGMENT_BIT ),
		};
		m_minifyLayout.descriptorLayout = device->createDescriptorSetLayout( "MinifyDepthPass"
			, std::move( bindings ) );
		m_minifyLayout.pipelineLayout = device->createPipelineLayout( "MinifyDepthPass" 
			, *m_minifyLayout.descriptorLayout );
		m_minifyLayout.descriptorPool = m_minifyLayout.descriptorLayout->createPool( "MinifyDepthPass"
			, MaxMipLevel );
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


		for ( auto & pipeline : m_minifyPipelines )
		{
			m_previousLevel.push_back( m_engine.getUboPools().getBuffer< MinifyConfiguration >( 0u ) );
			auto & previousLevel = m_previousLevel.back();
			auto & data = previousLevel.getData();
			data.previousLevel = index;
			data.textureSize = Point2i{ size.width, size.height };
			size.width >>= 1;
			size.height >>= 1;
			pipeline.sourceView = sourceView;
			viewInfo->subresourceRange.baseMipLevel = index + 1u;
			viewInfo->image = m_result.getTexture()->getTexture();
			pipeline.targetView = m_result.getTexture()->getTexture().createView( viewInfo );
			pipeline.descriptor = m_minifyLayout.descriptorPool->createDescriptorSet();
			pipeline.descriptor->createBinding( m_minifyLayout.descriptorLayout->getBinding( 0u )
				, *pipeline.sourceView
				, *m_minifySampler );
			previousLevel.createSizedBinding( *pipeline.descriptor
				, m_minifyLayout.descriptorLayout->getBinding( 1u ) );
			pipeline.descriptor->update();
			ashes::ImageViewCRefArray attaches;
			attaches.emplace_back( pipeline.targetView );
			pipeline.frameBuffer = m_renderPass->createFrameBuffer( "MinifyDepthPass"
				, VkExtent2D{ size.width, size.height }
				, std::move( attaches ) );

			pipeline.pipeline = device->createPipeline( "MinifyDepthPass"
				, ashes::GraphicsPipelineCreateInfo
				{
					0u,
					minifyProgram,
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
					*m_minifyLayout.pipelineLayout,
					*m_renderPass,
				} );
			sourceView = &pipeline.targetView;
			++index;
		}
	}

	void LineariseDepthPass::doCleanupLinearisePass()
	{
		m_linearisePipeline.reset();
		m_lineariseLayout.pipelineLayout.reset();
		m_lineariseDescriptor.reset();
		m_lineariseLayout.descriptorPool.reset();
		m_lineariseLayout.descriptorLayout.reset();
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

		for ( auto & ubo : m_previousLevel )
		{
			m_engine.getUboPools().putBuffer( ubo );
		}

		m_previousLevel.clear();
		m_minifyLayout.descriptorPool.reset();
		m_minifyLayout.pipelineLayout.reset();
		m_minifyLayout.descriptorLayout.reset();
	}

	void LineariseDepthPass::doPrepareFrame( ashes::CommandBuffer & cb
		, RenderPassTimer const & timer
		, uint32_t index )const
	{
		cb.begin();
		timer.beginPass( cb, index );

		// Linearisation pass.
		cb.beginDebugBlock(
			{
				m_prefix + " - Linearise Depth",
				makeFloatArray( m_engine.getNextRainbowColour() ),
			} );
		cb.memoryBarrier( VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT
			, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
			, m_depthBuffer.makeShaderInputResource( VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL ) );
		cb.beginRenderPass( *m_renderPass
			, *m_lineariseFrameBuffer
			, { transparentBlackClearColor }
			, VK_SUBPASS_CONTENTS_INLINE );
		cb.bindPipeline( *m_linearisePipeline );
		cb.bindDescriptorSet( *m_lineariseDescriptor, *m_lineariseLayout.pipelineLayout );
		cb.bindVertexBuffer( 0u, m_vertexBuffer->getBuffer(), 0u );
		cb.draw( 6u );
		cb.endRenderPass();
		cb.memoryBarrier( VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
			, VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT
			, m_depthBuffer.makeDepthStencilAttachment( VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ) );
		cb.endDebugBlock();

		// Minification passes.
		cb.beginDebugBlock(
			{
				m_prefix + " - Minify",
				makeFloatArray( m_engine.getNextRainbowColour() ),
			} );

		for ( auto & pipeline : m_minifyPipelines )
		{
			cb.beginRenderPass( *m_renderPass
				, *pipeline.frameBuffer
				, { transparentBlackClearColor }
				, VK_SUBPASS_CONTENTS_INLINE );
			cb.bindPipeline( *pipeline.pipeline );
			cb.bindDescriptorSet( *pipeline.descriptor, *m_minifyLayout.pipelineLayout );
			cb.bindVertexBuffer( 0u, m_vertexBuffer->getBuffer(), 0u );
			cb.draw( 6u );
			cb.endRenderPass();
		}

		cb.endDebugBlock();
		timer.endPass( cb, index );
		cb.end();
	}
}
