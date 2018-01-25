/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#pragma once

#include "VkRendererPrerequisites.hpp"

#include <Command/Queue.hpp>

namespace vk_renderer
{
	/**
	*\~french
	*\brief
	*	Classe encapsulant une VkQueue.
	*\~english
	*\brief
	*	VkQueue wrapper.
	*/
	class Queue
		: public renderer::Queue
	{
	public:
		/**
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le LogicalDevice parent.
		*\param[in] familyIndex
		*	L'index du type de file.
		*\~english
		*\brief
		*	Constructor.
		*\param[in] device
		*	The logical connection to the GPU.
		*\param[in] familyIndex
		*	The queue's family index.
		*/
		Queue( Device const & device
			, uint32_t familyIndex );
		/**
		*\~french
		*\brief
		*	Met en attente des tampons de commandes.
		*\param[in] commandBuffer
		*	Le tampon de commandes.
		*\param[in] fence
		*	Une barrière optionnelle.
		*\return
		*	\p true si tout s'est bien passé.
		*\~english
		*\brief
		*	Submits given command buffers.
		*\param[in] commandBuffer
		*	The command buffer.
		*\param[in] fence
		*	An optional fence.
		*\return
		*	\p true on ok.
		*/
		bool submit( renderer::CommandBuffer const & commandBuffer
			, renderer::Fence const * fence )const override;
		/**
		*\~french
		*\brief
		*	Met en attente des tampons de commandes.
		*\param[in] commandBuffer
		*	Le tampon de commandes.
		*\param[in] semaphoreToWait
		*	Le sémaphore à attendre lors de l'éxécution de la file.
		*\param[in] semaphoreStage
		*	L'étape du sémaphore.
		*\param[in] semaphoreToSignal
		*	Le sémaphore à signaler à la fin de l'éxécution de la file.
		*\param[in] fence
		*	Une barrière optionnelle.
		*\return
		*	\p true si tout s'est bien passé.
		*\~english
		*\brief
		*	Submits given command buffer.
		*\param[in] commandBuffer
		*	The command buffer.
		*\param[in] semaphoreToWait
		*	The semaphore to wait.
		*\param[in] semaphoreStage
		*	The semaphore stage.
		*\param[in] semaphoreToSignal
		*	The semaphore to signal.
		*\param[in] fence
		*	An optional fence.
		*\return
		*	\p true on ok.
		*/ 
		bool submit( renderer::CommandBuffer const & commandBuffer
			, renderer::Semaphore const & semaphoreToWait
			, renderer::PipelineStageFlags const & semaphoreStage
			, renderer::Semaphore const & semaphoreToSignal
			, renderer::Fence const * fence )const override;
		/**
		*\~french
		*\brief
		*	Met en attente des tampons de commandes.
		*\param[in] commandBuffers
		*	Les tampons de commandes.
		*\param[in] semaphoresToWait
		*	Les sémaphores à attendre lors de l'éxécution de la file.
		*\param[in] semaphoresStage
		*	Les étapes respectives des sémaphores.
		*\param[in] semaphoresToSignal
		*	Les sémaphores à signaler à la fin de l'éxécution de la file.
		*\param[in] fence
		*	Une barrière optionnelle.
		*\return
		*	\p true si tout s'est bien passé.
		*\~english
		*\brief
		*	Submits given command buffers.
		*\param[in] commandBuffers
		*	The command buffers.
		*\param[in] semaphoresToWait
		*	The semaphores to wait.
		*\param[in] semaphoresStage
		*	The semaphores respective stages.
		*\param[in] semaphoresToSignal
		*	The semaphores to signal.
		*\param[in] fence
		*	An optional fence.
		*\return
		*	\p true on ok.
		*/ 
		bool submit( renderer::CommandBufferCRefArray const & commandBuffers
			, renderer::SemaphoreCRefArray const & semaphoresToWait
			, renderer::PipelineStageFlagsArray const & semaphoresStage
			, renderer::SemaphoreCRefArray const & semaphoresToSignal
			, renderer::Fence const * fence )const override;
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
		bool present( renderer::SwapChainCRefArray const & swapChains
			, renderer::UInt32Array const & imagesIndex
			, renderer::SemaphoreCRefArray const & semaphoresToWait )const override;
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
		*\~french
		*\brief
		*	Attend que la file soit inactive.
		*\return
		*	\p true si tout s'est bien passé.
		*\~english
		*\brief
		*	Waits for the queue to be idle.
		*\return
		*	\p true on ok.
		*/
		bool waitIdle()const override;
		/**
		*\~french
		*\return
		*	L'index de la famille de la file.
		*\~english
		*\return
		*	The queue's family index.
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
		inline operator VkQueue const &( )const
		{
			return m_queue;
		}

	private:
		Device const & m_device;
		VkQueue m_queue{ VK_NULL_HANDLE };
		uint32_t m_familyIndex{ 0u };
	};
}
