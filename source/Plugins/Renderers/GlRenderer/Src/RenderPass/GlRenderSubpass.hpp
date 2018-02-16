/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#pragma once

#include "GlRendererPrerequisites.hpp"

#include <RenderPass/RenderPassAttachment.hpp>
#include <RenderPass/RenderSubpass.hpp>

namespace gl_renderer
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
		RenderSubpass( renderer::Device const & device
			, renderer::RenderPassAttachmentArray const & attaches
			, renderer::RenderSubpassState const & neededState );

		inline renderer::RenderPassAttachmentArray const & getAttaches()const
		{
			return m_attaches;
		}

	private:
		renderer::RenderPassAttachmentArray m_attaches;
	};
}
