/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_ZIP_ARCHIVE_H___
#define ___CASTOR_ZIP_ARCHIVE_H___

#include "CastorUtils/Data/File.hpp"
#include <list>

namespace castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.1
	\date		04/12/2014
	\~english
	\brief		Zip archive class
	\~french
	\brief		Classe de gestion d'archive zip
	*/
	class ZipArchive
	{
	public:
		struct Folder;
		using FolderList = std::list< Folder >;

		struct Folder
		{
			String name;
			FolderList folders;
			std::list< String > files;

			Folder();
			Folder( String const & name, Path const & path );
			Folder * findFolder( Path const & path );
			void addFile( Path const & path );
			void removeFile( Path const & path );
		};


		struct ZipImpl
		{
			virtual void open( Path const & path, File::OpenMode mode ) = 0;
			virtual void close() = 0;
			virtual void deflate( Folder const & files ) = 0;
			virtual StringArray inflate( Path const & outFolder, Folder & folder ) = 0;
			virtual bool findFolder( String const & folder ) = 0;
			virtual bool findFile( String const & file ) = 0;
		};

	public:
		/**
		 *\~english
		 *\brief		Default constructor
		 *\param[in]	path	The archive path name
		 *\param[in]	mode	The open mode
		 *\~french
		 *\brief		Constructeur par défaut
		 *\param[in]	path	Le chemin de l' archive
		 *\param[in]	mode	Le mode d'ouverture
		 */
		CU_API ZipArchive( Path const & path, File::OpenMode mode );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		CU_API virtual ~ZipArchive();
		/**
		 *\~english
		 *\brief		Deflates an archive
		 *\~french
		 *\brief		Compresse une archive
		 */
		CU_API bool deflate()const;
		/**
		 *\~english
		 *\brief		Inflates the archive
		 *\param[in]	folder	The archive output folder
		 *\~french
		 *\brief		Décompresse l'archive
		 *\param[in]	folder	Le dossier de sortie de l'archive
		 */
		CU_API bool inflate( Path const & folder );
		/**
		 *\~english
		 *\brief		adds a file to the archive
		 *\param[in]	file	The file path
		 *\~french
		 *\brief		Ajoute un fichier à l'archive
		 *\param[in]	file	Le chemin du fichier
		 */
		CU_API void addFile( Path const & file );
		/**
		 *\~english
		 *\brief		Removes a file for the archive
		 *\param[in]	fileName	The file path
		 *\~french
		 *\brief		Retire un fichier de l'archive
		 *\param[in]	fileName	Le chemin du fichier
		 */
		CU_API void removeFile( Path const & fileName );
		/**
		 *\~english
		 *\brief		Looks for a folder into the archive
		 *\param[in]	folder	The folder name
		 *\~french
		 *\brief		Recherche un dossier dans l'archive
		 *\param[in]	folder	Le nom du dossier
		 */
		CU_API bool findFolder( String const & folder );
		/**
		 *\~english
		 *\brief		Looks for a file into the archive
		 *\param[in]	file	The file name
		 *\~french
		 *\brief		Recherche un fichier dans l'archive
		 *\param[in]	file	Le nom du fichier
		 */
		CU_API bool findFile( String const & file );

	private:
		std::unique_ptr< ZipImpl > m_impl;
		Folder m_uncompressed;
		Path m_rootFolder;
	};
}

#endif
