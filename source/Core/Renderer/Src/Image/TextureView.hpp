/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_TextureView_HPP___
#define ___Renderer_TextureView_HPP___
#pragma once

#include "ImageViewCreateInfo.hpp"

namespace renderer
{
	/**
	*\~french
	*\brief
	*	Description d'une vue sur une image.
	*\~english
	*\brief
	*	Image view wrapper.
	*/
	class TextureView
	{
	protected:
		/**
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] device
		*	La connexion logique au GPU.
		*\param[in] image
		*	L'image sur laquelle la vue est créée.
		*\param[in] createInfo
		*	Les informations de création de la vue.
		*\~english
		*\brief
		*	Constructor.
		*\param[in] device
		*	The logical connection to the GPU.
		*\param[in] image
		*	The image from which the view is created.
		*\param[in] createInfo
		*	The view creation informations.
		*/
		TextureView( Device const & device
			, Texture const & image
			, ImageViewCreateInfo const & createInfo );

	public:
		/**
		*\~french
		*\brief
		*	Destructeur.
		*\~english
		*\brief
		*	Destructor.
		*/
		virtual ~TextureView() = default;
		/**
		*\~french
		*\brief
		*	Prépare une barrière mémoire de transition vers un layout général.
		*\param[in] srcLayout
		*	Le layout source.
		*\param[in] srcAccessFlags
		*	Les accès source.
		*\param[in] dstAccessFlags
		*	Les accès destination.
		*\param[in] srcQueueFamily
		*	La famille de file source, pour un transfer de famille de file.
		*\param[in] dstQueueFamily
		*	La famille de file destination, pour un transfer de famille de file.
		*\return
		*	La barrière mémoire.
		*\~english
		*\brief
		*	Prepares a transition memory barrier, for a general layout.
		*\param[in] srcLayout
		*	The old layout.
		*\param[in] srcAccessFlags
		*	The source access mask.
		*\param[in] dstAccessFlags
		*	The destination access mask.
		*\param[in] srcQueueFamily
		*	The source queue family, for a queue family transfer.
		*\param[in] dstQueueFamily
		*	The destination queue family, for a queue family transfer.
		*\return
		*	The memory barrier.
		*/
		ImageMemoryBarrier makeGeneralLayout( ImageLayout srcLayout
			, AccessFlags srcAccessFlags
			, AccessFlags dstAccessFlags
			, uint32_t srcQueueFamily = ~( 0u )
			, uint32_t dstQueueFamily = ~( 0u ) )const;
		/**
		*\~french
		*\brief
		*	Prépare une barrière mémoire de transition vers un layout de destination de transfert.
		*\param[in] srcLayout
		*	Le layout source.
		*\param[in] srcAccessFlags
		*	Les accès source.
		*\param[in] srcQueueFamily
		*	La famille de file source, pour un transfer de famille de file.
		*\param[in] dstQueueFamily
		*	La famille de file destination, pour un transfer de famille de file.
		*\return
		*	La barrière mémoire.
		*\~english
		*\brief
		*	Prepares a transition memory barrier, for a transfer destination layout.
		*\param[in] srcLayout
		*	The old layout.
		*\param[in] srcAccessFlags
		*	The source access mask.
		*\param[in] srcQueueFamily
		*	The source queue family, for a queue family transfer.
		*\param[in] dstQueueFamily
		*	The destination queue family, for a queue family transfer.
		*\return
		*	The memory barrier.
		*/
		ImageMemoryBarrier makeTransferDestination( ImageLayout srcLayout
			, AccessFlags srcAccessFlags
			, uint32_t srcQueueFamily = ~( 0u )
			, uint32_t dstQueueFamily = ~( 0u ) )const;
		/**
		*\~french
		*\brief
		*	Prépare une barrière mémoire de transition vers un layout de source de transfert.
		*\param[in] srcLayout
		*	Le layout source.
		*\param[in] srcAccessFlags
		*	Les accès source.
		*\param[in] srcQueueFamily
		*	La famille de file source, pour un transfer de famille de file.
		*\param[in] dstQueueFamily
		*	La famille de file destination, pour un transfer de famille de file.
		*\return
		*	La barrière mémoire.
		*\~english
		*\brief
		*	Prepares a transition memory barrier, for a transfer source layout.
		*\param[in] srcLayout
		*	The old layout.
		*\param[in] srcAccessFlags
		*	The source access mask.
		*\param[in] srcQueueFamily
		*	The source queue family, for a queue family transfer.
		*\param[in] dstQueueFamily
		*	The destination queue family, for a queue family transfer.
		*\return
		*	The memory barrier.
		*/
		ImageMemoryBarrier makeTransferSource( ImageLayout srcLayout
			, AccessFlags srcAccessFlags
			, uint32_t srcQueueFamily = ~( 0u )
			, uint32_t dstQueueFamily = ~( 0u ) )const;
		/**
		*\~french
		*\brief
		*	Prépare une barrière mémoire de transition vers un layout de ressource d'entrée (lecture seule) d'un shader.
		*\param[in] srcLayout
		*	Le layout source.
		*\param[in] srcAccessFlags
		*	Les accès source.
		*\param[in] srcQueueFamily
		*	La famille de file source, pour un transfer de famille de file.
		*\param[in] dstQueueFamily
		*	La famille de file destination, pour un transfer de famille de file.
		*\return
		*	La barrière mémoire.
		*\~english
		*\brief
		*	Prepares a transition memory barrier, for an input shader resource layout.
		*\param[in] srcLayout
		*	The old layout.
		*\param[in] srcAccessFlags
		*	The source access mask.
		*\param[in] srcQueueFamily
		*	The source queue family, for a queue family transfer.
		*\param[in] dstQueueFamily
		*	The destination queue family, for a queue family transfer.
		*\return
		*	The memory barrier.
		*/
		ImageMemoryBarrier makeShaderInputResource( ImageLayout srcLayout
			, AccessFlags srcAccessFlags
			, uint32_t srcQueueFamily = ~( 0u )
			, uint32_t dstQueueFamily = ~( 0u ) )const;
		/**
		*\~french
		*\brief
		*	Prépare une barrière mémoire de transition vers un layout de ressource de lecture de profondeur et/ou stencil.
		*\param[in] srcLayout
		*	Le layout source.
		*\param[in] srcAccessFlags
		*	Les accès source.
		*\param[in] srcQueueFamily
		*	La famille de file source, pour un transfer de famille de file.
		*\param[in] dstQueueFamily
		*	La famille de file destination, pour un transfer de famille de file.
		*\return
		*	La barrière mémoire.
		*\~english
		*\brief
		*	Prepares a transition memory barrier, for a depth ant/or stencil read only layout.
		*\param[in] srcLayout
		*	The old layout.
		*\param[in] srcAccessFlags
		*	The source access mask.
		*\param[in] srcQueueFamily
		*	The source queue family, for a queue family transfer.
		*\param[in] dstQueueFamily
		*	The destination queue family, for a queue family transfer.
		*\return
		*	The memory barrier.
		*/
		ImageMemoryBarrier makeDepthStencilReadOnly( ImageLayout srcLayout
			, AccessFlags srcAccessFlags
			, uint32_t srcQueueFamily = ~( 0u )
			, uint32_t dstQueueFamily = ~( 0u ) )const;
		/**
		*\~french
		*\brief
		*	Prépare une barrière mémoire de transition vers un layout d'attache couleur.
		*\param[in] srcLayout
		*	Le layout source.
		*\param[in] srcAccessFlags
		*	Les accès source.
		*\param[in] srcQueueFamily
		*	La famille de file source, pour un transfer de famille de file.
		*\param[in] dstQueueFamily
		*	La famille de file destination, pour un transfer de famille de file.
		*\return
		*	La barrière mémoire.
		*\~english
		*\brief
		*	Prepares a transition memory barrier, for a colour attachment layout.
		*\param[in] srcLayout
		*	The old layout.
		*\param[in] srcAccessFlags
		*	The source access mask.
		*\param[in] srcQueueFamily
		*	The source queue family, for a queue family transfer.
		*\param[in] dstQueueFamily
		*	The destination queue family, for a queue family transfer.
		*\return
		*	The memory barrier.
		*/
		ImageMemoryBarrier makeColourAttachment( ImageLayout srcLayout
			, AccessFlags srcAccessFlags
			, uint32_t srcQueueFamily = ~( 0u )
			, uint32_t dstQueueFamily = ~( 0u ) )const;
		/**
		*\~french
		*\brief
		*	Prépare une barrière mémoire de transition vers un layout d'attache profondeur et/ou stencil.
		*\param[in] srcLayout
		*	Le layout source.
		*\param[in] srcAccessFlags
		*	Les accès source.
		*\param[in] srcQueueFamily
		*	La famille de file source, pour un transfer de famille de file.
		*\param[in] dstQueueFamily
		*	La famille de file destination, pour un transfer de famille de file.
		*\return
		*	La barrière mémoire.
		*\~english
		*\brief
		*	Prepares a transition memory barrier, for a depth and/or stencil attachment layout.
		*\param[in] srcLayout
		*	The old layout.
		*\param[in] srcAccessFlags
		*	The source access mask.
		*\param[in] srcQueueFamily
		*	The source queue family, for a queue family transfer.
		*\param[in] dstQueueFamily
		*	The destination queue family, for a queue family transfer.
		*\return
		*	The memory barrier.
		*/
		ImageMemoryBarrier makeDepthStencilAttachment( ImageLayout srcLayout
			, AccessFlags srcAccessFlags
			, uint32_t srcQueueFamily = ~( 0u )
			, uint32_t dstQueueFamily = ~( 0u ) )const;
		/**
		*\~french
		*\brief
		*	Prépare une barrière mémoire de transition vers un layout de source de presentation.
		*\param[in] srcLayout
		*	Le layout source.
		*\param[in] srcAccessFlags
		*	Les accès source.
		*\param[in] srcQueueFamily
		*	La famille de file source, pour un transfer de famille de file.
		*\param[in] dstQueueFamily
		*	La famille de file destination, pour un transfer de famille de file.
		*\return
		*	La barrière mémoire.
		*\~english
		*\brief
		*	Prepares a transition memory barrier, for a presentation source layout.
		*\param[in] srcLayout
		*	The old layout.
		*\param[in] srcAccessFlags
		*	The source access mask.
		*\param[in] srcQueueFamily
		*	The source queue family, for a queue family transfer.
		*\param[in] dstQueueFamily
		*	The destination queue family, for a queue family transfer.
		*\return
		*	The memory barrier.
		*/
		ImageMemoryBarrier makePresentSource( ImageLayout srcLayout
			, AccessFlags srcAccessFlags
			, uint32_t srcQueueFamily = ~( 0u )
			, uint32_t dstQueueFamily = ~( 0u ) )const;
		/**
		*\~french
		*\return
		*	Le type de texture.
		*\~english
		*\return
		*	The texture type.
		*/
		inline TextureViewType getType()const
		{
			return m_createInfo.viewType;
		}
		/**
		*\~french
		*\return
		*	Le format de l'image de la vue.
		*\~english
		*\return
		*	The view's image's pixels format.
		*/
		inline Format getFormat()const
		{
			return m_createInfo.format;
		}
		/**
		*\~french
		*\return
		*	L'image de la vue.
		*\~english
		*\return
		*	The view's image.
		*/
		inline Texture const & getTexture()const
		{
			return m_image;
		}
		/**
		*\~french
		*\return
		*	Les informations de la ressource liée à l'image.
		*\~english
		*\return
		*	The image's resource information.
		*/
		inline ImageSubresourceRange const & getSubResourceRange()const
		{
			return m_createInfo.subresourceRange;
		}

