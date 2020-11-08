/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_FILE_H___
#define ___CASTOR_FILE_H___

#include "CastorUtils/CastorUtils.hpp"
#include "CastorUtils/Data/Path.hpp"
#include "CastorUtils/Design/FlagCombination.hpp"
#include "CastorUtils/Exception/Exception.hpp"
#include "CastorUtils/Exception/Assertion.hpp"

#include <fstream>

namespace castor
{
	/**
	 *\~english
	 *\brief		Opens a file
	 *\param[out]	file	Receives the file descriptor
	 *\param[in]	path	The file path
	 *\param[in]	mode	The opening mode
	 *\return		\p true on success
	 *\~french
	 *\brief		Ouvre un fichier
	 *\param[out]	file	Reçoit le descripteur du fichier
	 *\param[in]	path	Le chemin d'acès au fichier
	 *\param[in]	mode	Le mode d'ouverture
	 *\return		\p true en cas de réussite
	 */
	CU_API bool fileOpen( FILE *& file, char const * path, char const * mode );
	/**
	 *\~english
	 *\brief		Opens a file
	 *\param[out]	file	Receives the file descriptor
	 *\param[in]	path	The file path
	 *\param[in]	mode	The opening mode
	 *\return		\p true on success
	 *\~french
	 *\brief		Ouvre un fichier
	 *\param[out]	file	Reçoit le descripteur du fichier
	 *\param[in]	path	Le chemin d'acès au fichier
	 *\param[in]	mode	Le mode d'ouverture
	 *\return		\p true en cas de réussite
	 */
	CU_API bool fileOpen64( FILE *& file, char const * path, char const * mode );
	/**
	 *\~english
	 *\brief		Seeks into a file
	 *\param[in]	file	The file descriptor
	 *\param[in]	offset	The seek offset
	 *\param[in]	origin	The seek origin
	 *\return		\p true on success
	 *\~french
	 *\brief		Déplace le curseur du fichier
	 *\param[out]	file	Le descripteur du fichier
	 *\param[in]	offset	L'indice de déplacement
	 *\param[in]	origin	L'origine du déplacement
	 *\return		\p true en cas de réussite
	 */
	CU_API bool fileSeek( FILE * file, int64_t offset, int origin );
	/**
	 *\~english
	 *\brief		Retrieves the file cursor position
	 *\param[out]	file	The file descriptor
	 *\return		The position
	 *\~french
	 *\brief		Récupère la position du curseur dans un fichier
	 *\param[out]	file	Le descripteur du fichier
	 *\return		La position
	 */
	CU_API int64_t fileTell( FILE * file );

	class File
	{
	public:
		/*!
		\~english
		\brief open modes enumerator
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
			//!\~english	read, write and execution rights for owner.
			//!\~french		Droits en lecture, écriture et exécution pour le propriétaire.
			eUserRWX = eUserRead | eUserWrite | eUserExec,
			//!\~english	read, write and execution rights for owner group.
			//!\~french		Droits en lecture, écriture et exécution pour le groupe du propriétaire.
			eGroupRWX = eGroupRead | eGroupWrite | eGroupExec,
			//!\~english	read, write and execution rights for others.
			//!\~french		Droits en lecture, écriture et exécution pour les autres.
			eOthersRWX = eOthersRead | eOthersWrite | eOthersExec,
			//!\~english	read, write and execution rights for all users.
			//!\~french		Droits en lecture, écriture et exécution pour tous les utilisateurs.
			eAllRWX = eUserRWX | eGroupRWX | eOthersRWX,
		};
		/*!
		\~english
		\brief open modes enumerator
		\~french
		\brief Enumérateur des modes d'ouverture
		*/
		enum class OpenMode
			: uint32_t
		{
			//!\~english	Dummy open mode, not to be used.
			//!\~french		Mode d'ouverture 'dummy', à ne pas utiliser.
			eDummy = 0x00000000,
			//!\~english	read open mode.
			//!\~french		Mode d'ouverture en lecture.
			eRead = 0x00000001,
			//!\~english	write open mode.
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
			CU_ScopedEnumBounds( eBeginning )
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
			CU_ScopedEnumBounds( eAuto )
		};

