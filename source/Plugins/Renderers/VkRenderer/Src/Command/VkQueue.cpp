/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "Command/VkQueue.hpp"

#include "Command/VkCommandBuffer.hpp"
#include "Core/VkDevice.hpp"
#include "Core/VkSwapChain.hpp"
#include "Sync/VkFence.hpp"
#include "Sync/VkSemaphore.hpp"

namespace vk_renderer
{
	namespace
	{
		CommandBufferCRefArray convert( renderer::CommandBufferCRefArray const & values )
		{
			CommandBufferCRefArray result;
			result.reserve( values.size() );

			for ( auto & value : values )
			{
				result.emplace_back( static_cast< CommandBuffer const & >( value.get() ) );
			}

			return result;
		}

		SemaphoreCRefArray convert( renderer::SemaphoreCRefArray const & values )
		{
			SemaphoreCRefArray result;
			result.reserve( values.size() );

			for ( auto & value : values )
			{
				result.emplace_back( static_cast< Semaphore const & >( value.get() ) );
			}

			return result;
		}

		SwapChainCRefArray convert( renderer::SwapChainCRefArray const & values )
		{
			SwapChainCRefArray result;
			result.reserve( values.size() );

			for ( auto & value : values )
			{
				result.emplace_back( static_cast< SwapChain const & >( value.get() ) );
			}

			return result;
		}
	}

	Queue::Queue( Device const & device
		, uint32_t familyIndex )
		: m_device{ device }
		, m_familyIndex{ familyIndex }
	{
		vk::GetDeviceQueue( m_device, familyIndex, 0, &m_queue );
	}

	bool Queue::submit( renderer::CommandBuffer const & commandBuffer
		, renderer::Fence const * fence )const
	{
		VkSubmitInfo submitInfo
		{
			VK_STRUCTURE_TYPE_SUBMIT_INFO,
			nullptr,
			0u,                                                                         // waitSemaphoreCount
			nullptr,                                                                    // pWaitSemaphores
			nullptr,                                                                    // pWaitDstStageMask
			1u,                                                                         // commandBufferCount
			&static_cast< VkCommandBuffer const & >( static_cast< CommandBuffer const & >( commandBuffer ) ),  // pCommandBuffers
			0u,                                                                         // signalSemaphoreCount
			nullptr                                                                     // pSignalSemaphores
		};
		DEBUG_DUMP( submitInfo );
		auto res = vk::QueueSubmit( m_queue
			, 1u
			, &submitInfo
			, fence ? static_cast< VkFence const & >( *static_cast< Fence const * >( fence ) ) : VK_NULL_HANDLE );
		return checkError( res );
	}

	bool Queue::submit( renderer::CommandBuffer const & commandBuffer
		, renderer::Semaphore const & semaphoreToWait
		, renderer::PipelineStageFlags const & semaphoreStage
		, renderer::Semaphore const & semaphoreToSignal
		, renderer::Fence const * fence )const
	{
		auto vkSemaphoreStage = convert( semaphoreStage );
		VkSubmitInfo submitInfo
		{
			VK_STRUCTURE_TYPE_SUBMIT_INFO,
			nullptr,
			1u,                                                        // waitSemaphoreCount
			&static_cast< VkSemaphore const & >( static_cast< Semaphore const & >( semaphoreToWait ) ),    // pWaitSemaphores
			&vkSemaphoreStage,                                         // pWaitDstStageMask
			1u,                                                        // commandBufferCount
			&static_cast< VkCommandBuffer const & >( static_cast< CommandBuffer const & >( commandBuffer ) ),  // pCommandBuffers
			1u,                                                        // signalSemaphoreCount
			&static_cast< VkSemaphore const & >( static_cast< Semaphore const & >( semaphoreToSignal ) )   // pSignalSemaphores
		};
		DEBUG_DUMP( submitInfo );
		auto res = vk::QueueSubmit( m_queue
			, 1u
			, &submitInfo
			, fence ? static_cast< VkFence const & >( *static_cast< Fence const * >( fence ) ) : VK_NULL_HANDLE );
		return checkError( res );
	}

	bool Queue::submit( renderer::CommandBufferCRefArray const & commandBuffers
		, renderer::SemaphoreCRefArray const & semaphoresToWait
		, renderer::PipelineStageFlagsArray const & semaphoresStage
		, renderer::SemaphoreCRefArray const & semaphoresToSignal
		, renderer::Fence const * fence )const
	{
		auto vkcommandBuffers = makeVkArray< VkCommandBuffer >( convert( commandBuffers ) );
		auto vksemaphoresToWait = makeVkArray< VkSemaphore >( convert( semaphoresToWait ) );
		auto vksemaphoresToSignal = makeVkArray< VkSemaphore >( convert( semaphoresToSignal ) );
		auto vksemaphoresStage = convert< VkPipelineStageFlags >( semaphoresStage );

		std::vector< VkSubmitInfo > submitInfo
		{
			{
				VK_STRUCTURE_TYPE_SUBMIT_INFO,
				nullptr,
				static_cast< uint32_t >( vksemaphoresToWait.size() ),      // waitSemaphoreCount
				vksemaphoresToWait.data(),                                 // pWaitSemaphores
				vksemaphoresStage.data(),                                  // pWaitDstStageMask
				static_cast< uint32_t >( vkcommandBuffers.size() ),        // commandBufferCount
				vkcommandBuffers.data(),                                   // pCommandBuffers
				static_cast< uint32_t >( vksemaphoresToSignal.size() ),    // signalSemaphoreCount
				vksemaphoresToSignal.data()                                // pSignalSemaphores
			}
		};
		DEBUG_DUMP( submitInfo );
		auto res = vk::QueueSubmit( m_queue
			, static_cast< uint32_t >( submitInfo.size() )
			, submitInfo.data()
			, fence ? static_cast< VkFence const & >( *static_cast< Fence const * >( fence ) ) : VK_NULL_HANDLE );
		return checkError( res );
	}

	bool Queue::present( renderer::SwapChainCRefArray const & swapChains
		, renderer::UInt32Array const & imagesIndex
		, renderer::SemaphoreCRefArray const & semaphoresToWait )const
	{
		return checkError( presentBackBuffer( convert( swapChains )
			, imagesIndex
			, convert( semaphoresToWait ) ) );
	}

	VkResult Queue::presentBackBuffer( SwapChainCRefArray const & swapChains
		, renderer::UInt32Array const & imagesIndex
		, SemaphoreCRefArray const & semaphoresToWait )const
	{
		assert( swapChains.size() == imagesIndex.size() );
		auto vkswapchains = makeVkArray< VkSwapchainKHR >( swapChains );
		auto vksemaphoresToWait = makeVkArray< VkSemaphore >( semaphoresToWait );
		VkPresentInfoKHR presentInfo
		{
			VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
			nullptr,
			static_cast< uint32_t >( vksemaphoresToWait.size() ),    // waitSemaphoreCount
			vksemaphoresToWait.data(),                               // pWaitSemaphores
			static_cast< uint32_t >( imagesIndex.size() ),           // swapchainCount
			vkswapchains.data(),                                     // pSwapchains
			imagesIndex.data(),                                      // pImageIndices
			nullptr                                                  // pResults
		};
		DEBUG_DUMP( presentInfo );
		return vk::QueuePresentKHR( m_queue, &presentInfo );
	}

	bool Queue::waitIdle()const
	{
		auto res = vk::QueueWaitIdle( m_queue );
		return checkError( res );
	}
}
