/*
See LICENSE file in root folder
*/
#ifndef ___CU_ImageLoader_H___
#define ___CU_ImageLoader_H___

#include "CastorUtils/CastorUtilsPrerequisites.hpp"

namespace castor
{
	/**
	\~english
	\brief		An image loader.
	\~french
	\brief		Un loader d'image.
	*/
	class ImageLoaderImpl
	{
	public:
		virtual ~ImageLoaderImpl() = default;
		/**
		 *\~english
		 *\brief		Loads an image file data.
		 *\param[in]	data		The image data.
		 *\param[in]	size		The image data size.
		 *\param[in]	components	The image components to load.
		 *\return		The pixel buffer containing the image data.
		 *\~french
		 *\brief		Charge les données d'un fichier image.
		 *\param[in]	data		Les données de l'image.
		 *\param[in]	size		La taille des données de l'image.
		 *\param[in]	components	Les composantes de l'image à charger.
		 *\return		Le tampon de pixels contenant les données de l'image.
		 */
		CU_API virtual PxBufferPtrArray load( String const & imageFormat
			, uint8_t const * data
			, uint32_t size )const = 0;
	};

	using ImageLoaderPtr = std::unique_ptr< ImageLoaderImpl >;
	/**
	\~english
	\brief		Registers the image loaders, per file extension.
	\~french
	\brief		Enregistre les loaders d'image, par extension de fichier.
	*/
	class ImageLoader
	{
	public:
		/**
		 *\~english
		 *\brief		Registers an image loader.
		 *\param[in]	extension	The image file extension.
		 *\param[in]	loader		The image loader.
		 *\~french
		 *\brief		Enregistre un loader d'image.
		 *\param[in]	extension	L'extension de fichier image.
		 *\param[in]	loader		Le loader.
		 */
		CU_API void registerLoader( String const & extension, ImageLoaderPtr loader );
		/**
		 *\~english
		 *\brief		Registers an image loader.
		 *\param[in]	extensions	The image file extensions list.
		 *\param[in]	loader		The image loader.
		 *\~french
		 *\brief		Enregistre un loader d'image.
		 *\param[in]	extensions	La liste d'extensions de fichier image.
		 *\param[in]	loader		Le loader.
		 */
		CU_API void registerLoader( StringArray const & extensions, ImageLoaderPtr loader );
		/**
		 *\~english
		 *\brief		Unregisters the loader for given image file extension.
		 *\param[in]	extension	The image file extension.
		 *\~french
		 *\brief		Désenregistre le loader pour l'extension de fichier d'image donnée.
		 *\param[in]	extension	L'extension de fichier image.
		 */
		CU_API void unregisterLoader( String const & extension );
		/**
		 *\~english
		 *\brief		Unregisters the loader for given image file extension.
		 *\param[in]	extensions	The image file extensions list.
		 *\~french
		 *\brief		Désenregistre le loader pour l'extension de fichier d'image donnée.
		 *\param[in]	extensions	La liste d'extensions de fichier image.
		 */
		CU_API void unregisterLoader( StringArray const & extensions );
		/**
		 *\~english
		 *\brief		Loads an image file data.
		 *\param[in]	path		The image file path.
		 *\return		The pixel buffer containing the image data.
		 *\~french
		 *\brief		Charge les données d'un fichier image.
		 *\param[in]	path		Le chemin d'accès au fichier image.
		 *\return		Le tampon de pixels contenant les données de l'image.
		 */
		CU_API PxBufferPtrArray load( Path const & path )const;
		/**
		 *\~english
		 *\brief		Loads an image file data.
		 *\param[in]	imageFormat	The image data format.
		 *\param[in]	data		The image data.
		 *\param[in]	size		The image data size.
		 *\return		The pixel buffer containing the image data.
		 *\~french
		 *\brief		Charge les données d'un fichier image.
		 *\param[in]	imageFormat	Le format des données de l'image.
		 *\param[in]	data		Les données de l'image.
		 *\param[in]	size		La taille des données de l'image.
		 *\return		Le tampon de pixels contenant les données de l'image.
		 */
		CU_API PxBufferPtrArray load( String const & imageFormat
			, uint8_t const * data
			, uint32_t size )const;

	private:
		std::vector< ImageLoaderPtr > m_loaders;
		std::map< String, ImageLoaderImpl * > m_extLoaders;
	};
}

#endif
