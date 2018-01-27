/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#pragma once

#include "VkRendererPrerequisites.hpp"

#include <RenderPass/RenderPass.hpp>
#include <RenderPass/RenderPassState.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Description d'une passe de rendu (pouvant contenir plusieurs sous-passes).
	*\~english
	*\brief
	*	Describes a render pass (which can contain one or more render subpasses).
	*/
	class RenderPass
		: public renderer::RenderPass
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*\param[in] device
		*	La connexion logique au GPU.
		*\param[in] formats
		*	Les formats des attaches voulues pour la passe.
		*\param[in] subpasses
		*	Les sous passes (au moins 1 nécéssaire).
		*\param[in] initialState
		*	L'état voulu en début de passe.
		*\param[in] finalState
		*	L'état voulu en fin de passe.
		*\param[in] clear
		*	Dit si l'on veut vider le contenu des images au chargement de la passe.
		*\param[in] samplesCount
		*	Le nombre d'échantillons (pour le multisampling).
		*\~english
		*\brief
		*	Constructor.
		*\param[in] device
		*	The logical connection to the GPU.
		*\param[in] formats
		*	The attachments pixels formats.
		*\param[in] subpasses
		*	The rendersubpasses (at least one is necessary).
		*\param[in] initialState
		*	The state wanted at the beginning of the pass.
		*\param[in] finalState
		*	The state attained at the end of the pass.
		*\param[in] clear
		*	Tells if we want to clear the images at the beginning of the pass.
		*\param[in] samplesCount
		*	The samples count (for multisampling).
		*/
		RenderPass( Device const & device
			, std::vector< renderer::PixelFormat > const & formats
			, renderer::RenderSubpassPtrArray const & subpasses
			, renderer::RenderPassState const & initialState
			, renderer::RenderPassState const & finalState
			, bool clear
			, renderer::SampleCountFlag samplesCount );
		/**
		*\~french
		*\brief
		*	Destructeur.
		*\~english
		*\brief
		*	Destructor.
		*/
		~RenderPass();
		/**
		*\brief
		*	Crée un FrameBuffer compatible avec la passe de rendu.
		*\remarks
		*	Si la compatibilité entre les textures voulues et les formats de la passe de rendu
		*	n'est pas possible, une std::runtime_error est lancée.
		*\param[in] dimensions
		*	Les dimensions du tampon d'images.
		*\param[in] textures
		*	Les textures voulues pour le tampon d'images à créer.
		*\return
		*	Le FrameBuffer créé.
		*\~english
		*\brief
		*	Creates a frame buffer compatible with this render pass.
		*\remarks
		*	If the compatibility between wanted views and the render pass' formats
		*	is not possible, a std::runtime_error will be thrown.
		*\param[in] width, height
		*	The frame buffer's dimensions.
		*\param[in] views
		*	The views on the images from which the frame buffer is to br created.
		*\return
		*	The created frame buffer.
		*/
		renderer::FrameBufferPtr createFrameBuffer( renderer::UIVec2 const & dimensions
			, renderer::TextureAttachmentPtrArray && textures )const override;
		std::vector< VkClearValue > const & getClearValues (renderer::ClearValueArray const & clearValues)const;
		/**
		*\~french
		*\brief
		*	Conversion implicite vers VkRenderPass.
		*\~english
		*\brief
		*	VkRenderPass implicit cast operator.
		*/
		inline operator VkRenderPass const &( )const
		{
			return m_renderPass;
		}

	private:
		Device const & m_device;
		std::vector< renderer::PixelFormat > m_formats;
		VkRenderPass m_renderPass{};
		renderer::SampleCountFlag m_samplesCount{};
		RenderSubpassCRefArray m_subpasses;
		renderer::RenderPassState m_initialState;
		renderer::RenderPassState m_finalState;
		mutable std::vector< VkClearValue > m_clearValues;
	};
}
