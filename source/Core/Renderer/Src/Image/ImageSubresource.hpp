/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_ImageSubresource_HPP___
#define ___Renderer_ImageSubresource_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	/**
	*\~english
	*\brief
	*	Structure specifying a image subresource.
	*\~french
	*\brief
	*	Spécifie une sous-ressource d'une image.
	*/
	struct ImageSubresource
	{
		/**
		*\~english
		*\brief
		*	Combination of renderer::ImageAspectFlag, selecting the image aspects.
		*\~french
		*\brief
		*	Combinaison de renderer::ImageAspectFlag, sélectionnant les aspects de l'image.
		*/
		ImageAspectFlags aspectMask;
		/**
		*\~english
		*\brief
		*	The mipmap level.
		*\~french
		*\brief
		*	Le niveau de mipmap.
		*/
		uint32_t mipLevel;
		/**
		*\~english
		*\brief
		*	The array layer.
		*\~french
		*\brief
		*	La couche du tableau.
		*/
		uint32_t arrayLayer;
	};
}

#endif
