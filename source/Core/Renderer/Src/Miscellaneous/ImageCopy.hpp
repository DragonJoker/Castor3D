/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_ImageCopy_HPP___
#define ___Renderer_ImageCopy_HPP___
#pragma once

#include "Image/ImageSubresourceLayers.hpp"

namespace renderer
{
	/**
	*\~english
	*\brief
	*	Specifies an image copy operation.
	*\~french
	*\brief
	*	D�finit une op�ration de copie d'image.
	*/
	struct ImageCopy
	{
		/**
		*\~english
		*\brief
		*	Specifies the specific image subresources of the image used for the source image data.
		*\~french
		*\brief
		*	Sp�cifie les sous-ressources d'image sp�cifiques � l'image utilis�e en tant que source.
		*/
		ImageSubresourceLayers srcSubresource;
		/**
		*\~english
		*\brief
		*	Select the initial x, y, and z offsets in texels of the sub-regions of the source image data.
		*\~french
		*\brief
		*	Selectionne les d�calages \p x, \p y et \p z des sous-r�gions de l'image source.
		*/
		IVec3 srcOffset;
		/**
		*\~english
		*\brief
		*	Specifies the specific image subresources of the image used for the destination image data.
		*\~french
		*\brief
		*	Sp�cifie les sous-ressources d'image sp�cifiques � l'image utilis�e en tant que detination.
		*/
		ImageSubresourceLayers dstSubresource;
		/**
		*\~english
		*\brief
		*	Select the initial x, y, and z offsets in texels of the sub-regions of the destination image data.
		*\~french
		*\brief
		*	Selectionne les d�calages \p x, \p y et \p z des sous-r�gions de l'image destination.
		*/
		IVec3 dstOffset;
		/**
		*\~english
		*\brief
		*	The size in texels of the source image to copy in \p width, \p height and \p depth.
		*\~french
		*\brief
		*	Les dimensions en texels de l'image source � copier, en \p largeur, \p hauteur, et \p profondeur.
		*/
		UIVec3 extent;
	};
}

#endif
