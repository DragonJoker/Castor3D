/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_RenderSubpass_HPP___
#define ___Renderer_RenderSubpass_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
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
	{
	protected:
		/**
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] device
		*	La connexion logique au GPU.
		*\param[in] attaches
		*	Les attaches de la sous-passe.
		*\param[in] neededState
		*	L'état voulu pour l'exécution de cette sous passe.
		*\~english
		*\brief
		*	Constructor.
		*\param[in] device
		*	The logical connection to the GPU.
		*\param[in] attaches
		*	The attachments of the subpass.
		*\param[in] neededState
		*	The state wanted for this subpass execution.
		*/
		RenderSubpass( Device const & device
			, RenderPassAttachmentArray const & attaches
			, RenderSubpassState const & neededState );

	public:
		/**
		*\~english
		*\brief
		*	Destructor.
		*\~french
		*\brief
		*	Destructeur.
		*/
		virtual ~RenderSubpass() = default;
	};
}

#endif
