/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_ImageCopy_HPP___
#define ___Renderer_ImageCopy_HPP___
#pragma once

#include "Image/ImageSubresourceLayers.hpp"
#include "Miscellaneous/Extent3D.hpp"
#include "Miscellaneous/Offset3D.hpp"

namespace renderer
{
	/**
	*\~english
	*\brief
	*	Specifies an image copy operation.
	*\~french
	*\brief
	*	Définit une opération de copie d'image.
	*/
	struct ImageCopy
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
		Offset3D srcOffset;
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
		Offset3D dstOffset;
		/**
		*\~english
		*\brief
		*	The size in texels of the source image to copy in \p width, \p height and \p depth.
		*\~french
		*\brief
		*	Les dimensions en texels de l'image source à copier, en \p largeur, \p hauteur, et \p profondeur.
		*/
		Extent3D extent;
	};
}

#endif
