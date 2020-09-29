#include "Castor3D/Render/Passes/CombinePass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
#include "Castor3D/Cache/SamplerCache.hpp"
#include "Castor3D/Model/Vertex.hpp"
#include "Castor3D/Miscellaneous/Parameter.hpp"
#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"
#include "Castor3D/Render/RenderLoop.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"

#include <CastorUtils/Graphics/Image.hpp>

#include <ashespp/Buffer/VertexBuffer.hpp>
#include <ashespp/Image/Image.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>
#include <ashespp/RenderPass/SubpassDescription.hpp>

#include <ShaderWriter/Source.hpp>

#include <numeric>

using namespace castor;

namespace castor3d
{
	namespace
	{
		TextureLayoutSPtr doCreateTexture( RenderSystem & renderSystem
			, RenderDevice const & device
			, castor::String const & prefix
			, VkExtent2D const & size
			, VkFormat format )
		{
			ashes::ImageCreateInfo image
			{
				0u,
				VK_IMAGE_TYPE_2D,
				format,
				{ size.width, size.height, 1u },
				1u,
				1u,
				VK_SAMPLE_COUNT_1_BIT,
				VK_IMAGE_TILING_OPTIMAL,
				( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
					| VK_IMAGE_USAGE_SAMPLED_BIT
					| VK_IMAGE_USAGE_TRANSFER_SRC_BIT ),
			};
			auto texture = std::make_shared< TextureLayout >( renderSystem
				, image
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, prefix + cuT( "CombineResult" ) );
			texture->initialise( device );
			return texture;
		}

		ashes::ImageView doCreateView( ashes::Image const & texture )
		{
			ashes::ImageViewCreateInfo createInfo
			{
				0u,
				texture,
				VK_IMAGE_VIEW_TYPE_2D,
				texture.getFormat(),
				VkComponentMapping{},
				{ ashes::getAspectMask( texture.getFormat() ), 0u, 1u, 0u, 1u },
			};
			return texture.createView( std::move( createInfo ) );
		}

		ashes::ImageView doCreateBarrierView( IntermediateView const & view )
		{
			ashes::ImageViewCreateInfo createInfo
			{
				view.view->flags,
				view.view->image,
				view.view->viewType,
				view.view->format,
				view.view->components,
				{
					VkImageAspectFlags( ashes::isDepthStencilFormat( view.view->format )
						? VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT
						: ( ashes::isDepthFormat( view.view->format )
							? VK_IMAGE_ASPECT_DEPTH_BIT
							: ( ashes::isStencilFormat( view.view->format )
								? VK_IMAGE_ASPECT_STENCIL_BIT
								: VK_IMAGE_ASPECT_COLOR_BIT ) ) ),
					view.view->subresourceRange.baseMipLevel,
					view.view->subresourceRange.levelCount,
					view.view->subresourceRange.baseArrayLayer,
					view.view->subresourceRange.layerCount,
				},
			};
			return view.view.image->createView( std::move( createInfo ) );
		}

		ashes::ImageViewArray doCreateBarrierViews( IntermediateViewArray const & views )
		{
			ashes::ImageViewArray result;

			for ( auto & view : views )
			{
				result.push_back( doCreateBarrierView( view ) );
			}

			return result;
		}

		IntermediateView doCreateSampledView( IntermediateView const & view )
		{
			ashes::ImageViewCreateInfo createInfo
			{
				view.view->flags,
				view.view->image,
				view.view->viewType,
				view.view->format,
				view.view->components,
				{
					VkImageAspectFlags( ( ashes::isDepthFormat( view.view->format ) || ashes::isDepthOrStencilFormat( view.view->format ) )
						? VK_IMAGE_ASPECT_DEPTH_BIT
						: ( ashes::isStencilFormat( view.view->format )
							? VK_IMAGE_ASPECT_STENCIL_BIT
							: VK_IMAGE_ASPECT_COLOR_BIT ) ),
					view.view->subresourceRange.baseMipLevel,
					view.view->subresourceRange.levelCount,
					view.view->subresourceRange.baseArrayLayer,
					view.view->subresourceRange.layerCount,
				},
			};
			return
			{
				view.name,
				view.view.image->createView( std::move( createInfo ) ),
				view.layout,
				view.factors,
			};
		}

		IntermediateViewArray doCreateSampledViews( IntermediateViewArray const & views )
		{
			IntermediateViewArray result;

			for ( auto & view : views )
			{
				result.push_back( doCreateSampledView( view ) );
			}

			return result;
		}

