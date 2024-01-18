/*
See LICENSE file in root folder
*/
#ifndef ___CU_ImageLoader_H___
#define ___CU_ImageLoader_H___

#include "CastorUtils/Graphics/GraphicsModule.hpp"

#include "CastorUtils/Design/ArrayView.hpp"
#include "CastorUtils/Design/NonCopyable.hpp"
#include "CastorUtils/Graphics/Image.hpp"
#include "CastorUtils/Math/Point.hpp"

namespace castor
{
	class ImageLoaderImpl
		: public NonMovable
	{
	public:
		virtual ~ImageLoaderImpl()noexcept = default;
		/**
		 *\~english
		 *\brief		Loads an image file data.
		 *\param[in]	imageFormat	The image format, loader wise.
		 *\param[in]	data		The image data.
		 *\param[in]	size		The image data size.
		 *\param[out]	buffer		Receives the buffer data.
		 *\return		The image layout.
		 *\~french
		 *\brief		Charge les données d'un fichier image.
		 *\param[in]	imageFormat	Le format de l'image, niveau loader.
		 *\param[in]	data		Les données de l'image.
		 *\param[in]	size		La taille des données de l'image.
		 *\param[out]	buffer		Reçoit le buffer.
		 *\return		Le layout de l'image.
		 */
		CU_API virtual ImageLayout load( String const & imageFormat
			, uint8_t const * data
			, uint32_t size
			, PxBufferBaseUPtr & buffer )const = 0;
		/**
		 *\~english
		 *\brief		Loads an image file data.
		 *\param[in]	name		The image name.
		 *\param[in]	imagePath	Path to the image file.
		 *\param[in]	imageFormat	The image format, loader wise.
		 *\param[in]	data		The image data.
		 *\param[in]	size		The image data size.
		 *\return		The image.
		 *\~french
		 *\brief		Charge les données d'un fichier image.
		 *\param[in]	name		Le nom de l'image.
		 *\param[in]	imagePath	Chemin d'accès au fichier.
		 *\param[in]	imageFormat	Le format de l'image, niveau loader.
		 *\param[in]	data		Les données de l'image.
		 *\param[in]	size		La taille des données de l'image.
		 *\return		L'image.
		 */
		CU_API Image load( String const & name
			, Path const & imagePath
			, String const & imageFormat
			, uint8_t const * data
			, uint32_t size )const;
		/**
		 *\~english
		 *\brief		Loads an image file data.
		 *\param[in]	name		The image name.
		 *\param[in]	imageFormat	The image format, loader wise.
		 *\param[in]	data		The image data.
		 *\param[in]	size		The image data size.
		 *\return		The image.
		 *\~french
		 *\brief		Charge les données d'un fichier image.
		 *\param[in]	name		Le nom de l'image.
		 *\param[in]	imageFormat	Le format de l'image, niveau loader.
		 *\param[in]	data		Les données de l'image.
		 *\param[in]	size		La taille des données de l'image.
		 *\return		L'image.
		 */
		CU_API Image load( String const & name
			, String const & imageFormat
			, uint8_t const * data
			, uint32_t size )const;
		/**
		 *\~english
		 *\brief		Loads an image file data.
		 *\param[in]	name		The image name.
		 *\param[in]	imagePath	Path to the image file.
		 *\param[in]	data		The image data.
		 *\param[in]	size		The image data size.
		 *\return		The image.
		 *\~french
		 *\brief		Charge les données d'un fichier image.
		 *\param[in]	name		Le nom de l'image.
		 *\param[in]	imagePath	Cheminvers le fichier de l'image.
		 *\param[in]	data		Les données de l'image.
		 *\param[in]	size		La taille des données de l'image.
		 *\return		L'image.
		 */
		CU_API Image load( String const & name
			, Path const & imagePath
			, uint8_t const * data
			, uint32_t size )const;
	};

	class ImageLoader
		: public NonMovable
	{
	public:
		CU_API explicit ImageLoader( PxCompressionSupport support = {} );
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
		 *\param[in]	name	The image name.
		 *\param[in]	path	The image file path.
		 *\param[in]	config	The loader configuration.
		 *\return		The image.
		 *\~french
		 *\brief		Charge les données d'un fichier image.
		 *\param[in]	name	Le nom de l'image.
		 *\param[in]	path	Le chemin d'accès au fichier image.
		 *\param[in]	config	La configuration du loader.
		 *\return		L'image.
		 */
		CU_API Image load( String const & name
			, Path const & path
			, ImageLoaderConfig const & config )const;
		/**
		 *\~english
		 *\brief		Loads an image file data.
		 *\param[in]	name		The image name.
		 *\param[in]	imagePath	Path to the image file.
		 *\param[in]	data		The image data.
		 *\param[in]	size		The image data size.
		 *\param[in]	config		The loader configuration.
		 *\return		The image.
		 *\~french
		 *\brief		Charge les données d'un fichier image.
		 *\param[in]	name		Le nom de l'image.
		 *\param[in]	imagePath	Chemin d'accès au fichier.
		 *\param[in]	data		Les données de l'image.
		 *\param[in]	size		La taille des données de l'image.
		 *\param[in]	config		La configuration du loader.
		 *\return		L'image.
		 */
		CU_API Image load( String const & name
			, Path const & imagePath
			, uint8_t const * data
			, uint32_t size
			, ImageLoaderConfig const & config )const;
		/**
		 *\~english
		 *\brief		Loads an image file data.
		 *\param[in]	name		The image name.
		 *\param[in]	imageFormat	The image format, loader wise.
		 *\param[in]	data		The image data.
		 *\param[in]	size		The image data size.
		 *\param[in]	config		The loader configuration.
		 *\return		The image.
		 *\~french
		 *\brief		Charge les données d'un fichier image.
		 *\param[in]	name		Le nom de l'image.
		 *\param[in]	imageFormat	Le format de l'image, niveau loader.
		 *\param[in]	data		Les données de l'image.
		 *\param[in]	size		La taille des données de l'image.
		 *\param[in]	config		La configuration du loader.
		 *\return		L'image.
		 */
		CU_API Image load( String const & name
			, String const & imageFormat
			, uint8_t const * data
			, uint32_t size
			, ImageLoaderConfig const & config )const;

		void setCompressionSupport( PxCompressionSupport support )
		{
			m_options.support = std::move( support );
		}

		PxBufferConvertOptions const & getOptions()const
		{
			return m_options;
		}

	private:
		CU_API void checkData( uint8_t const * data
			, uint32_t size )const;
		CU_API ImageLoaderImpl * findLoader( Path const & imagePath )const;
		CU_API ImageLoaderImpl * findLoader( String const & imageFormat )const;

	private:
		std::vector< ImageLoaderPtr > m_loaders;
		StringMap< ImageLoaderImpl * > m_extLoaders;
		PxBufferConvertOptions m_options;
	};
}

#endif
