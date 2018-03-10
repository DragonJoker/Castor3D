/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_SubpassDescription_HPP___
#define ___Renderer_SubpassDescription_HPP___
#pragma once

#include "AttachmentReference.hpp"

namespace renderer
{
	/**
	*\~english
	*\brief
	*	Specifies a subpass description.
	*\~french
	*\brief
	*	Définit la description d'une sous-passe.
	*/
	struct SubpassDescription
	{
		/**
		*\~english
		*\brief
		*	Specifies usage of the subpass.
		*\~french
		*\brief
		*	Définit l'utilisation de la sous-passe.
		*/
		SubpassDescriptionFlags flags = 0u;
		/**
		*\~english
		*\brief
		*	Specifies whether this is a compute or graphics subpass.
		*\~french
		*\brief
		*	Définit si c'est une sous-passe de calcul ou graphique.
		*/
		PipelineBindPoint pipelineBindPoint = PipelineBindPoint::eGraphics;
		/**
		*\~english
		*\brief
		*	Lists the render pass attachments that can be read in the fragment shader stage during the subpass.
		*\~french
		*\brief
		*	Liste les attaches de la passe de rendu pouvant être lues depuis un fragment shader pendant la sous-passe.
		*/
		AttachmentReferenceArray inputAttachments;
		/**
		*\~english
		*\brief
		*	Lists the render pass attachments that will be used as colour attachments in the subpass.
		*\~french
		*\brief
		*	Liste les attaches de la passe de rendu qui seront utilisées comme attaches couleur dans la sous-passe.
		*/
		AttachmentReferenceArray colorAttachments;
		/**
		*\~english
		*\brief
		*	If not empty, lists the render pass attachments that are resolved at the end of the subpass.
		*	This array must be empty, or have the same attachment count as \p colorAttachments.
		*\~french
		*\brief
		*	Si non vide, liste les attaches de la passe de rendu qui sont résolues à la fin de la sous-passe.
		*	Ce tableau doit être vide, ou avoir le même nombre d'attaches que \p colorAttachments.
		*/
		AttachmentReferenceArray resolveAttachments;
		/**
		*\~english
		*\brief
		*	Specifies which attachment will be used for depth/stencil data.
		*\~french
		*\brief
		*	Définit quelle attache sera utilisée pour les données de profondeur/stencil.
		*/
		std::optional< AttachmentReference > depthStencilAttachment;
		/**
		*\~english
		*\brief
		*	Describes the attachments that are not used by the subpass, but whose content must be preserved throughout the subpass.
		*\~french
		*\brief
		*	Décrit les attaches non utilisées par la sous-passe, mais dont le contenu doit être préservé durant la sous-passe.
		*/
		UInt32Array reserveAttachments;
	};
}

#endif
