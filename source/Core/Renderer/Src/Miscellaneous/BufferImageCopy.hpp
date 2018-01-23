/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_BufferImageCopy_HPP___
#define ___Renderer_BufferImageCopy_HPP___
#pragma once

#include "Image/ImageSubresourceLayers.hpp"

namespace renderer
{
	/**
	*\~english
	*\brief
	*	Specifies a buffer image copy operation.
	*\~french
	*\brief
	*	D�finit une op�ration de copie d'image depuis, ou vers, un tampon.
	*/
	struct BufferImageCopy
	{
		/**
		*\~english
		*\brief
		*	The offset in bytes from the start of the buffer where the image data is copied from or to.
		*\~french
		*\brief
		*	Le d�calage en octets depuis le d�but du tampon, depuis ou vers o� les donn�es de l'image sont copi�es.
		*/
		uint32_t bufferOffset;
		/**
		*\~english
		*\brief
		*	Specifies the data in buffer memory as a subregion of a larger two- or three-dimensional image, and control the addressing calculations of data in buffer memory.
		*\remarks
		*	If either \p bufferRowLength is zero, that aspect of the buffer memory is considered to be tightly packed according to the \p imageExtent.
		*\~french
		*\brief
		*	Sp�cifie les donn�es en m�moire du tampon d'une sous-r�gion d'une image en 2, ou 3 dimensions d'un image plus large, et contr�le le calcul d'adressage des donn�es dans la m�moire du tampon.
		*\remarks
		*	Si \p bufferRowLength vaut 0, cet aspect de la m�moire du tampon est consid�r� comme �troitement ordonn�es, en fonction de \p imageExtent.
		*/
		uint32_t bufferRowLength;
		/**
		*\~english
		*\brief
		*	Specifies the data in buffer memory as a subregion of a larger two- or three-dimensional image, and control the addressing calculations of data in buffer memory.
		*\remarks
		*	If either \p bufferImageHeight is zero, that aspect of the buffer memory is considered to be tightly packed according to the \p imageExtent.
		*\~french
		*\brief
		*	Sp�cifie les donn�es en m�moire du tampon d'une sous-r�gion d'une image en 2, ou 3 dimensions d'un image plus large, et contr�le le calcul d'adressage des donn�es dans la m�moire du tampon.
		*\remarks
		*	Si \p bufferImageHeight vaut 0, cet aspect de la m�moire du tampon est consid�r� comme �troitement ordonn�es, en fonction de \p imageExtent.
		*/
		uint32_t bufferImageHeight;
		/**
		*\~english
		*\brief
		*	Specifies the specific image subresources of the image used for the source or destination image data.
		*\~french
		*\brief
		*	Sp�cifie les sous-ressources d'image sp�cifiques � l'image utilis�e en tant que source ou destination.
		*/
		ImageSubresourceLayers imageSubresource;
		/**
		*\~english
		*\brief
		*	Selects the initial \p x, \p y, \p z offsets in texels of the sub-region of the source or destination image data.
		*\~french
		*\brief
		*	S�lectionne les d�calages en \p x, \p y, \p z, en texels de la sous-r�gion de l'image source ou destination.
		*/
		IVec3 imageOffset;
		/**
		*\~english
		*\brief
		*	The size in texels of the image to copy in \p width, \p height and \p depth.
		*\~french
		*\brief
		*	Les dimensions en texels de l'image � copier, en tant que \p largeur, \p hauteur, et \p profondeur.
		*/
		UIVec3 imageExtent;
	};
}

#endif
