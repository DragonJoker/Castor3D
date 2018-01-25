/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#pragma once

#include "VkRendererPrerequisites.hpp"

#include <RenderPass/RenderSubpass.hpp>
#include <RenderPass/RenderSubpassState.hpp>

namespace vk_renderer
{
	/**
	*\~french
	*\brief
	*	Description d'une sous passe de rendu.
	*\~english
	*\brief
	*	Describes a render subpass.
	*/
	class RenderSubpass
		: public renderer::RenderSubpass
	{
	public:
		/**
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] device
		*	La connexion logique au GPU.
		*\param[in] formats
		*	Les formats des attaches voulues pour la passe.
		*\param[in] neededState
		*	L'état voulu pour l'exécution de cette sous passe.
		*\~english
		*\brief
		*	Constructor.
		*\param[in] device
		*	The logical connection to the GPU.
		*\param[in] formats
		*	The attachments pixels formats.
		*\param[in] neededState
		*	The state wanted for this subpass execution.
		*/
		RenderSubpass( Device const & device
			, std::vector< renderer::PixelFormat > const & formats
			, renderer::RenderSubpassState const & neededState );
		/**
		*\~french
		*\return
		*	La VkSubpassDescription de cette sous passe.
		*\~english
		*\return
		*	This subpass' VkSubpassDescription.
		*/
		VkSubpassDescription const & retrieveDescription()const;
		/**
		*\~french
		*\return
		*	L'état voulu pour l'exécution de cette sous passe.
		*\~english
		*\return
		*	The state needed to execute this subpass.
		*/
		inline renderer::RenderSubpassState const & getNeededState()const
		{
			return m_neededState;
		}
		/**
		*\~french
		*\brief
		*	Opérateur de conversion implicite vers VkSubpassDescription.
		*\~english
		*\brief
		*	VkSubpassDescription implicit cast operator.
		*/
		inline operator VkSubpassDescription const &( )const
		{
			return m_description;
		}

	private:
		Device const & m_device;
		VkSubpassDescription m_description{};
		renderer::RenderSubpassState m_neededState{ 0u, 0u };
		std::vector< VkAttachmentReference > m_colourReferences;
		VkAttachmentReference m_depthReference{ 0xFFFFFFFF, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };
	};
}
