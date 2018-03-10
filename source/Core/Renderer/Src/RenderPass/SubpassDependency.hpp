/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_SubpassDependency_HPP___
#define ___Renderer_SubpassDependency_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	static uint32_t constexpr ExternalSubpass = ~( 0u );
	/**
	*\~english
	*\brief
	*	Specifies a subpass dependency.
	*\~french
	*\brief
	*	Définit les dependances d'une sous-passe.
	*/
	struct SubpassDependency
	{
		/**
		*\~english
		*\brief
		*	The subpass index of the first subpass in the dependency.
		*\~french
		*\brief
		*	L'indice de la première sous-passe dans la dépendance.
		*/
		uint32_t srcSubpass = 0u;
		/**
		*\~english
		*\brief
		*	The subpass index of the second subpass in the dependency.
		*\~french
		*\brief
		*	L'indice de la seconde sous-passe dans la dépendance.
		*/
		uint32_t dstSubpass = 0u;
		/**
		*\~english
		*\brief
		*	Specifies the source stage mask.
		*\~french
		*\brief
		*	Définit le masque du niveau de la source.
		*/
		PipelineStageFlags srcStageMask = 0u;
		/**
		*\~english
		*\brief
		*	Specifies the destination stage mask.
		*\~french
		*\brief
		*	Définit le masque du niveau de la destination.
		*/
		PipelineStageFlags dstStageMask = 0u;
		/**
		*\~english
		*\brief
		*	Specifies the source access mask.
		*\~french
		*\brief
		*	Définit le masque d'accès de la source.
		*/
		AccessFlags srcAccessMask = 0u;
		/**
		*\~english
		*\brief
		*	Specifies the destination access mask.
		*\~french
		*\brief
		*	Définit le masque d'accès de la destination.
		*/
		AccessFlags dstAccessMask = 0u;
		/**
		*\~english
		*\brief
		*	The depency mask.
		*\~french
		*\brief
		*	Le masque de la dépendance.
		*/
		DependencyFlags dependencyFlags = 0u;
	};
}

#endif
