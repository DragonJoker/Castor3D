/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_CommandPool_HPP___
#define ___Renderer_CommandPool_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	/**
	*\brief
	*	Pool de tampons de commandes.
	*/
	class CommandPool
	{
	protected:
		/**
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le device parent.
		*\param[in] queueFamilyIndex
		*	L'index de la famille à laquelle appartient le pool.
		*\param[in] flags
		*	Combinaison binaire de VkCommandPoolCreateFlagBits.
		*/
		CommandPool( Device const & device
			, uint32_t queueFamilyIndex
			, CommandPoolCreateFlags flags = 0 );

	public:
		/**
		*\~english
		*\brief
		*	Destructor.
		*\~french
		*\brief
		*	Destructeur.
		*/
		virtual ~CommandPool() = default;
		/**
		*\brief
		*	Crée un tampon de commandes.
		*\param[in] primary
		*	Dit si le tampon est un tampon de commandes primaire (\p true) ou secondaire (\p false).
		*\return
		*	Le tampon de commandes créé.
		*/
		virtual CommandBufferPtr createCommandBuffer( bool primary = true )const = 0;

	protected:
		Device const & m_device;
	};
}

#endif
