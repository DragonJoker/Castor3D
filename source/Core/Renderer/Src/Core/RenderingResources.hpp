/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_RenderingResources_HPP___
#define ___Renderer_RenderingResources_HPP___
#pragma once

#include "Command/CommandBuffer.hpp"
#include "Sync/Fence.hpp"
#include "Sync/Semaphore.hpp"

namespace renderer
{
	/**
	*\brief
	*	Classe regroupant les ressources de rendu nécessaires au dessin d'une image.
	*/
	class RenderingResources
	{
	protected:
		/**
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le Device parent.
		*/
		RenderingResources( Device const & device );

	public:
		/**
		*\~english
		*\brief
		*	Destructor.
		*\~french
		*\brief
		*	Destructeur.
		*/
		virtual ~RenderingResources() = default;
		/**
		*\brief
		*	Attend que le tampon de commandes soit prêt à l'enregistrement.
		*\param[in] timeout
		*	Le temps à attendre pour le signalement.
		*\return
		*	\p true si l'attente n'est pas sortie en timeout.
		*/
		virtual bool waitRecord( uint32_t timeout ) = 0;
		/**
		*\brief
		*	Définit le tampon de fenêtre.
		*\param[in] backBuffer
		*	Le nouveau tampon de fenêtre.
		*/
		inline void setBackBuffer( uint32_t backBuffer )
		{
			m_backBuffer = backBuffer;
		}
		/**
		*\return
		*	Le tampon de fenêtre.
		*/
		inline uint32_t getBackBuffer()const
		{
			return m_backBuffer;
		}
		/**
		*\return
		*	Le sémaphore d'attente que l'image soit disponible.
		*/
		inline Semaphore const & getImageAvailableSemaphore()const
		{
			return *m_imageAvailableSemaphore;
		}
		/**
		*\return
		*	Le sémaphore d'attente que le rendu soit terminé.
		*/
		inline Semaphore const & getRenderingFinishedSemaphore()const
		{
			return *m_finishedRenderingSemaphore;
		}
		/**
		*\return
		*	Le tampon de commandes.
		*/
		inline CommandBuffer const & getCommandBuffer()const
		{
			return *m_commandBuffer;
		}
		/**
		*\return
		*	La barrière.
		*/
		inline Fence const & getFence()const
		{
			return *m_fence;
		}
		/**
		*\return
		*	La périphérique logique.
		*/
		inline Device const & getDevice()const
		{
			return m_device;
		}

	protected:
		Device const & m_device;
		SemaphorePtr m_imageAvailableSemaphore;
		SemaphorePtr m_finishedRenderingSemaphore;
		FencePtr m_fence;
		CommandBufferPtr m_commandBuffer;
		uint32_t m_backBuffer{ 0u };
	};
}

#endif
