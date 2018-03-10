/*
This file belongs to RendererLib.
See LICENSE file in root folder
*/
#pragma once

#include "VkRendererPrerequisites.hpp"

#include <Command/CommandPool.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Encapsulation d'un VkCommandPool.
	*\~english
	*\brief
	*	VkCommandPool wrapper.
	*/
	class CommandPool
		: public renderer::CommandPool
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le device parent.
		*\param[in] queueFamilyIndex
		*	L'index de la famille à laquelle appartient le pool.
		*\param[in] flags
		*	Combinaison binaire de renderer::CommandPoolCreateFlag.
		*\~english
		*\brief
		*	Constructor.
		*\param[in] device
		*	The logical connection to the GPU.
		*\param[in] queueFamilyIndex
		*	The pool's owner family index.
		*\param[in] flags
		*	renderer::CommandPoolCreateFlag bitwise OR combination.
		*/
		CommandPool( Device const & device
			, uint32_t queueFamilyIndex
			, renderer::CommandPoolCreateFlags flags = 0 );
		/**
		*\~french
		*\brief
		*	Destructeur.
		*\~english
		*\brief
		*	Destructor.
		*/
		~CommandPool();
		/**
		*\brief
		*	Crée un tampon de commandes.
		*\param[in] primary
		*	Dit si le tampon est un tampon de commandes primaire (\p true) ou secondaire (\p false).
		*\return
		*	Le tampon de commandes créé.
		*\~english
		*\brief
		*	Creates a command buffer.
		*\param[in] primary
		*	Tells if the command buffer is primary (\p true), or not (\p false).
		*\return
		*	The created command buffer.
		*/
		renderer::CommandBufferPtr createCommandBuffer( bool primary )const override;
		/**
		*\~french
		*\brief
		*	Conversion implicite vers VkCommandPool.
		*\~english
		*\brief
		*	VkCommandPool implicit cast operator.
		*/
		inline operator VkCommandPool const &( )const
		{
			return m_commandPool;
		}

	private:
		Device const & m_device;
		VkCommandPool m_commandPool{};
	};
}
