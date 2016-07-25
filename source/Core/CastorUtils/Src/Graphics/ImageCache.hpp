/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___CASTOR_IMAGE_CACHE_H___
#define ___CASTOR_IMAGE_CACHE_H___

#include "Design/Collection.hpp"

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		15/10/2015
	\~english
	\brief		Image manager.
	\remark		Holds the loaded images, and helps their loading.
	\~french
	\brief		Gestionnaire d'images.
	\remark		Détient les images, et simplifie leur création.
	*/
	class ImageCache
		: private Collection< Image, String >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\~french
		 *\brief		Constructeur.
		 */
		CU_API ImageCache();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		CU_API ~ImageCache();
		/**
		 *\~english
		 *\brief		Creates an image.
		 *\remarks		If the image already exists, it is returned.
		 *\param[in]	p_name		The font name.
		 *\param[in]	p_path		The full access path to the image file.
		 *\return		The created (or retrieved) image.
		 *\~french
		 *\brief		Crée une image.
		 *\remarks		Si la police existe déjà, elle est retournée.
		 *\param[in]	p_name		Le nom de la police.
		 *\param[in]	p_path		Le chemin complet d'accès au fichier de l'image.
		 *\return		L'image créée (ou récupérée).
		 */
		CU_API ImageSPtr Add( String const & p_name, Path const & p_path );
		/**
		 *\~english
		 *\brief		Creates the image with given params.
		 *\remarks		If the image already exists, it is returned.
		 *\param[in]	p_name		The image name.
		 *\param[in]	p_size		The wanted image dimensions.
		 *\param[in]	p_format	The wanted image pixel format.
		 *\~french
		 *\brief		Crée l'image avec les paramètres donnés.
		 *\param[in]	p_name		Le nom de l'image.
		 *\param[in]	p_size		Les dimensions voulues pour l'image.
		 *\param[in]	p_format	Le format de pixel voulu pour l'image.
		 */
		CU_API ImageSPtr Add( String const & p_name, Size const & p_size, ePIXEL_FORMAT p_format );

	public:
		using Collection< Image, String >::begin;
		using Collection< Image, String >::end;
		using Collection< Image, String >::lock;
		using Collection< Image, String >::unlock;
		using Collection< Image, String >::clear;
		using Collection< Image, String >::find;
		using Collection< Image, String >::has;
	};
}

#endif

