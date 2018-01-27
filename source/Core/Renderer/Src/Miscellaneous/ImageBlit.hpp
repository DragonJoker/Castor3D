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
	*	D�finit une op�ration de copie rapide d'image.
	*/
	struct ImageBlit
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
	};
}

#endif
