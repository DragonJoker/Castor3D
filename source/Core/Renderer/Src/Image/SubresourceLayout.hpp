/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_SubresourceLayout_HPP___
#define ___Renderer_SubresourceLayout_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	/**
	*\~english
	*\brief
	*	Information about the layout of the image subresource.
	*\~french
	*\brief
	*	Information à propos du layout d'une sous-ressource d'une image.
	*/
	struct SubresourceLayout
	{
		/**
		*\~english
		*\brief
		*	The byte offet from the start of the image where the image subresource begins.
		*\~french
		*\brief
		*	Le décalage en octets où le début de la sous-ressource de l'image commence.
		*/
		uint64_t offset;
		/**
		*\~english
		*\brief
		*	The byte size of the image subresource.
		*	\p size includes any extra memory that is required, based on \p rowPitch.
		*\~french
		*\brief
		*	La taille en octets de la sous-ressource de l'image.
		*	\p size inclut la mémoire additionnelle requise, en fonction de \p rowPitch.
		*/
		uint64_t size;
		/**
		*\~english
		*\brief
		*	The number of bytes between each row of texels in an image.
		*\~french
		*\brief
		*	Le nombre d'octets entre chaque ligne de texels d'une image.
		*/
		uint64_t rowPitch;
		/**
		*\~english
		*\brief
		*	The number of bytes between each array layer of an image.
		*\~french
		*\brief
		*	Le nombre d'octets entre chaque couche de tableau d'une image.
		*/
		uint64_t arrayPitch;
		/**
		*\~english
		*\brief
		*	The number of bytes between each slice of a 3D image.
		*\~french
		*\brief
		*	Le nombre d'octets entre chaque tranche d'une image 3D.
		*/
		uint64_t depthPitch;
	};
}

#endif
