/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "RenderPass/VkRenderPass.hpp"

#include "Command/VkCommandBuffer.hpp"
#include "Core/VkDevice.hpp"
#include "Image/VkTexture.hpp"
#include "Image/VkTextureView.hpp"
#include "RenderPass/VkFrameBuffer.hpp"
#include "RenderPass/VkRenderPassState.hpp"
#include "RenderPass/VkRenderSubpass.hpp"
#include "RenderPass/VkRenderSubpassState.hpp"

#include <algorithm>

namespace vk_renderer
{
	namespace
	{
		RenderSubpassCRefArray doConvert( renderer::RenderSubpassPtrArray const & subpasses )
		{
			RenderSubpassCRefArray result;

			for ( auto & subpass : subpasses )
			{
				result.emplace_back( static_cast< RenderSubpass const & >( *subpass ) );
			}

			return result;
		}
	}

	RenderPass::RenderPass( Device const & device
		, renderer::RenderPassAttachmentArray const & attaches
		, renderer::RenderSubpassPtrArray && subpasses
		, renderer::RenderPassState const & initialState
		, renderer::RenderPassState const & finalState
		, renderer::SampleCountFlag samplesCount )
		: renderer::RenderPass{ device
			, attaches
			, std::move( subpasses )
			, initialState
			, finalState
			, samplesCount }
		, m_device{ device }
		, m_subpasses{ doConvert( renderer::RenderPass::getSubpasses() ) }
		, m_samplesCount{ samplesCount }
		, m_initialState{ initialState }
		, m_finalState{ finalState }
	{
		// On crée les attaches pour les tampons de couleur et de profondeur.
		std::vector< VkAttachmentDescription > attachments;
		attachments.reserve( getSize() );
		uint32_t index{ 0 };

		for ( auto const & attach : *this )
		{
			if ( renderer::isDepthOrStencilFormat( attach.getFormat() ) )
			{
				attachments.push_back(
				{
					0u,                                             // flags
					convert( attach.getFormat() ),                  // format
					convert( m_samplesCount ),                      // samples
					attach.getClear()                               // loadOp
						? VK_ATTACHMENT_LOAD_OP_CLEAR
						: VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					VK_ATTACHMENT_STORE_OP_STORE,                   // storeOp
					attach.getClear()                               // stencilLoadOp
						? VK_ATTACHMENT_LOAD_OP_CLEAR
						: VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					VK_ATTACHMENT_STORE_OP_STORE,                   // stencilStoreOp
					convert( *( m_initialState.begin() + index ) ), // initialLayout
					convert( *( m_finalState.begin() + index ) )    // finalLayout
				} );
			}
			else
			{
				attachments.push_back(
				{
					0u,                                             // flags
					convert( attach.getFormat() ),                  // format
					convert( m_samplesCount ),                      // samples
					attach.getClear()                               // loadOp
						? VK_ATTACHMENT_LOAD_OP_CLEAR
						: VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					VK_ATTACHMENT_STORE_OP_STORE,                   // storeOp
					VK_ATTACHMENT_LOAD_OP_DONT_CARE,                // stencilLoadOp
					VK_ATTACHMENT_STORE_OP_DONT_CARE,               // stencilStoreOp
					convert( *( m_initialState.begin() + index ) ), // initialLayout
					convert( *( m_finalState.begin() + index ) )    // finalLayout
				} );
			}

			++index;
		}

		assert( !m_subpasses.empty() && "Can't create a render pass without subpass." );

		std::vector< VkSubpassDescription > descriptions;
		descriptions.reserve( m_subpasses.size() );
		std::vector< VkSubpassDependency > dependencies;
		dependencies.reserve( 1 + m_subpasses.size() );
		RenderSubpassState currentState
		{
			convert( m_initialState.getPipelineStage() ),
			convert( m_initialState.getAccess() )
		};
		uint32_t subpassIndex = VK_SUBPASS_EXTERNAL;

		for ( auto const & subpass : m_subpasses )
		{
			auto const & state = subpass.get().getNeededState();

			if ( currentState.m_pipelineStage != convert( state.getPipelineStage() )
				|| currentState.m_access != convert( state.getAccess() ) )
			{
				dependencies.push_back(
				{
					subpassIndex,                                    // srcSubpass
					subpassIndex + 1,                                // dstSubpass
					currentState.m_pipelineStage,                    // srcStageMask
					convert( state.getPipelineStage() ),             // dstStageMask
					currentState.m_access,                           // srcAccessMask
					convert( state.getAccess() ),                    // dstAccessMask
					VK_DEPENDENCY_BY_REGION_BIT                      // dependencyFlags
				} );
			}

			currentState = convert( state );
			++subpassIndex;
			descriptions.push_back( subpass.get() );
		}

		if ( currentState.m_pipelineStage != convert( m_finalState.getPipelineStage() )
			|| currentState.m_access != convert( m_finalState.getAccess() ) )
		{
			dependencies.push_back(
			{
				subpassIndex,                                        // srcSubpass
				VK_SUBPASS_EXTERNAL,                                 // dstSubpass
				currentState.m_pipelineStage,                        // srcStageMask
				convert( m_finalState.getPipelineStage() ),          // dstStageMask
				currentState.m_access,                               // srcAccessMask
				convert( m_finalState.getAccess() ),                 // dstAccessMask
				VK_DEPENDENCY_BY_REGION_BIT                          // dependencyFlags
			} );
		}

		// Enfin on crée la passe
		VkRenderPassCreateInfo renderPassInfo
		{
			VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
			nullptr,
			0u,                                                    // flags
			static_cast< uint32_t >( attachments.size() ),         // attachmentCount
			attachments.empty() ? nullptr : attachments.data(),    // pAttachments
			static_cast< uint32_t >( descriptions.size() ),        // subpassCount
			descriptions.empty() ? nullptr : descriptions.data(),  // pSubpasses
			static_cast< uint32_t >( dependencies.size() ),        // dependencyCount
			dependencies.empty() ? nullptr : dependencies.data(),  // pDependencies
		};
		DEBUG_DUMP( renderPassInfo );

		auto res = m_device.vkCreateRenderPass( m_device, &renderPassInfo, nullptr, &m_renderPass );

		if ( !checkError( res ) )
		{
			throw std::runtime_error{ "Render pass creation failed: " + getLastError() };
		}
	}

	RenderPass::~RenderPass()
	{
		m_device.vkDestroyRenderPass( m_device, m_renderPass, nullptr );
	}

	renderer::FrameBufferPtr RenderPass::createFrameBuffer( renderer::UIVec2 const & dimensions
		, renderer::FrameBufferAttachmentArray && attaches )const
	{
		return std::make_unique< FrameBuffer >( m_device
			, *this
			, dimensions
			, std::move( attaches ) );
	}

	std::vector< VkClearValue > const & RenderPass::getClearValues (renderer::ClearValueArray const & clearValues)const
	{
		m_clearValues = convert< VkClearValue >( clearValues );
		return m_clearValues;
	}
}
