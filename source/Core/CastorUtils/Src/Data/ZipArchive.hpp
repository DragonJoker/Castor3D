/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_ZIP_ARCHIVE_H___
#define ___CASTOR_ZIP_ARCHIVE_H___

#include "File.hpp"
#include <set>

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
		typedef std::list< Folder > FolderList;

		struct Folder
		{
			String name;
			FolderList folders;
			std::list< String > files;

			Folder();
			Folder( String const & p_name, Path const & p_path );
			Folder * findFolder( Path const & p_path );
			void addFile( Path const & p_path );
			void removeFile( Path const & p_path );
		};


		struct ZipImpl
		{
			virtual void open( Path const & p_path, File::OpenMode p_mode ) = 0;
			virtual void close() = 0;
			virtual void deflate( Folder const & p_files ) = 0;
			virtual StringArray inflate( Path const & p_outFolder, Folder & p_folder ) = 0;
			virtual bool findFolder( String const & p_folder ) = 0;
			virtual bool findFile( String const & p_file ) = 0;
		};

	public:
		/**
		 *\~english
		 *\brief		Default constructor
		 *\param[in]	p_path	The archive path name
		 *\param[in]	p_mode	The open mode
		 *\~french
		 *\brief		Constructeur par défaut
		 *\param[in]	p_path	Le chemin de l' archive
		 *\param[in]	p_mode	Le mode d'ouverture
		 */
		CU_API ZipArchive( Path const & p_path, File::OpenMode p_mode );
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
		 *\param[in]	p_folder	The archive output folder
		 *\~french
		 *\brief		Décompresse l'archive
		 *\param[in]	p_folder	Le dossier de sortie de l'archive
		 */
		CU_API bool inflate( Path const & p_folder );
		/**
		 *\~english
		 *\brief		adds a file to the archive
		 *\param[in]	p_file	The file path
		 *\~french
		 *\brief		Ajoute un fichier à l'archive
		 *\param[in]	p_file	Le chemin du fichier
		 */
		CU_API void addFile( Path const & p_file );
		/**
		 *\~english
		 *\brief		Removes a file for the archive
		 *\param[in]	p_fileName	The file path
		 *\~french
		 *\brief		Retire un fichier de l'archive
		 *\param[in]	p_fileName	Le chemin du fichier
		 */
		CU_API void removeFile( Path const & p_fileName );
		/**
		 *\~english
		 *\brief		Looks for a folder into the archive
		 *\param[in]	p_folder	The folder name
		 *\~french
		 *\brief		Recherche un dossier dans l'archive
		 *\param[in]	p_folder	Le nom du dossier
		 */
		CU_API bool findFolder( String const & p_folder );
		/**
		 *\~english
		 *\brief		Looks for a file into the archive
		 *\param[in]	p_file	The file name
		 *\~french
		 *\brief		Recherche un fichier dans l'archive
		 *\param[in]	p_file	Le nom du fichier
		 */
		CU_API bool findFile( String const & p_file );

	private:
		std::unique_ptr< ZipImpl > m_impl;
		Folder m_uncompressed;
		Path m_rootFolder;
	};
}

#endif
