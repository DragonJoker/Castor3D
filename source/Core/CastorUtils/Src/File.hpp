/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

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
#ifndef ___CASTOR_FILE_H___
#define ___CASTOR_FILE_H___

#include <fstream>
#include "CastorUtils.hpp"
#include "Path.hpp"
#include "Exception.hpp"
#include "Assertion.hpp"

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
	 *\brief Convert from little or big endian to the other.
	 *\param[in,out]	p_value	Data to be converted.
	 *\~french
	 *\brief Change le boutisme du paramètre
	 *\param[in,out]	p_value	La valeur dont on veut changer le boutisme.
	 */
	template< class T > inline void SwitchEndianness( T & p_value )
	{
		T l_tTmp;
		uint8_t * l_pToConvert = reinterpret_cast< uint8_t * >( & p_value );
		uint8_t * l_pConverted = reinterpret_cast< uint8_t * >( & l_tTmp );

		for ( std::size_t i = 0; i < sizeof( T ); ++i )
		{
			l_pConverted[i] = l_pToConvert[sizeof( T ) - i - 1];
		}

		p_value = l_tTmp;
	}
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
	 *\param[in]	p_i64Offset	The seek offset
	 *\param[in]	p_iOrigin	The seek origin
	 *\return		\p true on success
	 *\~french
	 *\brief		Déplace le curseur du fichier
	 *\param[out]	p_pFile		Le descripteur du fichier
	 *\param[in]	p_i64Offset	L'indice de déplacement
	 *\param[in]	p_iOrigin	L'origine du déplacement
	 *\return		\p true en cas de réussite
	 */
	CU_API bool FSeek( FILE * p_pFile, int64_t p_i64Offset, int p_iOrigin );
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
		typedef enum eCREATE_MODE
		CASTOR_TYPE( uint32_t )
		{
			eCREATE_MODE_USER_READ = 0x00000001,	//!<\~english Owner can read	\~french Le propriétaire peut lire
			eCREATE_MODE_USER_WRITE = 0x00000002,	//!<\~english Owner can write	\~french Le propriétaire peut écrire
			eCREATE_MODE_USER_EXEC = 0x00000004,	//!<\~english Owner can execute	\~french Le propriétaire peut exécuter
			eCREATE_MODE_GROUP_READ = 0x00000010,	//!<\~english Owner group can read	\~french Le groupe du propriétaire peut lire
			eCREATE_MODE_GROUP_WRITE = 0x00000020,	//!<\~english Owner group can write	\~french Le groupe du propriétaire peut écrire
			eCREATE_MODE_GROUP_EXEC = 0x00000040,	//!<\~english Owner group can execute	\~french Le groupe du propriétaire peut exécuter
			eCREATE_MODE_OTHERS_READ = 0x00000100,	//!<\~english Others can read	\~french Les autres peuvent lire
			eCREATE_MODE_OTHERS_WRITE = 0x00000200,	//!<\~english Others can write	\~french Les autres peuvent écrire
			eCREATE_MODE_OTHERS_EXEC = 0x00000400,	//!<\~english Others can execute	\~french Les autres peuvent exécuter
		}	eCREATE_MODE;
		//!\~english Read, write and execution rights for owner	\~french Droits en lecture, écriture et exécution pour le propriétaire
		static const uint32_t eCREATE_MODE_USER_RWX = eCREATE_MODE_USER_READ | eCREATE_MODE_USER_WRITE | eCREATE_MODE_USER_EXEC;
		//!\~english Read, write and execution rights for owner group	\~french Droits en lecture, écriture et exécution pour le groupe du propriétaire
		static const uint32_t eCREATE_MODE_GROUP_RWX = eCREATE_MODE_GROUP_READ | eCREATE_MODE_GROUP_WRITE | eCREATE_MODE_GROUP_EXEC;
		//!\~english Read, write and execution rights for others	\~french Droits en lecture, écriture et exécution pour les autres
		static const uint32_t eCREATE_MODE_OTHERS_RWX = eCREATE_MODE_OTHERS_READ | eCREATE_MODE_OTHERS_WRITE | eCREATE_MODE_OTHERS_EXEC;
		/*!
		\~english
		\brief Open modes enumerator
		\~french
		\brief Enumérateur des modes d'ouverture
		*/
		typedef enum eOPEN_MODE
		CASTOR_TYPE( uint32_t )
		{
			eOPEN_MODE_DUMMY = 0x00000000,	//!<\~english Dummy open mode, not to be used	\~french Mode d'ouverture 'dummy', à ne pas utiliser
			eOPEN_MODE_READ = 0x00000001,	//!<\~english Read open mode					\~french Mode d'ouverture en lecture
			eOPEN_MODE_WRITE = 0x00000002,	//!<\~english Write open mode					\~french Mode d'ouverture en création / écriture
			eOPEN_MODE_APPEND = 0x00000004,	//!<\~english Append open mode					\~french Mode d'ouverture en écriture en fin de fichier
			eOPEN_MODE_BINARY = 0x00000008,	//!<\~english Binary open mode					\~french Mode d'ouverture en binaire
			eOPEN_MODE_COUNT
		}	eOPEN_MODE;
		/*!
		\~english
		\brief Offset modes enumerator
		\~french
		\brief Modes d'offset pour la fonction seek
		*/
		typedef enum eOFFSET_MODE
		CASTOR_TYPE( uint8_t )
		{
			eOFFSET_MODE_BEGINNING,	//!<\~english The offset is set from the beginning of the file	\~french L'offset est défini par rapport au début du fichier
			eOFFSET_MODE_CURRENT,	//!<\~english The offset is set from the current position		\~french L'offset est défini par rapport à la position actuelle
			eOFFSET_MODE_END,		//!<\~english The offset is set from the end of the file		\~french L'offset est défini par rapport à la fin du fichier
		}	eOFFSET_MODE;
		/*!
		\~english
		\brief Text file encoding mode
		\~french
		\brief Mode d'encodage des fichiers texte
		*/
		typedef enum eENCODING_MODE
		CASTOR_TYPE( uint8_t )
		{
			eENCODING_MODE_AUTO,	//!<\~english Auto select text encoding	\~french Encodage de texte en sélection automatique
			eENCODING_MODE_ASCII,	//!<\~english ASCII text encoding		\~french Encodage de texte en ASCII
			eENCODING_MODE_UTF8,	//!<\~english UTF8 text encoding		\~french Encodage de texte en UTF8
			eENCODING_MODE_UTF16,	//!<\~english UTF16 text encoding		\~french Encodage de texte en UTF16
		}	eENCODING_MODE;

	protected:
		/**
		 *\~english
		 *\brief		Opens the file at the given path with the given mode and encoding
		 *\param[in]	p_fileName	The file path
		 *\param[in]	p_iMode		The opening mode, combination of one or more eOPEN_MODE
		 *\param[in]	p_eEncoding	The file encoding mode
		 *\~french
		 *\brief		Ouvre le fichier situé au chemin donné, avec le mode et l'encodage donnés
		 *\param[in]	p_fileName	Le chemin du fichier
		 *\param[in]	p_iMode		Le mode d'ouverture, combinaison d'un ou plusieurs eOPEN_MODE
		 *\param[in]	p_eEncoding	Le mode d'encodage du fichier
		 */
		CU_API File( Path const & p_fileName, int p_iMode, eENCODING_MODE p_eEncoding = eENCODING_MODE_ASCII );

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
		 *\param[in]	p_i64Offset	The wanted offset
		 *\param[in]	p_eOrigin	The offset mode
		 *\return		\p 0 if successful
		 *\~french
		 *\brief		Fonction de déplacement dans le fichier, selon l'offset et le mode donnés
		 *\param[in]	p_i64Offset	L'offset
		 *\param[in]	p_eOrigin	Le mode
		 *\return		\p 0 si réussi
		 */
		CU_API int Seek( long long p_i64Offset, eOFFSET_MODE p_eOrigin = eOFFSET_MODE_BEGINNING );
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
		CU_API static Path	DirectoryGetCurrent();
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
		CU_API static bool DirectoryCreate( Path const & p_path, uint32_t p_flags = eCREATE_MODE_USER_RWX | eCREATE_MODE_GROUP_RWX | eCREATE_MODE_OTHERS_RWX );
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
		inline 	Path const & GetFileFullPath()const
		{
			return m_strFileFullPath;
		}
		/**
		 *\~english
		 *\brief		Retrieves the full file path
		 *\return		The value
		 *\~french
		 *\brief		Récupère le chemin du fichier
		 *\return		La valeur
		*/
		inline 	Path GetFilePath()const
		{
			return m_strFileFullPath.GetPath();
		}
		/**
		 *\~english
		 *\brief		Retrieves the file name and extension
		 *\return		The value
		 *\~french
		 *\brief		Récupère le nom et l'extension du fichier
		 *\return		La valeur
		*/
		inline 	Path GetFileName()const
		{
			return m_strFileFullPath.GetFullFileName();
		}

	protected:
		DECLARE_INVARIANT_BLOCK()
		CU_API uint64_t DoWrite( uint8_t const * p_pBuffer, uint64_t p_uiSize );
		CU_API uint64_t DoRead( uint8_t * p_pBuffer, uint64_t p_uiSize );

	protected:
		//!\~english The opening mode	\~french Le mode d'ouverture
		int m_iMode;
		//!\~english The encoding mode	\~french Le mode d'encodage
		eENCODING_MODE m_eEncoding;
		//!\~english The full file path	\~french Le chemin d'accès au fichier
		Path m_strFileFullPath;
		//!\~english The file	\~french Le fichier
		FILE * m_pFile;
		//!\~english Tells the FILE is owned by this instance	\~french Dit si le fichier est géré par cette instance
		bool m_bOwnFile;
		//!\~english The current cursor position in file	\~french La position actuelle du curseur dans le fichier
		unsigned long long m_ullCursor;
		//!\~english The total file length	\~french La taille totale du fichier
		unsigned long long m_ullLength;
	};
}

#endif