	protected:
		/**
		 *\~english
		 *\brief		Opens the file at the given path with the given mode and encoding
		 *\param[in]	filePath	The file path
		 *\param[in]	mode		The opening mode, combination of one or more OpenMode
		 *\param[in]	encoding	The file encoding mode
		 *\~french
		 *\brief		Ouvre le fichier situé au chemin donné, avec le mode et l'encodage donnés
		 *\param[in]	filePath	Le chemin du fichier
		 *\param[in]	mode		Le mode d'ouverture, combinaison d'un ou plusieurs OpenMode
		 *\param[in]	encoding	Le mode d'encodage du fichier
		 */
		CU_API File( Path const & filePath
			, FlagCombination< OpenMode > const & mode
			, EncodingMode encoding = EncodingMode::eASCII );

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
		 *\brief		sets the cursor in the file according to the given offset and the given mode
		 *\param[in]	offset	The wanted offset
		 *\param[in]	origin	The offset mode
		 *\return		\p 0 if successful
		 *\~french
		 *\brief		Fonction de déplacement dans le fichier, selon l'offset et le mode donnés
		 *\param[in]	offset	L'offset
		 *\param[in]	origin	Le mode
		 *\return		\p 0 si réussi
		 */
		CU_API int seek( long long offset
			, OffsetMode origin = OffsetMode::eBeginning );
		using TraverseDirFunction = std::function< bool( Path const & path ) >;
		using HitFileFunction = std::function< void( Path const & folder, String const & name ) >;
		using FilterFunction = std::function< bool( Path const & folder, String const & name ) >;
		/**
		*\brief
		*	Traverses the files and directories of a directory.
		*\param[in] folderPath
		*	The directory path.
		*\param[in] directoryFunction
		*	returns \p true to traverse it, \p false to ignore.
		*\param[in] fileFunction
		*	Placeholder to handle a file name.
		*\return
		*	\p false if any error occured.
		*/
		CU_API static bool traverseDirectory( Path const & folderPath
			, TraverseDirFunction directoryFunction
			, HitFileFunction fileFunction );
		/**
		*\brief
		*	Filters the files in a directory, recursively or not.
		*\param[in] folderPath
		*	The directory path.
		*\param[in] onFile
		*	The filter function, returns \p true to add to the list, \p false to ignore.
		*\param[in] recursive
		*	Tells if search must be recursive.
		*\return
		*	The files list.
		*/
		CU_API static PathArray filterDirectoryFiles( Path const & folderPath
			, FilterFunction onFile
			, bool recursive = false );
		/**
		 *\~english
		 *\brief		List all files in a directory, recursively or not
		 *\param[in]	folderPath	The directory path
		 *\param[out]	files		Receives the files list
		 *\param[in]	recursive	Tells if search must be recursive
		 *\return		\p true if the directory is listed
		 *\~french
		 *\brief		Liste tous les fichiers d'un répertoire, récursivement ou pas
		 *\param[in]	folderPath	Le chemin du dossier
		 *\param[out]	files		Reçoit la liste des fichiers
		 *\param[in]	recursive	Définit si la recherche doit être récursive ou pas
		 *\return		\p true si le dossier a été listé
		 */
		CU_API static bool listDirectoryFiles( Path const & folderPath
			, PathArray & files
			, bool recursive = false );
		/**
		 *\~english
		 *\brief		Gives the current directory (id est where the execution is)
		 *\return		The directory
		 *\~french
		 *\brief		donne le répertoire d'exécution
		 *\return		Le répertoire
		 */
		CU_API static Path getExecutableDirectory();
		/**
		 *\~english
		 *\brief		Gives the user home directory
		 *\return		The directory
		 *\~french
		 *\brief		donne le répertoire de l'utilisateur courant
		 *\return		Le répertoire
		 */
		CU_API static Path	getUserDirectory();
		/**
		 *\~english
		 *\brief		Tests directory existence
		 *\param[in]	folderPath	Directory path
		 *\return		\p true if directory is found
		 *\~french
		 *\brief		Teste l'existence d'un dossier
		 *\param[in]	folderPath	Le chemin du dossier
		 *\return		\p true si le dossier existe
		 */
		CU_API static bool	directoryExists( Path const & folderPath );
		/**
		 *\~english
		 *\brief		Creates a directory
		 *\param[in]	folderPath	Directory path
		 *\param[in]	flags		User rights
		 *\return		\p true if the directory has been created
		 *\~french
		 *\brief		Crée un dossier
		 *\param[in]	folderPath	Le chemin du dossier
		 *\param[in]	flags		Les droits d'utilisation
		 *\return		\p true si le dossier a été créé
		 */
		CU_API static bool directoryCreate( Path const & folderPath
			, FlagCombination< CreateMode > const & flags = CreateMode::eAllRWX );
		/**
		 *\~english
		 *\brief		Deletes an empty directory
		 *\param[in]	folderPath	Directory path
		 *\return		\p true if the directory has been deleted
		 *\~french
		 *\brief		Supprime un dossier vide
		 *\param[in]	folderPath	Le chemin du dossier
		 *\return		\p true si le dossier a été supprimé
		 */
		CU_API static bool directoryDelete( Path const & folderPath );
		/**
		 *\~english
		 *\brief		Tests file existence
		 *\param[in]	filePath	File name and path
		 *\return		\p true if file is found
		 *\~french
		 *\brief		Teste l'existence d'un fichier
		 *\param[in]	filePath	Le chemin du fichier
		 *\return		\p true si le fichier existe
		 */
		CU_API static bool fileExists( Path const & filePath );
		/**
		 *\~english
		 *\brief		Deletes a file
		 *\param[in]	filePath	File name and path
		 *\return		\p true if file has been correctly deleted
		 *\~french
		 *\brief		Supprime un fichier
		 *\param[in]	filePath	Le chemin du fichier
		 *\return		\p true si le fichier a été supprimé correctement
		 */
		CU_API static bool deleteFile( Path const & filePath );
		/**
		 *\~english
		 *\brief		Copy a file into a folder
		 *\param[in]	filePath	File name and path
		 *\param[in]	folderPath	The destination folder
		 *\return		\p true if file has been correctly copied
		 *\~french
		 *\brief		Copie un fichier dans un dossier
		 *\param[in]	filePath	Le chemin du fichier
		 *\param[in]	folderPath	Le dossier de destination
		 *\return		\p true si le fichier a été copié correctement
		 */
		CU_API static bool copyFile( Path const & filePath
			, Path const & folderPath );
		/**
		 *\~english
		 *\brief		Retrieves the file size
		 *\return		The value
		 *\~french
		 *\brief		Récupère la taille du fichier
		 *\return		La valeur
		*/
		CU_API long long getLength();
		/**
		 *\~english
		 *\brief		Tells if the file cursor is OK
		 *\return		\p true if the file cursor is not in error and not EOF
		 *\~french
		 *\brief		Dit si le curseur du fichier est OK
		 *\return		\p true si le curseur du fichier n'est pas en erreur ni EOF
		*/
		CU_API bool isOk()const;
		/**
		 *\~english
		 *\brief		Retrieves the file cursor position
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position du curseur du fichier
		 *\return		La valeur
		*/
		CU_API long long tell();
		/**
		 *\~english
		 *\brief		Retrieves the full file path, name and extension
		 *\return		The value
		 *\~french
		 *\brief		Récupère le chemin, nom et extension du fichier
		 *\return		La valeur
		*/
		inline	Path const & getFileFullPath()const
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
		inline Path getFilePath()const
		{
			return m_fileFullPath.getPath();
		}
		/**
		 *\~english
		 *\brief		Retrieves the file name and extension
		 *\return		The value
		 *\~french
		 *\brief		Récupère le nom et l'extension du fichier
		 *\return		La valeur
		*/
		inline Path getFileName()const
		{
			return Path{ m_fileFullPath.getFullFileName() };
		}

	protected:
		CU_DeclareInvariantBlock();
		CU_API uint64_t doWrite( uint8_t const * buffer, uint64_t size );
		CU_API uint64_t doRead( uint8_t * buffer, uint64_t size );

	private:
		CU_API static bool deleteEmptyDirectory( Path const & folderPath );

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
	CU_ImplementClassFlags( File, OpenMode )
	CU_ImplementClassFlags( File, CreateMode )
}

#endif