		ashes::RenderPassPtr doCreateRenderPass( RenderDevice const & device
			, castor::String const & prefix
			, VkFormat format )
		{
			ashes::VkAttachmentDescriptionArray attaches
			{
				{
					0u,
					format,
					VK_SAMPLE_COUNT_1_BIT,
					VK_ATTACHMENT_LOAD_OP_CLEAR,
					VK_ATTACHMENT_STORE_OP_STORE,
					VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					VK_ATTACHMENT_STORE_OP_DONT_CARE,
					VK_IMAGE_LAYOUT_UNDEFINED,
					VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
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
					ashes::nullopt,
					{},
				} );
			ashes::VkSubpassDependencyArray dependencies
			{
				{
					VK_SUBPASS_EXTERNAL,
					0u,
					VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
					VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					VK_ACCESS_SHADER_READ_BIT,
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
			auto result = device->createRenderPass( prefix + "Combine"
				, std::move( createInfo ) );
			return result;
		}

		ashes::FrameBufferPtr doCreateFrameBuffer( castor::String const & prefix
			, ashes::RenderPass const & renderPass
			, ashes::ImageView const & view
			, VkExtent2D const & size )
		{
			ashes::ImageViewCRefArray attachments{ view };
			return renderPass.createFrameBuffer( prefix + "Combine"
				, size
				, std::move( attachments ) );
		}
	}

	//*********************************************************************************************

	CombinePass::CombineQuad::CombineQuad( Engine & engine
		, RenderDevice const & device
		, castor::String const & prefix
		, IntermediateViewArray const & lhsViews
		, UniformBufferOffsetT< uint32_t > const & indexUbo
		, RenderQuadConfig const & config )
		: RenderQuad{ *engine.getRenderSystem(), device, cuT( "Combine" ), VK_FILTER_LINEAR, config }
		, m_lhsViews{ lhsViews }
		, m_indexUbo{ indexUbo }
		, m_lhsSampler{ createSampler( engine, prefix + cuT( "Combine" ), VK_FILTER_LINEAR, &m_lhsViews[0].view->subresourceRange ) }
	{
		m_lhsSampler->initialise( m_device );
	}

	void CombinePass::CombineQuad::doFillDescriptorSet( ashes::DescriptorSetLayout & descriptorSetLayout
		, ashes::DescriptorSet & descriptorSet )
	{
		uint32_t index = 0u;
		m_indexUbo.createSizedBinding( descriptorSet
			, descriptorSetLayout.getBinding( 0u ) );

		for ( auto & lhsView : m_lhsViews )
		{
			descriptorSet.createBinding( descriptorSetLayout.getBinding( 1u )
				, lhsView.view
				, m_lhsSampler->getSampler()
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				, index++ );
		}
	}

	//*********************************************************************************************

	String const CombinePass::LhsMaps = cuT( "c3d_mapsLhs" );
	String const CombinePass::RhsMap = cuT( "c3d_mapRhs" );

	CombinePass::CombinePass( Engine & engine
		, RenderDevice const & device
		, castor::String const & prefix
		, VkFormat outputFormat
		, VkExtent2D const & outputSize
		, ShaderModule const & vertexShader
		, ShaderModule const & pixelShader
		, IntermediateViewArray const & lhsViews
		, IntermediateView const & rhsView
		, CombinePassConfig config )
		: m_engine{ engine }
		, m_vertexShader{ vertexShader }
		, m_pixelShader{ pixelShader }
		, m_lhsBarrierViews{ doCreateBarrierViews( lhsViews ) }
		, m_lhsViews{ doCreateSampledViews( lhsViews ) }
		, m_rhsBarrierView{ doCreateBarrierView( rhsView ) }
		, m_rhsView{ doCreateSampledView( rhsView ) }
		, m_config{ config }
		, m_prefix{ prefix }
		, m_image{ ( config.resultTexture
			? std::move( *config.resultTexture )
			: doCreateTexture( *engine.getRenderSystem(), prefix, outputSize, outputFormat ) ) }
		, m_view{ doCreateView( m_image->getTexture() ) }
		, m_timer{ std::make_shared< RenderPassTimer >( m_engine, device, m_prefix, cuT( "Combine" ) ) }
		, m_renderPass{ doCreateRenderPass( device, m_prefix, outputFormat ) }
		, m_frameBuffer{ doCreateFrameBuffer( m_prefix, *m_renderPass, m_view, outputSize ) }
		, m_indexUbo{ getCurrentRenderDevice( engine ).uboPools->getBuffer< uint32_t >( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT ) }
		, m_quad{ engine, m_prefix, m_lhsViews, m_indexUbo, std::move( config ) }
	{
		ashes::PipelineShaderStageCreateInfoArray program
		{
			makeShaderState( device, m_vertexShader ),
			makeShaderState( device, m_pixelShader ),
		};
		ashes::VkDescriptorSetLayoutBindingArray bindings
		{
			makeDescriptorSetLayoutBinding( 0u
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, VK_SHADER_STAGE_FRAGMENT_BIT ),
			makeDescriptorSetLayoutBinding( 1u
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT
				, uint32_t( m_lhsViews.size() ) ),
		};
		m_quad.createPipeline( outputSize
			, Position{}
			, program
			, m_rhsView.view
			, *m_renderPass
			, bindings
			, {} );
		auto commands = getCommands( *m_timer, 0u );
		m_commandBuffer = std::move( commands.commandBuffer );
		m_finished = std::move( commands.semaphore );
	}

	CombinePass::~CombinePass()
	{
		getCurrentRenderDevice( m_engine ).uboPools->putBuffer( m_indexUbo );
	}

	void CombinePass::accept( PipelineVisitorBase & visitor )
	{
		visitor.visit( m_vertexShader );
		visitor.visit( m_pixelShader );
	}

	void CombinePass::update( CpuUpdater & updater )
	{
		m_indexUbo.getData() = updater.combineIndex;
	}

	ashes::Semaphore const & CombinePass::combine( ashes::Semaphore const & toWait )const
	{
		//RenderPassTimerBlock timerBlock{ m_timer->start() };
		//timerBlock->notifyPassRender();
		auto & device = m_quad.getDevice();
		auto * result = &toWait;

		device.graphicsQueue->submit( *m_commandBuffer
			, *result
			, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
			, *m_finished
			, nullptr );
		result = m_finished.get();

		return *result;
	}

	CommandsSemaphore CombinePass::getCommands( RenderPassTimer const & timer
		, uint32_t index )const
	{
		auto & device = m_quad.getDevice();
		CommandsSemaphore result
		{
			device.graphicsCommandPool->createCommandBuffer( m_prefix + "CombinePass" ),
			device->createSemaphore( m_prefix + "CombinePass" ),
		};
		auto & cmd = *result.commandBuffer;

		cmd.begin();
		//timer.beginPass( cmd, index );
		cmd.beginDebugBlock(
			{
				m_prefix + " - Combine pass",
				makeFloatArray( device.renderSystem.getEngine()->getNextRainbowColour() ),
			} );

		auto it = m_lhsViews.begin();
		for ( auto & lhsView : m_lhsBarrierViews )
		{
			cmd.memoryBarrier( ashes::getStageMask( it->layout )
				, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
				, lhsView.makeLayoutTransition( it->layout
					, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
					, VK_QUEUE_FAMILY_IGNORED
					, VK_QUEUE_FAMILY_IGNORED ) );
			++it;
		}

		cmd.memoryBarrier( ashes::getStageMask( m_rhsView.layout )
			, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
			, m_rhsBarrierView.makeLayoutTransition( m_rhsView.layout
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				, VK_QUEUE_FAMILY_IGNORED
				, VK_QUEUE_FAMILY_IGNORED ) );

		cmd.beginRenderPass( *m_renderPass
			, *m_frameBuffer
			, { transparentBlackClearColor }
			, VK_SUBPASS_CONTENTS_INLINE );
		m_quad.registerFrame( cmd );
		cmd.endRenderPass();

		it = m_lhsViews.begin();
		for ( auto & lhsView : m_lhsBarrierViews )
		{
			cmd.memoryBarrier( VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
				, ashes::getStageMask( it->layout )
				, lhsView.makeLayoutTransition( VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
					, it->layout
					, VK_QUEUE_FAMILY_IGNORED
					, VK_QUEUE_FAMILY_IGNORED ) );
			++it;
		}

		cmd.memoryBarrier( VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
			, ashes::getStageMask( m_rhsView.layout )
			, m_rhsBarrierView.makeLayoutTransition( VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				, m_rhsView.layout
				, VK_QUEUE_FAMILY_IGNORED
				, VK_QUEUE_FAMILY_IGNORED ) );
		
		cmd.endDebugBlock();
		//timer.endPass( cmd, index );
		cmd.end();

		return result;
	}
}
