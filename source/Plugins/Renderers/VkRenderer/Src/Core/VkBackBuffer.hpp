/*
This file belongs to Renderer.
See LICENSE file in root folder
*/
#pragma once

#include "Image/VkTexture.hpp"
#include "Image/VkTextureView.hpp"

#include <Core/BackBuffer.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Classe encapsulant une image venant d'une swap chain.
	*/
	class BackBuffer
		: public renderer::BackBuffer
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*\param[in] device
		*	La connexion logique au GPU.
		*\param[in] swapchain
		*	La swapchain.
		*\param[in] imageIndex
		*	L'index de l'image, parmi les images de la swap chain.
		*/
		BackBuffer( Device const & device
			, renderer::SwapChain const & swapchain
			, uint32_t imageIndex
			, renderer::PixelFormat format
			, renderer::UIVec2 const & dimensions
			, Texture && texture );
		/**
		*\~french
		*\return
		*	L'index de l'image dans la swap chain.
		*\~english
		*\return
		*	The image index, inside the swap chain.
		*/
		inline uint32_t getImageIndex()const
		{
			return m_imageIndex;
		}
		/**
		*\~french
		*\return
		*	L'image du back buffer.
		*\~english
		*\return
		*	The back buffer image.
		*/
		inline Texture const & getTexture()const
		{
			return m_image;
		}
		/**
		*\~french
		*\return
		*	La vue sur l'image.
		*\~english
		*\return
		*	The image view.
		*/
		inline TextureView const & getView()const
		{
			return m_view;
		}

	private:
		Texture m_image;
		TextureView m_view;
		uint32_t m_imageIndex{ 0u };
	};
}
