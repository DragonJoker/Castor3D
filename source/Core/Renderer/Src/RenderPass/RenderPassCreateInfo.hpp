/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_RenderPassCreateInfo_HPP___
#define ___Renderer_RenderPassCreateInfo_HPP___
#pragma once

#include "AttachmentDescription.hpp"
#include "SubpassDescription.hpp"
#include "SubpassDependency.hpp"

namespace renderer
{
	/**
	*\~english
	*\brief
	*	Specifies parameters of a newly created render pass.
	*\~french
	*\brief
	*	Définit les paramètres de création d'une passe de rendu.
	*/
	struct RenderPassCreateInfo
	{
		/**
		*\~english
		*\brief
		*	Reserved for future use.
		*\~french
		*\brief
		*	Réservé pour plus tard.
		*/
		RenderPassCreateFlags flags;
		/**
		*\~english
		*\brief
		*	An array to AttachmentDescription, describing properties of the attachments.
		*\~french
		*\brief
		*	Un tableau de AttachmentDescription, décrivant les propriétés des attaches.
		*/
		AttachmentDescriptionArray attachments;
		/**
		*\~english
		*\brief
		*	Describes the properties of the subpasses to create.
		*\~french
		*\brief
		*	Définit les propriétés des sous passes à créer.
		*/
		SubpassDescriptionArray subpasses;
		/**
		*\~english
		*\brief
		*	Describes dependencies between pairs of subpasses.
		*\~french
		*\brief
		*	Définit les dépendances entre des paires de sous-passes.
		*/
		SubpassDependencyArray dependencies;
	};
}

#endif
