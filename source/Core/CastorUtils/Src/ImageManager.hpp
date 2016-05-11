/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___CASTOR_IMAGE_MANAGER_H___
#define ___CASTOR_IMAGE_MANAGER_H___

#include "Collection.hpp"
#include "Loader.hpp"

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
	class ImageManager
		: private Collection< Image, String >
	{
	public:
		/*!
		\author		Sylvain DOREMUS
		\version	0.8.0
		\date		29/09/2015
		\~english
		\brief		ImageManager loader.
		\~french
		\brief		Loader de ImageManager.
		*/
		class BinaryLoader
			: public Loader< ImageManager, eFILE_TYPE_BINARY, BinaryFile >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor.
			 *\~french
			 *\brief		Constructeur.
			 */
			CU_API BinaryLoader();
			/**
			 *\~english
			 *\brief			Loads the image manager.
			 *\param[in,out]	p_manager	The manager to load.
			 *\param[in]		p_file		The source file.
			 *\~french
			 *\brief			Charge le gestionnaire d'images.
			 *\param[in,out]	p_manager	Le gestionnaire à charger.
			 *\param[in]		p_file		Le fichier source.
			 */
			CU_API virtual bool operator()( ImageManager & p_manager, BinaryFile & p_file );

		private:
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\~french
		 *\brief		Constructeur.
		 */
		CU_API ImageManager();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		CU_API ~ImageManager();
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
		CU_API ImageSPtr create( String const & p_name, Path const & p_path );
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
		CU_API ImageSPtr create( String const & p_name, Size const & p_size, ePIXEL_FORMAT p_format );
		/**
		 *\~english
		 *\brief		Retrieves a image.
		 *\param[in]	p_name	The image name.
		 *\return		The image, nullptr if not found.
		 *\~french
		 *\brief		Récupère une image.
		 *\param[in]	p_name	Le nom de l'image.
		 *\return		L'image, nullptr si non trouvée.
		 */
		CU_API ImageSPtr get( String const & p_name );

	public:
		using Collection< Image, String >::begin;
		using Collection< Image, String >::end;
		using Collection< Image, String >::lock;
		using Collection< Image, String >::unlock;
		using Collection< Image, String >::clear;
	};
}

#endif

