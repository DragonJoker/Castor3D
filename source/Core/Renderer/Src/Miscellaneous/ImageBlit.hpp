/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_ImageBlit_HPP___
#define ___Renderer_ImageBlit_HPP___
#pragma once

#include "Image/ImageSubresourceLayers.hpp"

namespace renderer
{
	/**
	*\~english
	*\brief
	*	Specifies an image blit operation.
	*\~french
	*\brief
	*	Définit une opération de copie rapide d'image.
	*/
	struct ImageBlit
	{
		/**
		*\~english
		*\brief
		*	Specifies the specific image subresources of the image used for the source image data.
		*\~french
		*\brief
		*	Spécifie les sous-ressources d'image spécifiques à l'image utilisée en tant que source.
		*/
		ImageSubresourceLayers srcSubresource;
		/**
		*\~english
		*\brief
		*	Select the initial x, y, and z offsets in texels of the sub-regions of the source image data.
		*\~french
		*\brief
		*	Selectionne les décalages \p x, \p y et \p z des sous-régions de l'image source.
		*/
		IVec3 srcOffset;
		/**
		*\~english
		*\brief
		*	Specifies the specific image subresources of the image used for the destination image data.
		*\~french
		*\brief
		*	Spécifie les sous-ressources d'image spécifiques à l'image utilisée en tant que detination.
		*/
		ImageSubresourceLayers dstSubresource;
		/**
		*\~english
		*\brief
		*	Select the initial x, y, and z offsets in texels of the sub-regions of the destination image data.
		*\~french
		*\brief
		*	Selectionne les décalages \p x, \p y et \p z des sous-régions de l'image destination.
		*/
		IVec3 dstOffset;
	};
}

#endif
