/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_ImageSubresourceLayers_HPP___
#define ___Renderer_ImageSubresourceLayers_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	/**
	*\~english
	*\brief
	*	Structure specifying a image subresource layers.
	*\~french
	*\brief
	*	Spécifie les couches d'une sous-ressource d'une image.
	*/
	struct ImageSubresourceLayers
	{
		/**
		*\~english
		*\brief
		*	Combination of renderer::ImageAspectFlag, selecting the color, depth and/or stencil aspects to be copied.
		*\~french
		*\brief
		*	Combinaison de renderer::ImageAspectFlag, sélectionnant les aspects à copier, parmi couleur, profondeur et/ou stencil.
		*/
		ImageAspectFlags aspectMask;
		/**
		*\~english
		*\brief
		*	The mipmap level to copy from or to.
		*\~french
		*\brief
		*	Le niveau de mipmap depuis ou vers lequel s'effectue la copie.
		*/
		uint32_t mipLevel;
		/**
		*\~english
		*\brief
		*	The starting layer.
		*\~french
		*\brief
		*	La couche de base du tableau.
		*/
		uint32_t baseArrayLayer;
		/**
		*\~english
		*\brief
		*	The number of layers to copy.
		*\~french
		*\brief
		*	Le nombre de couches.
		*/
		uint32_t layerCount;
	};
}

#endif
