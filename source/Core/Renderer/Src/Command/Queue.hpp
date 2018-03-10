/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_Queue_HPP___
#define ___Renderer_Queue_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	/**
	*\~french
	*\brief
	*	File de tampons de commandes.
	*\~english
	*\brief
	*	Command buffers queue.
	*/
	class Queue
	{
	protected:
		Queue();

	public:
		virtual ~Queue() = default;
		/**
		*\~french
		*\brief
		*	Met des tampons de commandes dans la file.
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
		virtual bool submit( CommandBufferCRefArray const & commandBuffers
			, SemaphoreCRefArray const & semaphoresToWait
			, PipelineStageFlagsArray const & semaphoresStage
			, SemaphoreCRefArray const & semaphoresToSignal
			, Fence const * fence )const = 0;
		/**
		*\~french
		*\brief
		*	Présente la file à l'API de rendu.
		*\return
		*	\p true si tout s'est bien passé.
		*\~english
		*\brief
		*	Presents the queue to the rendering API.
		*\return
		*	\p true on ok.
		*/ 
		virtual bool present( SwapChainCRefArray const & swapChains
			, UInt32Array const & imagesIndex
			, SemaphoreCRefArray const & semaphoresToWait )const = 0;
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
		virtual bool waitIdle()const = 0;
		/**
		*\~french
		*\return
		*	L'index de la famille de la file.
		*\~english
		*\return
		*	The queue family index.
		*/
		virtual uint32_t getFamilyIndex()const = 0;
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
		*	Submits given command buffer.
		*\param[in] commandBuffer
		*	The command buffer.
		*\param[in] fence
		*	An optional fence.
		*\return
		*	\p true on ok.
		*/
		inline bool submit( CommandBuffer const & commandBuffer
			, Fence const * fence )const
		{
			return submit( { commandBuffer }
				, SemaphoreCRefArray{}
				, PipelineStageFlagsArray{}
				, SemaphoreCRefArray{}
				, fence );
		}
		/**
		*\~french
		*\brief
		*	Met en attente un tampon de commandes.
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
		*	The semaphore respective stages.
		*\param[in] semaphoreToSignal
		*	The semaphore to signal.
		*\param[in] fence
		*	An optional fence.
		*\return
		*	\p true on ok.
		*/
		inline bool submit( CommandBuffer const & commandBuffer
			, Semaphore const & semaphoreToWait
			, PipelineStageFlags const & semaphoreStage
			, Semaphore const & semaphoreToSignal
			, Fence const * fence )const
		{
			return submit( { commandBuffer }
				, SemaphoreCRefArray{ semaphoreToWait }
				, PipelineStageFlagsArray{ semaphoreStage }
				, SemaphoreCRefArray{ semaphoreToSignal }
				, fence );
		}
	};
}

#endif
