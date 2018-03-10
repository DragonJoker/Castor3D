/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_AttachmentLoadOp_HPP___
#define ___Renderer_AttachmentLoadOp_HPP___
#pragma once

namespace renderer
{
	/**
	*\english
	*\brief
	*	Specifies how contents of an attachment are treated at the beginning of a subpass.
	*\french
	*\brief
	*	Spécifie comment est traité le contenu d'une attache au début d'une sous-passe.
	*/
	enum class AttachmentLoadOp
		: uint32_t
	{
		/**
		*\~english
		*\brief
		*	The previous contents of the image within the render area will be preserved.
		*	For attachments with a depth/stencil format, this uses the access type.
		*\~french
		*\brief
		*	Le précédent contenu de l'image dans la zone de rendu sera préservé.
		*	Pour les attaches ayant un format profondeur/stencil, cela utilise les types d'accès.
		*/
		eLoad,
		/**
		*\~english
		*\brief
		*	The contents within the render area will be cleared to a uniform value, which is specified when a render pass instance is begun.
		*	For attachments with a depth/stencil format, this uses the access type renderer::AccessFlag::eDepthStencilAttachmentWrite.
		*	For attachments with a color format, this uses the access type renderer::AccessFlag::eColourAttachmentWrite.
		*\~french
		*\brief
		*	Le contenu dans la zone de rendu sera vidé par une valeur uniforme, spécifiée lorsque l'instance de la passe de rendu est démarrée.
		*	Pour les attaches ayant un format profondeur/stencil, cela utilise le type d'accès renderer::AccessFlag::eDepthStencilAttachmentWrite.
		*	Pour les attaches ayant un format couleur, cela utilise le type d'accès renderer::AccessFlag::eColourAttachmentWrite.
		*/
		eClear,
		/**
		*\~english
		*\brief
		*	The previous contents within the area need not be preserved; the contents of the attachment will be undefined inside the render area.
		*	For attachments with a depth/stencil format, this uses the access type renderer::AccessFlag::eDepthStencilAttachmentWrite.
		*	For attachments with a color format, this uses the access type renderer::AccessFlag::eColourAttachmentWrite.
		*\~french
		*\brief
		*	Le précédent contenu dans la zone de rendu n'a pas besoin d'être préservé; le contenu de l'attache sera indéfini dans la zone de rendu.
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
	inline std::string getName( AttachmentLoadOp value )
	{
		switch ( value )
		{
		case AttachmentLoadOp::eLoad:
			return "load";

		case AttachmentLoadOp::eClear:
			return "clear";

		case AttachmentLoadOp::eDontCare:
			return "dont_care";

		default:
			assert( false && "Unsupported AttachmentLoadOp." );
			throw std::runtime_error{ "Unsupported AttachmentLoadOp" };
		}

		return 0;
	}
}

#endif
