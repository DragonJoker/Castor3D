/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_RenderSubpassAttachment_HPP___
#define ___Renderer_RenderSubpassAttachment_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	static constexpr uint32_t AttachmentUnused = ~( 0u);
	/**
	*\~english
	*\brief
	*	An attach to a render subpass.
	*\~french
	*\brief
	*	Une attache à une sous-passe de rendu.
	*/
	struct AttachmentReference
	{
		/**
		*\~english
		*\brief
		*	The index of the attachment of the render pass.
		*	If any color or depth/stencil attachments are renderer::AttachmentUnused, then no writes occur for those attachments.
		*\~french
		*\brief
		*	L'indice de l'attache dans la passe de rendu.
		*	Si une attache couleur ou profondeur/stencil vaut renderer::AttachmentUnused, alors rien n'est écrit dans cette attache.
		*/
		uint32_t attachment;
		/**
		*\~english
		*\brief
		*	Specifies the layout the attachment uses during the subpass.
		*\~french
		*\brief
		*	Définit le layout que l'attache utilise pendant la sous-passe.
		*/
		ImageLayout layout;
	};
}

#endif
