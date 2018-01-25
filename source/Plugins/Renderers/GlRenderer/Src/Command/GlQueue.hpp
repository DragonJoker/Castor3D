/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#pragma once

#include "GlRendererPrerequisites.hpp"

#include <Command/Queue.hpp>

namespace gl_renderer
{
	/**
	*\brief
	*	Concept de file de commandes.
	*/
	class Queue
		: public renderer::Queue
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*/
		Queue();
		/**
		*\brief
		*	Met en attente des tampons de commandes.
		*\param[in] commandBuffer
		*	Le tampon de commandes.
		*\param[in] fence
		*	Une barrière optionnelle.
		*\return
		*	\p true si tout s'est bien passé.
		*/
		bool submit( renderer::CommandBuffer const & commandBuffer
			, renderer::Fence const * fence )const override;
		/**
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
		*/ 
		bool submit( renderer::CommandBuffer const & commandBuffer
			, renderer::Semaphore const & semaphoreToWait
			, renderer::PipelineStageFlags const & semaphoreStage
			, renderer::Semaphore const & semaphoreToSignal
			, renderer::Fence const * fence )const override;
		/**
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
		*/ 
		bool submit( renderer::CommandBufferCRefArray const & commandBuffers
			, renderer::SemaphoreCRefArray const & semaphoresToWait
			, renderer::PipelineStageFlagsArray const & semaphoresStage
			, renderer::SemaphoreCRefArray const & semaphoresToSignal
			, renderer::Fence const * fence )const override;
		/**
		*\brief
		*	Présente la file à Vulkan.
		*\return
		*	\p true si tout s'est bien passé.
		*/ 
		bool present( renderer::SwapChainCRefArray const & swapChains
			, renderer::UInt32Array const & imagesIndex
			, renderer::SemaphoreCRefArray const & semaphoresToWait )const override;
		/**
		*\brief
		*	Attend que la file soit inactive.
		*\return
		*	\p true si tout s'est bien passé.
		*/
		bool waitIdle()const override;
		/**
		*\return
		*	L'index de la famille de la file.
		*/
		inline uint32_t getFamilyIndex()const override
		{
			return 0u;
		}
	};
}
