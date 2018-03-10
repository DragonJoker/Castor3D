/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_AttachmentStoreOp_HPP___
#define ___Renderer_AttachmentStoreOp_HPP___
#pragma once

namespace renderer
{
	/**
	*\english
	*\brief
	*	Specifies how contents of an attachment are treated at the end of a subpass.
	*\french
	*\brief
	*	Spécifie comment est traité le contenu d'une attache à la fin d'une sous-passe.
	*/
	enum class AttachmentStoreOp
		: uint32_t
	{
		/**
		*\~english
		*\brief
		*	The contents generated during the render pass and within the render area are written to memory.
		*	For attachments with a depth/stencil format, this uses the access type.
		*\~french
		*\brief
		*	Le contenu généré pendant la passe de rendu, et dans la zone de rendu, est écrit en mémoire.
		*	Pour les attaches ayant un format profondeur/stencil, cela utilise les types d'accès.
		*/
		eStore,
		/**
		*\~english
		*\brief
		*	The contents within the render area are not needed after rendering, and may be discarded; the contents of the attachment will be undefined inside the render area.
		*	For attachments with a depth/stencil format, this uses the access type renderer::AccessFlag::eDepthStencilAttachmentWrite.
		*	For attachments with a color format, this uses the access type renderer::AccessFlag::eColourAttachmentWrite.
		*\~french
		*\brief
		*	Le contenu dans la zone de rendu n'est pas utile après le rendu, il peut être jeté; le contenu de l'attache sera indéfini dans la zone de rendu.
		*	Pour les attaches ayant un format profondeur/stencil, cela utilise le type d'accès renderer::AccessFlag::eDepthStencilAttachmentWrite.
		*	Pour les attaches ayant un format couleur, cela utilise le type d'accès renderer::AccessFlag::eColourAttachmentWrite.
		*/
		eDontCare,
	};
	/**
	*\~english
	*\brief
	*	Gets the name of the given element type.
	*\param[in] value
	*	The element type.
	*\return
	*	The name.
	*\~french
	*\brief
	*	Récupère le nom du type d'élément donné.
	*\param[in] value
	*	Le type d'élément.
	*\return
	*	Le nom.
	*/
	inline std::string getName( AttachmentStoreOp value )
	{
		switch ( value )
		{
		case AttachmentStoreOp::eStore:
			return "store";

		case AttachmentStoreOp::eDontCare:
			return "dont_care";

		default:
			assert( false && "Unsupported AttachmentStoreOp." );
			throw std::runtime_error{ "Unsupported AttachmentStoreOp" };
		}

		return 0;
	}
}

#endif
