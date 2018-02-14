/*
This file belongs to Renderer.
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
			, renderer::UIVec2 const & dimensions
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
		*\brief
		*	Copie des données dans la RAM.
		*\param[in] queue
		*	La file utilisée pour la copie.
		*\param[in] index
		*	L'index de l'attache.
		*\param[in] xoffset, yoffset
		*	Le décalage à partir duquel les données seront copiées, par rapport
		*	au début du stockage de la texture, en VRAM.
		*\param[in] width, height
		*	Les dimensions des données à copier.
		*\param[in] format
		*	Le format voulu pour les données.
		*\param[out] data
		*	Reçoit les données copiées.
		*\~english
		*\brief
		*	Copies data from an attach in RAM.
		*\param[in] queue
		*	The queue used for the copy.
		*\param[in] index
		*	The attach index.
		*\param[in] xoffset, yoffset
		*	The X and Y offsets.
		*\param[in] width, height
		*	The data copy dimensions.
		*\param[in] format
		*	The wanted format for the copy.
		*\param[out] data
		*	Receives copied data.
		*/
		void download( renderer::Queue const & queue
			, uint32_t index
			, uint32_t xoffset
			, uint32_t yoffset
			, uint32_t width
			, uint32_t height
			, renderer::PixelFormat format
			, uint8_t * data )const noexcept;
		/**
		*\return
		*	Les dimensions du tampon.
		*/
		inline renderer::UIVec2 const & getDimensions()const noexcept
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
		renderer::UIVec2 m_dimensions;
	};
}
