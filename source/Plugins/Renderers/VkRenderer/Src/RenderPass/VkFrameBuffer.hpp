/*
This file belongs to RendererLib.
See LICENSE file in root folder
*/
#pragma once

#include "VkRendererPrerequisites.hpp"

#include <RenderPass/FrameBuffer.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Classe encapsulant le concept de Framebuffer.
	*\remarks
	*	Contient les tampon de profondeur et de couleur.
	*/
	class FrameBuffer
		: public renderer::FrameBuffer
	{
	public:
		/**
		*\brief
		*	Constructeur, crée un FrameBuffer compatible avec la passe de rendu donnée.
		*\remarks
		*	Si la compatibilité entre les textures voulues et les formats de la passe de rendu
		*	n'est pas possible, une std::runtime_error est lancée.
		*\param[in] dimensions
		*	Les dimensions du tampon d'images.
		*\param[in] textures
		*	Les textures voulues pour le tampon d'images à créer.
		*\~english
		*\brief
		*	Constructor, creates a frame buffer compatible with given render pass.
		*\param[in] device
		*	The logical connection to the GPU.
		*\param[in] renderPass
		*	The render pass with which this framebuffer is compatible.
		*\param[in] dimensions
		*	The frame buffer's dimensions.
		*\param[in] textures
		*	The attachments.
		*/
		FrameBuffer( Device const & device
			, RenderPass const & renderPass
			, renderer::Extent2D const & dimensions
			, renderer::FrameBufferAttachmentArray && attachments );
		/**
		*\~french
		*\brief
		*	Destructeur.
		*\~english
		*\brief
		*	Destructor.
		*/
		~FrameBuffer();
		/**
		*\return
		*	Les dimensions du tampon.
		*/
		inline renderer::Extent2D const & getDimensions()const noexcept
		{
			return m_dimensions;
		}
		/**
		*\~french
		*\brief
		*	Conversion implicite vers VkFramebuffer.
		*\~english
		*\brief
		*	VkFramebuffer implicit cast operator.
		*/
		inline operator VkFramebuffer const &( )const
		{
			return m_framebuffer;
		}

	private:
		Device const & m_device;
		TextureViewCRefArray m_views;
		VkFramebuffer m_framebuffer{};
		renderer::Extent2D m_dimensions;
	};
}
