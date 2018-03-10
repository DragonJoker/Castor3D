/*
This file belongs to RendererLib.
See LICENSE file in root folder
*/
#pragma once

#include "VkRendererPrerequisites.hpp"

#include <Sync/Fence.hpp>

namespace vk_renderer
{
	/**
	*\~french
	*\brief
	*	Classe permettant la synchronisation des opérations sur une file.
	*\~english
	*\brief
	*	Class allowing synchronisation operations on a queue.
	*/
	class Fence
		: public renderer::Fence
	{
	public:
		/**
		*\~french
		*\brief
		*	Constructeur
		*\param[in] device
		*	Le device parent.
		*\param[in] flags
		*	Les indicateurs de création de la barrière.
		*\~english
		*\brief
		*	Constructor.
		*\param[in] device
		*	The logical connection to the GPU.
		*\param[in] flags
		*	The fence creation flags.
		*/
		Fence( Device const & device
			, renderer::FenceCreateFlags flags = 0 );
		/**
		*\~french
		*\brief
		*	Destructeur.
		*\~english
		*\brief
		*	Destructor.
		*/
		~Fence();
		/**
		*\~french
		*\brief
		*	Attend que la barrière soit signalée.
		*\param[in] timeout
		*	Le temps à attendre pour le signalement.
		*\return
		*	\p WaitResult::eSuccess ou \p WaitResult::eTimeOut en cas de succès.
		*\~english
		*\brief
		*	Waits for the fence to be signaled.
		*\param[in] timeout
		*	The time to wait for the signal.
		*\return
		*	\p WaitResult::eSuccess or \p WaitResult::eTimeOut on success.
		*/ 
		renderer::WaitResult wait( uint32_t timeout )const override;
		/**
		*\~french
		*\brief
		*	Remet la barrière en non signalée.
		*\~english
		*\brief
		*	Resets the fence back to unsignaled.
		*/
		void reset()const override;
		/**
		*\~french
		*\brief
		*	Conversion implicite vers VkFence.
		*\~english
		*\brief
		*	VkFence implicit cast operator.
		*/
		inline operator VkFence const &()const
		{
			return m_fence;
		}

	private:
		Device const & m_device;
		VkFence m_fence{ VK_NULL_HANDLE };
	};
}
