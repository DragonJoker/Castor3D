/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_RenderPassAttachment_HPP___
#define ___Renderer_RenderPassAttachment_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	/**
	*\~english
	*\brief
	*	An attach to a render pass.
	*\~french
	*\brief
	*	Une attache à une passe de rendu.
	*/
	struct AttachmentDescription
	{
		/**
		*\~english
		*\brief
		*	The attach index.
		*\~french
		*\brief
		*	L'indice de l'attache.
		*/
		uint32_t index;
		/**
		*\~english
		*\brief
		*	The attach pixel format.
		*\~french
		*\brief
		*	Le format des pixels de l'attache.
		*/
		Format format;
		/**
		*\~english
		*\brief
		*	The number of samples of the image as defined in renderer::SampleCountFlag.
		*\~french
		*\brief
		*	Le nombre d'échantillons de l'image tel que défini dans renderer::SampleCountFlag.
		*/
		SampleCountFlag samples;
		/**
		*\~english
		*\brief
		*	Specifies how the contents of color and depth components of the attachment are treated at the beginning of the subpass where it is first used.
		*\~french
		*\brief
		*	Spécifie comment le contenu des composantes couleur et profondeur de l'attache sont traitées au début de la sous-passe où elles sont utilisées pour la première fois.
		*/
		AttachmentLoadOp loadOp;
		/**
		*\~english
		*\brief
		*	Specifies how the contents of color and depth components of the attachment are treated at the end of the subpass where it is last used.
		*\~french
		*\brief
		*	Spécifie comment le contenu des composantes couleur et profondeur de l'attache sont traitées à la fin de la sous-passe où elles sont utilisées pour la dernière fois.
		*/
		AttachmentStoreOp storeOp;
		/**
		*\~english
		*\brief
		*	Specifies how the contents of stencil components of the attachment are treated at the beginning of the subpass where it is first used.
		*\~french
		*\brief
		*	Spécifie comment le contenu des composantes stencil de l'attache sont traitées au début de la sous-passe où elles sont utilisées pour la première fois.
		*/
		AttachmentLoadOp stencilLoadOp;
		/**
		*\~english
		*\brief
		*	Specifies how the contents of stencil components of the attachment are treated at the end of the last subpass where it is used.
		*\~french
		*\brief
		*	Spécifie comment le contenu des composantes stencil de l'attache sont traitées à la fin de la sous-passe où elles sont utilisées pour la dernière fois.
		*/
		AttachmentStoreOp stencilStoreOp;
		/**
		*\~english
		*\brief
		*	The layout the attachment image subresource will be in when a render pass instance begins.
		*\~french
		*\brief
		*	Le layout de la sous-ressource de l'image de l'attache, lorsqu'une instance de la passe de rendu commence.
		*/
		ImageLayout initialLayout;
		/**
		*\~english
		*\brief
		*	The layout the attachment image subresource will be transitioned to when a render pass instance ends.
		*	During a render pass instance, an attachment can use a different layout in each subpass, if desired.
		*\~french
		*\brief
		*	Le layout vers lequel la sous-ressource de l'image de l'attache sera transitionné, lorsqu'une instance de la passe de rendu termine.
		*	Pendant une instanec de passe de rendu, une attache peut utiliser différents layouts pour chaque sous-pass, si on le souhaite.
		*/
		ImageLayout finalLayout;
	};
}

#endif
