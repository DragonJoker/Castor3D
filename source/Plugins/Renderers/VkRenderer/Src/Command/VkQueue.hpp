/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#pragma once

#include "VkRendererPrerequisites.hpp"

#include <Command/Queue.hpp>

namespace vk_renderer
{
	class Queue
		: public renderer::Queue
	{
	public:
		Queue( Device const & device
			, uint32_t familyIndex );
		/**
		*\~french
		*\brief
		*	Présente la file à Vulkan.
		*\return
		*	\p true si tout s'est bien passé.
		*\~english
		*\brief
		*	present the queue to Vulkan.
		*\return
		*	\p true on ok.
		*/
		VkResult presentBackBuffer( SwapChainCRefArray const & swapChains
			, renderer::UInt32Array const & imagesIndex
			, SemaphoreCRefArray const & semaphoresToWait )const;
		/**
		*\copydoc		renderer::Queue::submit
		*/ 
		bool submit( renderer::CommandBufferCRefArray const & commandBuffers
			, renderer::SemaphoreCRefArray const & semaphoresToWait
			, renderer::PipelineStageFlagsArray const & semaphoresStage
			, renderer::SemaphoreCRefArray const & semaphoresToSignal
			, renderer::Fence const * fence )const override;
		/**
		*\copydoc		renderer::Queue::present
		*/
		bool present( renderer::SwapChainCRefArray const & swapChains
			, renderer::UInt32Array const & imagesIndex
			, renderer::SemaphoreCRefArray const & semaphoresToWait )const override;
		/**
		/**
		*\copydoc		renderer::Queue::waitIdle
		*/
		bool waitIdle()const override;
		/**
		/**
		*\copydoc		renderer::Queue::getFamilyIndex
		*/
		inline uint32_t getFamilyIndex()const override
		{
			return m_familyIndex;
		}
		/**
		*\~french
		*\brief
		*	Conversion implicite vers VkQueue.
		*\~english
		*\brief
		*	VkQueue implicit cast operator.
		*/
		inline operator VkQueue const &()const
		{
			return m_queue;
		}

	private:
		Device const & m_device;
		VkQueue m_queue{ VK_NULL_HANDLE };
		uint32_t m_familyIndex{ 0u };
	};
}
