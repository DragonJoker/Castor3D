/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_ImageMemoryBarrier_HPP___
#define ___Renderer_ImageMemoryBarrier_HPP___
#pragma once

#include "Image/ImageSubresourceRange.hpp"

namespace renderer
{
	/**
	*\brief
	*	Encapsulation d'un vkImageMemoryBarrier.
	*/
	class ImageMemoryBarrier
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*\param[in] srcAccessMask
		*	Les indicateurs d'accès avant la barrière.
		*\param[in] dstAccessMask
		*	Les indicateurs d'accès après la barrière.
		*\param[in] oldLayout
		*	Le layout d'image avant la barrière.
		*\param[in] newLayout
		*	Le layout d'image après la barrière.
		*\param[in] srcQueueFamilyIndex
		*	La famille de file voulue avant la barrière.
		*\param[in] dstQueueFamilyIndex
		*	La famille de file voulue après la barrière.
		*\param[in] image
		*	L'image.
		*\param[in] subresourceRange
		*	L'intervalle de sous-ressource.
		*/
		ImageMemoryBarrier( AccessFlags srcAccessMask
			, AccessFlags dstAccessMask
			, ImageLayout oldLayout
			, ImageLayout newLayout
			, uint32_t srcQueueFamilyIndex
			, uint32_t dstQueueFamilyIndex
			, Texture const & image
			, ImageSubresourceRange const & subresourceRange );
		/**
		*\return
		*	Les indicateurs d'accès avant la barrière.
		*/
		inline AccessFlags getSrcAccessMask()const
		{
			return m_srcAccessMask;
		}
		/**
		*\return
		*	Les indicateurs d'accès après la barrière.
		*/
		inline AccessFlags getDstAccessMask()const
		{
			return m_dstAccessMask;
		}
		/**
		*\return
		*	Le layout d'image avant la barrière.
		*/
		inline ImageLayout getOldLayout()const
		{
			return m_oldLayout;
		}
		/**
		*\return
		*	Le layout d'image après la barrière.
		*/
		inline ImageLayout getNewLayout()const
		{
			return m_newLayout;
		}
		/**
		*\return
		*	La famille de file voulue avant la barrière.
		*/
		inline uint32_t getSrcQueueFamilyIndex()const
		{
			return m_srcQueueFamilyIndex;
		}
		/**
		*\return
		*	La famille de file voulue après la barrière.
		*/
		inline uint32_t getDstQueueFamilyIndex()const
		{
			return m_dstQueueFamilyIndex;
		}
		/**
		*\return
		*	L'image.
		*/
		inline Texture const & getImage()const
		{
			return m_image;
		}
		/**
		*\return
		*	L'intervalle de sous-ressource.
		*/
		inline ImageSubresourceRange const & getSubresourceRange()const
		{
			return m_subresourceRange;
		}

	private:
		AccessFlags m_srcAccessMask;
		AccessFlags m_dstAccessMask;
		ImageLayout m_oldLayout;
		ImageLayout m_newLayout;
		uint32_t m_srcQueueFamilyIndex;
		uint32_t m_dstQueueFamilyIndex;
		Texture const & m_image;
		ImageSubresourceRange m_subresourceRange;
	};
}

#endif
