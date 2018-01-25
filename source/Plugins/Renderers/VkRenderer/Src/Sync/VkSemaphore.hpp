/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#pragma once

#include "VkRendererPrerequisites.hpp"

#include <Sync/Semaphore.hpp>

namespace vk_renderer
{
	/**
	*\~french
	*\brief
	*	Classe encapsulant un VkSemaphore.
	*\remarks
	*	Un sémaphore est un élément de synchronisation servant pour les files.
	*\~english
	*\brief
	*	VkSemaphore wrapper.
	*\remarks
	*	Semaphores are used to synchronise queues.
	*/
	class Semaphore
		: public renderer::Semaphore
	{
	public:
		/**
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le LogicalDevice parent.
		*\~english
		*\brief
		*	Constructor.
		*\param[in] device
		*	The logical connection to the GPU.
		*/
		explicit Semaphore( Device const & device );
		/**
		*\~french
		*\brief
		*	Destructeur.
		*\~english
		*\brief
		*	Destructor.
		*/
		~Semaphore();
		/**
		*\~french
		*\brief
		*	Conversion implicite vers VkSemaphore.
		*\~english
		*\brief
		*	VkSemaphore implicit cast operator.
		*/
		inline operator VkSemaphore const &( )const
		{
			return m_semaphore;
		}

	private:
		Device const & m_device;
		VkSemaphore m_semaphore{ VK_NULL_HANDLE };
	};
}
