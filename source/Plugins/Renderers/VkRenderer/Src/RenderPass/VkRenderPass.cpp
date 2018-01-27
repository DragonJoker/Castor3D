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
		, std::vector< renderer::PixelFormat > const & formats
		, renderer::RenderSubpassPtrArray const & subpasses
		, renderer::RenderPassState const & initialState
		, renderer::RenderPassState const & finalState
		, bool clear
		, renderer::SampleCountFlag samplesCount )
		: renderer::RenderPass{ device
			, formats
			, subpasses
			, initialState
			, finalState
			, clear
			, samplesCount }
		, m_device{ device }
		, m_formats{ formats }
		, m_subpasses{ doConvert( subpasses ) }
		, m_samplesCount{ samplesCount }
		, m_initialState{ initialState }
		, m_finalState{ finalState }
	{
		// On crée les attaches pour les tampons de couleur et de profondeur.
		std::vector< VkAttachmentDescription > attachments;
		attachments.reserve( m_formats.size() );
		uint32_t index{ 0 };

		for ( auto const & format : m_formats )
		{
			if ( renderer::isDepthStencilFormat( format )
				|| renderer::isDepthFormat( format )
				|| renderer::isStencilFormat( format ) )
			{
				attachments.push_back(
				{
					0u,                                             // flags
					convert( format ),                              // format
					convert( m_samplesCount ),                      // samples
					clear                                           // loadOp
						? VK_ATTACHMENT_LOAD_OP_CLEAR
						: VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					VK_ATTACHMENT_STORE_OP_STORE,                   // storeOp
					clear                                           // stencilLoadOp
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
					convert( format ),                              // format
					convert( m_samplesCount ),                      // samples
					clear                                           // loadOp
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

		auto res = vk::CreateRenderPass( m_device, &renderPassInfo, nullptr, &m_renderPass );

		if ( !checkError( res ) )
		{
			throw std::runtime_error{ "Render pass creation failed: " + getLastError() };
		}
	}

	RenderPass::~RenderPass()
	{
		vk::DestroyRenderPass( m_device, m_renderPass, nullptr );
	}

	renderer::FrameBufferPtr RenderPass::createFrameBuffer( renderer::UIVec2 const & dimensions
		, renderer::TextureAttachmentPtrArray && textures )const
	{
		// On vérifie la compatibilité des vues demandés pour le framebuffer à créer.
		auto it = std::find_if( textures.begin()
			, textures.end()
			, [this]( renderer::TextureAttachmentPtr const & attach )
		{
			return m_formats.end() == std::find_if( m_formats.begin()
				, m_formats.end()
				, [&attach]( auto format )
			{
				return format == attach->getFormat();
			} );
		} );

		if ( it != textures.end() )
		{
			throw std::runtime_error{ "Incompatible views for framebuffer creation from this render pass." };
		}

		// On crée le framebuffer.
		return std::make_unique< FrameBuffer >( m_device
			, *this
			, dimensions
			, std::move( textures ) );
	}

	std::vector< VkClearValue > const & RenderPass::getClearValues (renderer::ClearValueArray const & clearValues)const
	{
		m_clearValues = convert< VkClearValue >( clearValues );
		return m_clearValues;
	}
}
