/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_IMAGE_CACHE_H___
#define ___CASTOR_IMAGE_CACHE_H___

#include "CastorUtils/Graphics/GraphicsModule.hpp"

#include "CastorUtils/Design/Collection.hpp"

namespace castor
{
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
		CU_API explicit ImageCache( ImageLoader const & loader );
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
		 *\param[in]	name		The image name.
		 *\param[in]	path		The full access path to the image file.
		 *\return		The created (or retrieved) image.
		 *\~french
		 *\brief		Crée une image.
		 *\remarks		Si l'image existe déjà, elle est retournée.
		 *\param[in]	name		Le nom de l'image.
		 *\param[in]	path		Le chemin complet d'accès au fichier de l'image.
		 *\return		L'image créée (ou récupérée).
		 */
		CU_API ImageSPtr add( String const & name
			, Path const & path );
		/**
		 *\~english
		 *\brief		Creates the image with given params.
		 *\remarks		If the image already exists, it is returned.
		 *\param[in]	name	The image name.
		 *\param[in]	size	The wanted image dimensions.
		 *\param[in]	format	The wanted image pixel format.
		 *\~french
		 *\brief		Crée l'image avec les paramètres donnés.
		 *\param[in]	name	Le nom de l'image.
		 *\param[in]	size	Les dimensions voulues pour l'image.
		 *\param[in]	format	Le format de pixel voulu pour l'image.
		 */
		CU_API ImageSPtr add( String const & name
			, Size const & size
			, PixelFormat format );

	public:
		using Collection< Image, String >::begin;
		using Collection< Image, String >::end;
		using Collection< Image, String >::lock;
		using Collection< Image, String >::unlock;
		using Collection< Image, String >::clear;
		using Collection< Image, String >::find;
		using Collection< Image, String >::has;

	private:
		ImageLoader const & m_loader;
	};
}

#endif

