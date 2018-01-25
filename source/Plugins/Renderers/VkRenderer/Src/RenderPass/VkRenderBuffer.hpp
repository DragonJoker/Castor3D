/**
*\file
*	Texture.h
*\author
*	Sylvain Doremus
*/
#ifndef ___VkRenderer_RenderBuffer_HPP___
#define ___VkRenderer_RenderBuffer_HPP___
#pragma once

#include "Miscellaneous/VkMemoryStorage.hpp"
#include "Image/VkTexture.hpp"
#include "Image/VkTextureView.hpp"

#include <RenderPass/RenderBuffer.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Un wrapper autour d'une texture ne pouvant être utilisée qu'en destination de rendu.
	*/
	class RenderBuffer
		: public renderer::RenderBuffer
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le périphérique logique.
		*/
		RenderBuffer( Device const & device
			, renderer::PixelFormat format
			, renderer::UIVec2 const & size );
		/**
		*\brief
		*	Prépare une barrière mémoire de transition vers un layout d'attache couleur.
		*\return
		*	La barrière mémoire.
		*/
		renderer::ImageMemoryBarrier makeColourAttachment()const override;
		/**
		*\brief
		*	Prépare une barrière mémoire de transition vers un layout d'attache profondeur et stencil.
		*\return
		*	La barrière mémoire.
		*/
		renderer::ImageMemoryBarrier makeDepthStencilAttachment()const override;
		/**
		*\return
		*	L'image vulkan.
		*/
		inline Texture const & getImage()const noexcept
		{
			return m_texture;
		}

	private:
		VkFormat m_format;
		Texture m_texture;
	};
}

#endif
