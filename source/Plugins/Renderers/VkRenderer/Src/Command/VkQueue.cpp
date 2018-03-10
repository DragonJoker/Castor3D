/*
This file belongs to RendererLib.
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
		m_device.vkGetDeviceQueue( m_device, familyIndex, 0, &m_queue );
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
		return m_device.vkQueuePresentKHR( m_queue, &presentInfo );
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
				vksemaphoresToWait.empty()                                 // pWaitSemaphores
					? nullptr
					: vksemaphoresToWait.data(),
				vksemaphoresStage.empty()                                  // pWaitDstStageMask
					? nullptr
					: vksemaphoresStage.data(),
				static_cast< uint32_t >( vkcommandBuffers.size() ),        // commandBufferCount
				vkcommandBuffers.empty()                                   // pCommandBuffers
					? nullptr
					: vkcommandBuffers.data(),
				static_cast< uint32_t >( vksemaphoresToSignal.size() ),    // signalSemaphoreCount
				vksemaphoresToSignal.empty()                               // pSignalSemaphores
					? nullptr
					: vksemaphoresToSignal.data()
			}
		};
		DEBUG_DUMP( submitInfo );
		auto res = m_device.vkQueueSubmit( m_queue
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

	bool Queue::waitIdle()const
	{
		auto res = m_device.vkQueueWaitIdle( m_queue );
		return checkError( res );
	}
}
