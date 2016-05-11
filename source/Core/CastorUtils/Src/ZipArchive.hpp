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
#ifndef ___CASTOR_ZIP_ARCHIVE_H___
#define ___CASTOR_ZIP_ARCHIVE_H___

#include "File.hpp"
#include <set>

namespace Castor
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
			Folder * FindFolder( Path const & p_path );
			void AddFile( Path const & p_path );
			void RemoveFile( Path const & p_path );
		};


		struct ZipImpl
		{
			virtual void Open( Path const & p_path, File::eOPEN_MODE p_mode ) = 0;
			virtual void Close() = 0;
			virtual void Deflate( Folder const & p_files ) = 0;
			virtual StringArray Inflate( Path const & p_outFolder, Folder & p_folder ) = 0;
			virtual bool FindFolder( String const & p_folder ) = 0;
			virtual bool FindFile( String const & p_file ) = 0;
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
		CU_API ZipArchive( Path const & p_path, File::eOPEN_MODE p_mode );
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
		CU_API bool Deflate()const;
		/**
		 *\~english
		 *\brief		Inflates the archive
		 *\param[in]	p_folder	The archive output folder
		 *\~french
		 *\brief		Décompresse l'archive
		 *\param[in]	p_folder	Le dossier de sortie de l'archive
		 */
		CU_API bool Inflate( Path const & p_folder );
		/**
		 *\~english
		 *\brief		Adds a file to the archive
		 *\param[in]	p_file	The file path
		 *\~french
		 *\brief		Ajoute un fichier à l'archive
		 *\param[in]	p_file	Le chemin du fichier
		 */
		CU_API void AddFile( Path const & p_file );
		/**
		 *\~english
		 *\brief		Removes a file for the archive
		 *\param[in]	p_fileName	The file path
		 *\~french
		 *\brief		Retire un fichier de l'archive
		 *\param[in]	p_fileName	Le chemin du fichier
		 */
		CU_API void RemoveFile( Path const & p_fileName );
		/**
		 *\~english
		 *\brief		Looks for a folder into the archive
		 *\param[in]	p_folder	The folder name
		 *\~french
		 *\brief		Recherche un dossier dans l'archive
		 *\param[in]	p_folder	Le nom du dossier
		 */
		CU_API bool FindFolder( String const & p_folder );
		/**
		 *\~english
		 *\brief		Looks for a file into the archive
		 *\param[in]	p_file	The file name
		 *\~french
		 *\brief		Recherche un fichier dans l'archive
		 *\param[in]	p_file	Le nom du fichier
		 */
		CU_API bool FindFile( String const & p_file );

	private:
		std::unique_ptr< ZipImpl > m_impl;
		Folder m_uncompressed;
		Path m_rootFolder;
	};
}

#endif
