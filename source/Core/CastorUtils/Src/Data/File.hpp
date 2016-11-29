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
#ifndef ___CASTOR_FILE_H___
#define ___CASTOR_FILE_H___

#include <fstream>
#include "CastorUtils.hpp"
#include "Path.hpp"
#include "Design/FlagCombination.hpp"
#include "Exception/Exception.hpp"
#include "Exception/Assertion.hpp"

#if defined( DeleteFile )
#	undef DeleteFile
#endif
#if defined( CopyFile )
#	undef CopyFile
#endif

namespace Castor
{
	/**
	 *\~english
	 *\brief		Opens a file
	 *\param[out]	p_pFile		Receives the file descriptor
	 *\param[in]	p_pszPath	The file path
	 *\param[in]	p_pszMode	The opening mode
	 *\return		\p true on success
	 *\~french
	 *\brief		Ouvre un fichier
	 *\param[out]	p_pFile		Reçoit le descripteur du fichier
	 *\param[in]	p_pszPath	Le chemin d'acès au fichier
	 *\param[in]	p_pszMode	Le mode d'ouverture
	 *\return		\p true en cas de réussite
	 */
	CU_API bool FOpen( FILE *& p_pFile, char const * p_pszPath, char const * p_pszMode );
	/**
	 *\~english
	 *\brief		Opens a file
	 *\param[out]	p_pFile		Receives the file descriptor
	 *\param[in]	p_pszPath	The file path
	 *\param[in]	p_pszMode	The opening mode
	 *\return		\p true on success
	 *\~french
	 *\brief		Ouvre un fichier
	 *\param[out]	p_pFile		Reçoit le descripteur du fichier
	 *\param[in]	p_pszPath	Le chemin d'acès au fichier
	 *\param[in]	p_pszMode	Le mode d'ouverture
	 *\return		\p true en cas de réussite
	 */
	CU_API bool FOpen64( FILE *& p_pFile, char const * p_pszPath, char const * p_pszMode );
	/**
	 *\~english
	 *\brief		Seeks into a file
	 *\param[in]	p_pFile		The file descriptor
	 *\param[in]	p_offset	The seek offset
	 *\param[in]	p_iOrigin	The seek origin
	 *\return		\p true on success
	 *\~french
	 *\brief		Déplace le curseur du fichier
	 *\param[out]	p_pFile		Le descripteur du fichier
	 *\param[in]	p_offset	L'indice de déplacement
	 *\param[in]	p_iOrigin	L'origine du déplacement
	 *\return		\p true en cas de réussite
	 */
	CU_API bool FSeek( FILE * p_pFile, int64_t p_offset, int p_iOrigin );
	/**
	 *\~english
	 *\brief		Retrieves the file cursor position
	 *\param[out]	p_pFile		The file descriptor
	 *\return		The position
	 *\~french
	 *\brief		Récupère la position du curseur dans un fichier
	 *\param[out]	p_pFile		Le descripteur du fichier
	 *\return		La position
	 */
	CU_API int64_t FTell( FILE * p_pFile );
	/*!
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 03/01/2011
	\~english
	\brief User friendly File class
	\remark Adds some static functions to check file/directory validity, file/directory creation...
	\~french
	\brief Classe de gestion de fichier
	\remark Ajoute quelques fonctions statiques de vérification d'existence, suppression, ...
	*/
	class File
	{
	public:
		/*!
		\~english
		\brief Open modes enumerator
		\~french
		\brief Enumérateur des modes d'ouverture
		*/
		enum class CreateMode
			: uint32_t
		{
			//!\~english	Owner can read.
			//!\~french		Le propriétaire peut lire.
			eUserRead = 0x00000001,
			//!\~english	Owner can write.
			//!\~french		Le propriétaire peut écrire.
			eUserWrite = 0x00000002,
			//!\~english	Owner can execute.
			//!\~french		Le propriétaire peut exécuter.
			eUserExec = 0x00000004,
			//!\~english	Owner group can read.
			//!\~french		Le groupe du propriétaire peut lire.
			eGroupRead = 0x00000010,
			//!\~english	Owner group can write.
			//!\~french		Le groupe du propriétaire peut écrire.
			eGroupWrite = 0x00000020,
			//!\~english	Owner group can execute.
			//!\~french		Le groupe du propriétaire peut exécuter.
			eGroupExec = 0x00000040,
			//!\~english	Others can read.
			//!\~french		Les autres peuvent lire.
			eOthersRead = 0x00000100,
			//!\~english	Others can write.
			//!\~french		Les autres peuvent écrire.
			eOthersWrite = 0x00000200,
			//!\~english	Others can execute.
			//!\~french		Les autres peuvent exécuter.
			eOthersExec = 0x00000400,
			//!\~english	Read, write and execution rights for owner.
			//!\~french		Droits en lecture, écriture et exécution pour le propriétaire.
			eUserRWX = eUserRead | eUserWrite | eUserExec,
			//!\~english	Read, write and execution rights for owner group.
			//!\~french		Droits en lecture, écriture et exécution pour le groupe du propriétaire.
			eGroupRWX = eGroupRead | eGroupWrite | eGroupExec,
			//!\~english	Read, write and execution rights for others.
			//!\~french		Droits en lecture, écriture et exécution pour les autres.
			eOthersRWX = eOthersRead | eOthersWrite | eOthersExec,
			//!\~english	Read, write and execution rights for all users.
			//!\~french		Droits en lecture, écriture et exécution pour tous les utilisateurs.
			eAllRWX = eUserRWX | eGroupRWX | eOthersRWX,
		};
		/*!
		\~english
		\brief Open modes enumerator
		\~french
		\brief Enumérateur des modes d'ouverture
		*/
		enum class OpenMode
			: uint32_t
		{
			//!\~english	Dummy open mode, not to be used.
			//!\~french		Mode d'ouverture 'dummy', à ne pas utiliser.
			eDummy = 0x00000000,
			//!\~english	Read open mode.
			//!\~french		Mode d'ouverture en lecture.
			eRead = 0x00000001,
			//!\~english	Write open mode.
			//!\~french		Mode d'ouverture en création / écriture.
			eWrite = 0x00000002,
			//!\~english	Append open mode.
			//!\~french		Mode d'ouverture en écriture en fin de fichier.
			eAppend = 0x00000004,
			//!\~english	Binary open mode.
			//!\~french		Mode d'ouverture en binaire.
			eBinary = 0x00000008,
		};
		/*!
		\~english
		\brief Offset modes enumerator
		\~french
		\brief Modes d'offset pour la fonction seek
		*/
		enum class OffsetMode
			: uint8_t
		{
			//!\~english	The offset is set from the beginning of the file.
			//!\~french		L'offset est défini par rapport au début du fichier.
			eBeginning,
			//!\~english	The offset is set from the current position.
			//!\~french		L'offset est défini par rapport à la position actuelle.
			eCurrent,
			//!\~english	The offset is set from the end of the file.
			//!\~french		L'offset est défini par rapport à la fin du fichier.
			eEnd,
			CASTOR_SCOPED_ENUM_BOUNDS( eBeginning )
		};
		/*!
		\~english
		\brief Text file encoding mode
		\~french
		\brief Mode d'encodage des fichiers texte
		*/
		enum class EncodingMode
			: uint8_t
		{
			//!\~english	Auto select text encoding.
			//!\~french		Encodage de texte en sélection automatique.
			eAuto,
			//!\~english	ASCII text encoding.
			//!\~french		Encodage de texte en ASCII.
			eASCII,
			//!\~english	UTF8 text encoding.
			//!\~french		Encodage de texte en UTF8.
			eUTF8,
			//!\~english	UTF16 text encoding.
			//!\~french		Encodage de texte en UTF16.
			eUTF16,
			CASTOR_SCOPED_ENUM_BOUNDS( eAuto )
		};