	private:
		/**
		*\~french
		*\brief
		*	Prépare une barrière mémoire de transition.
		*\param[in] srcLayout
		*	Le layout source.
		*\param[in] dstLayout
		*	Le layout destination.
		*\param[in] srcAccessFlags
		*	Les accès source.
		*\param[in] dstAccessFlags
		*	Les accès destination.
		*\param[in] srcQueueFamily
		*	La famille de file source, pour un transfer de famille de file.
		*\param[in] dstQueueFamily
		*	La famille de file destination, pour un transfer de famille de file.
		*\return
		*	La barrière mémoire.
		*\~english
		*\brief
		*	Prepares a transition memory barrier.
		*\param[in] srcLayout
		*	The old layout.
		*\param[in] srcAccessFlags
		*	The source access mask.
		*\param[in] dstAccessFlags
		*	The destination access mask.
		*\param[in] srcQueueFamily
		*	The source queue family, for a queue family transfer.
		*\param[in] dstQueueFamily
		*	The destination queue family, for a queue family transfer.
		*\return
		*	The memory barrier.
		*/
		virtual ImageMemoryBarrier doMakeLayoutTransition( ImageLayout srcLayout
			, ImageLayout dstLayout
			, AccessFlags srcAccessFlags
			, AccessFlags dstAccessMask
			, uint32_t srcQueueFamily
			, uint32_t dstQueueFamily )const = 0;

	protected:
		ImageViewCreateInfo m_createInfo;

	private:
		Device const & m_device;
		Texture const & m_image;
	};
}

#endif
