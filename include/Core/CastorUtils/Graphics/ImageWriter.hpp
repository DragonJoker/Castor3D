/*
See LICENSE file in root folder
*/
#ifndef ___CU_ImageWriter_H___
#define ___CU_ImageWriter_H___

#include "CastorUtils/Graphics/GraphicsModule.hpp"
#include "CastorUtils/Data/DataModule.hpp"

#include "CastorUtils/Design/NonCopyable.hpp"

namespace castor
{
	class ImageWriterImpl
		: public NonMovable
	{
	public:
		virtual ~ImageWriterImpl()noexcept = default;
		/**
		 *\~english
		 *\brief		Saves an image data to a file.
		 *\param[in]	path	The image file path.
		 *\param[in]	buffer	The image data.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Enregistre les données d'une image dans un fichier.
		 *\param[in]	path	Le chemin d'accès au fichier image.
		 *\param[in]	buffer	Les données de l'image.
		 *\return		\p false si un problème a été rencontré.
		 */
		CU_API virtual bool write( Path const & path
			, PxBufferBase const & buffer )const = 0;
	};

	class ImageWriter
	{
	public:
		/**
		 *\~english
		 *\brief		Registers an image writer.
		 *\param[in]	extension	The image file extension.
		 *\param[in]	writer		The image writer.
		 *\~french
		 *\brief		Enregistre un writer d'image.
		 *\param[in]	extension	L'extension de fichier image.
		 *\param[in]	writer		Le writer.
		 */
		CU_API void registerWriter( String const & extension, ImageWriterPtr writer );
		/**
		 *\~english
		 *\brief		Registers an image writer.
		 *\param[in]	extensions	The image file extensions list.
		 *\param[in]	writer		The image writer.
		 *\~french
		 *\brief		Enregistre un writer d'image.
		 *\param[in]	extensions	La liste d'extensions de fichier image.
		 *\param[in]	writer		Le writer.
		 */
		CU_API void registerWriter( StringArray const & extensions, ImageWriterPtr writer );
		/**
		 *\~english
		 *\brief		Unregisters the writer for given image file extension.
		 *\param[in]	extension	The image file extension.
		 *\~french
		 *\brief		Désenregistre le writer pour l'extension de fichier d'image donnée.
		 *\param[in]	extension	L'extension de fichier image.
		 */
		CU_API void unregisterWriter( String const & extension );
		/**
		 *\~english
		 *\brief		Unregisters the writer for given image file extension.
		 *\param[in]	extensions	The image file extensions list.
		 *\~french
		 *\brief		Désenregistre le writer pour l'extension de fichier d'image donnée.
		 *\param[in]	extensions	La liste d'extensions de fichier image.
		 */
		CU_API void unregisterWriter( StringArray const & extensions );
		/**
		 *\~english
		 *\brief		Saves an image data to a file.
		 *\param[in]	path	The image file path.
		 *\param[in]	buffer	The image data.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Enregistre les données d'une image dans un fichier.
		 *\param[in]	path	Le chemin d'accès au fichier image.
		 *\param[in]	buffer	Les données de l'image.
		 *\return		\p false si un problème a été rencontré.
		 */
		CU_API bool write( Path const & path
			, PxBufferBase const & buffer )const;

	private:
		Vector< ImageWriterPtr > m_writers;
		StringMap< ImageWriterImpl * > m_extWriters;
	};
}

#endif