	protected:
		/**
		 *\~english
		 *\brief		Opens the file at the given path with the given mode and encoding
		 *\param[in]	p_fileName	The file path
		 *\param[in]	p_mode		The opening mode, combination of one or more OpenMode
		 *\param[in]	p_eEncoding	The file encoding mode
		 *\~french
		 *\brief		Ouvre le fichier situé au chemin donné, avec le mode et l'encodage donnés
		 *\param[in]	p_fileName	Le chemin du fichier
		 *\param[in]	p_mode		Le mode d'ouverture, combinaison d'un ou plusieurs OpenMode
		 *\param[in]	p_eEncoding	Le mode d'encodage du fichier
		 */
		CU_API File( Path const & p_fileName, FlagCombination< OpenMode > const & p_mode, EncodingMode p_encoding = EncodingMode::eASCII );

	public:
		/**
		 *\~english
		 *\brief		Destructor, closes the file
		 *\~french
		 *\brief		Destructeur, ferme le fichier
		 */
		CU_API virtual ~File();
		/**
		 *\~english
		 *\brief		Seek function, sets the cursor in the file according to the given offset and the given mode
		 *\param[in]	p_offset	The wanted offset
		 *\param[in]	p_origin	The offset mode
		 *\return		\p 0 if successful
		 *\~french
		 *\brief		Fonction de déplacement dans le fichier, selon l'offset et le mode donnés
		 *\param[in]	p_offset	L'offset
		 *\param[in]	p_origin	Le mode
		 *\return		\p 0 si réussi
		 */
		CU_API int Seek( long long p_offset, OffsetMode p_origin = OffsetMode::eBeginning );
		/**
		 *\~english
		 *\brief		List all files in a directory, recursively or not
		 *\param[in]	p_folderPath	The directory path
		 *\param[out]	p_files			Receives the files list
		 *\param[in]	p_recursive		Tells if search must be recursive
		 *\return		\p true if the directory is listed
		 *\~french
		 *\brief		Liste tous les fichiers d'un répertoire, récursivement ou pas
		 *\param[in]	p_folderPath	Le chemin du dossier
		 *\param[out]	p_files			Reçoit la liste des fichiers
		 *\param[in]	p_recursive		Définit si la recherche doit être récursive ou pas
		 *\return		\p true si le dossier a été listé
		 */
		CU_API static bool ListDirectoryFiles( Path const & p_folderPath, PathArray & p_files, bool p_recursive = false );
		/**
		 *\~english
		 *\brief		Gives the current directory (id est where the execution is)
		 *\return		The directory
		 *\~french
		 *\brief		Donne le répertoire d'exécution
		 *\return		Le répertoire
		 */
		CU_API static Path GetExecutableDirectory();
		/**
		 *\~english
		 *\brief		Gives the user home directory
		 *\return		The directory
		 *\~french
		 *\brief		Donne le répertoire de l'utilisateur courant
		 *\return		Le répertoire
		 */
		CU_API static Path	GetUserDirectory();
		/**
		 *\~english
		 *\brief		Tests directory existence
		 *\param[in]	p_path	Directory path
		 *\return		\p true if directory is found
		 *\~french
		 *\brief		Teste l'existence d'un dossier
		 *\param[in]	p_path	Le chemin du dossier
		 *\return		\p true si le dossier existe
		 */
		CU_API static bool	DirectoryExists( Path const & p_path );
		/**
		 *\~english
		 *\brief		Creates a directory
		 *\param[in]	p_path	Directory path
		 *\param[in]	p_flags	User rights
		 *\return		\p true if the directory has been created
		 *\~french
		 *\brief		Crée un dossier
		 *\param[in]	p_path	Le chemin du dossier
		 *\param[in]	p_flags	Les droits d'utilisation
		 *\return		\p true si le dossier a été créé
		 */
		CU_API static bool DirectoryCreate( Path const & p_path, FlagCombination< CreateMode > const & p_flags = CreateMode::eAllRWX );
		/**
		 *\~english
		 *\brief		Deletes an empty directory
		 *\param[in]	p_path	Directory path
		 *\return		\p true if the directory has been deleted
		 *\~french
		 *\brief		Supprime un dossier vide
		 *\param[in]	p_path	Le chemin du dossier
		 *\return		\p true si le dossier a été supprimé
		 */
		CU_API static bool DirectoryDelete( Path const & p_path );
		/**
		 *\~english
		 *\brief		Tests file existence
		 *\param[in]	p_pathFile	File name and path
		 *\return		\p true if file is found
		 *\~french
		 *\brief		Teste l'existence d'un fichier
		 *\param[in]	p_pathFile	Le chemin du fichier
		 *\return		\p true si le fichier existe
		 */
		CU_API static bool	FileExists( Path const & p_pathFile );
		/**
		 *\~english
		 *\brief		Deletes a file
		 *\param[in]	p_file	    File name and path
		 *\return		\p true if file has been correctly deleted
		 *\~french
		 *\brief		Supprime un fichier
		 *\param[in]	p_file	    Le chemin du fichier
		 *\return		\p true si le fichier a été supprimé correctement
		 */
		CU_API static bool	DeleteFile( Path const & p_file );
		/**
		 *\~english
		 *\brief		Copy a file into a folder
		 *\param[in]	p_file	    File name and path
		 *\param[in]	p_folder	The destination folder
		 *\return		\p true if file has been correctly copied
		 *\~french
		 *\brief		Copie un fichier dans un dossier
		 *\param[in]	p_file	    Le chemin du fichier
		 *\param[in]	p_folder	Le dossier de destination
		 *\return		\p true si le fichier a été copié correctement
		 */
		CU_API static bool	CopyFile( Path const & p_file, Path const & p_folder );
		/**
		 *\~english
		 *\brief		Retrieves the file size
		 *\return		The value
		 *\~french
		 *\brief		Récupère la taille du fichier
		 *\return		La valeur
		*/
		CU_API long long GetLength();
		/**
		 *\~english
		 *\brief		Tells if the file cursor is OK
		 *\return		\p true if the file cursor is not in error and not EOF
		 *\~french
		 *\brief		Dit si le curseur du fichier est OK
		 *\return		\p true si le curseur du fichier n'est pas en erreur ni EOF
		*/
		CU_API bool IsOk()const;
		/**
		 *\~english
		 *\brief		Retrieves the file cursor position
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position du curseur du fichier
		 *\return		La valeur
		*/
		CU_API long long Tell();
		/**
		 *\~english
		 *\brief		Retrieves the full file path, name and extension
		 *\return		The value
		 *\~french
		 *\brief		Récupère le chemin, nom et extension du fichier
		 *\return		La valeur
		*/
		inline	Path const & GetFileFullPath()const
		{
			return m_fileFullPath;
		}
		/**
		 *\~english
		 *\brief		Retrieves the full file path
		 *\return		The value
		 *\~french
		 *\brief		Récupère le chemin du fichier
		 *\return		La valeur
		*/
		inline Path GetFilePath()const
		{
			return m_fileFullPath.GetPath();
		}
		/**
		 *\~english
		 *\brief		Retrieves the file name and extension
		 *\return		The value
		 *\~french
		 *\brief		Récupère le nom et l'extension du fichier
		 *\return		La valeur
		*/
		inline Path GetFileName()const
		{
			return Path{ m_fileFullPath.GetFullFileName() };
		}

	protected:
		DECLARE_INVARIANT_BLOCK()
		CU_API uint64_t DoWrite( uint8_t const * p_buffer, uint64_t p_uiSize );
		CU_API uint64_t DoRead( uint8_t * p_buffer, uint64_t p_uiSize );

	protected:
		//!\~english	The opening mode.
		//!\~french		Le mode d'ouverture.
		FlagCombination< OpenMode > m_mode{ 0u };
		//!\~english	The encoding mode.
		//!\~french		Le mode d'encodage.
		EncodingMode m_encoding{ EncodingMode::eASCII };
		//!\~english	The full file path.
		//!\~french		Le chemin d'accès au fichier.
		Path m_fileFullPath;
		//!\~english	The file.
		//!\~french		Le fichier.
		FILE * m_file{ nullptr };
		//!\~english	The current cursor position in file.
		//!\~french		La position actuelle du curseur dans le fichier.
		uint64_t m_cursor{ 0 };
		//!\~english	The total file length.
		//!\~french		La taille totale du fichier.
		uint64_t m_length{ 0 };
	};
	IMPLEMENT_FLAGS( File::OpenMode )
	IMPLEMENT_FLAGS( File::CreateMode )
}

#endif
