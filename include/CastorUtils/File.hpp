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
#ifndef ___Castor_File___
#define ___Castor_File___

#include <fstream>
#include "CastorUtils.hpp"
#include "Path.hpp"
#include "Exception.hpp"
#include "Assertion.hpp"

#pragma warning( push)
#pragma warning( disable : 4996)

namespace Castor
{
	/**
	 *\~english
	 *\brief Convert from little or big endian to the other.
	 *\param[in,out]	p_tValue	Data to be converted.
	 *\~french
	 *\brief Change le boutisme du paramètre
	 *\param[in,out]	p_tValue	La valeur dont on veut changer le boutisme.
	 */
	template <class T> inline void SwitchEndianness( T & p_tValue )
	{
		T l_tTmp;
		uint8_t * l_pToConvert = reinterpret_cast< uint8_t * >( & p_tValue );
		uint8_t * l_pConverted = reinterpret_cast< uint8_t * >( & l_tTmp );

		for( std::size_t i = 0; i < sizeof( T ); ++i )
		{
			l_pConverted[i] = l_pToConvert[sizeof( T ) - i - 1];
		}

		p_tValue = l_tTmp;
	}
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
		typedef enum eOPEN_MODE CASTOR_TYPE( uint32_t )
		{	eOPEN_MODE_DUMMY	= 0x00000000	//!<\~english Dummy open mode, not to be used	\~french Mode d'ouverture 'dummy', à ne pas utiliser
		,	eOPEN_MODE_READ		= 0x00000001	//!<\~english Read open mode					\~french Mode d'ouverture en lecture
		,	eOPEN_MODE_WRITE	= 0x00000002	//!<\~english Write open mode					\~french Mode d'ouverture en création / écriture
		,	eOPEN_MODE_APPEND	= 0x00000004	//!<\~english Append open mode					\~french Mode d'ouverture en écriture en fin de fichier
		,	eOPEN_MODE_BINARY	= 0x00000008	//!<\~english Binary open mode					\~french Mode d'ouverture en binaire
		,	eOPEN_MODE_COUNT
		}	eOPEN_MODE;
		/*!
		\~english
		\brief Offset modes enumerator
		\~french
		\brief Modes d'offset pour la fonction seek
		*/
		typedef enum eOFFSET_MODE CASTOR_TYPE( uint8_t )
		{	eOFFSET_MODE_BEGINNING	//!<\~english The offset is set from the beginning of the file	\~french L'offset est défini par rapport au début du fichier
		,	eOFFSET_MODE_CURRENT	//!<\~english The offset is set from the current position		\~french L'offset est défini par rapport à la position actuelle
		,	eOFFSET_MODE_END		//!<\~english The offset is set from the end of the file		\~french L'offset est défini par rapport à la fin du fichier
		}	eOFFSET_MODE;
		/*!
		\~english
		\brief Text file encoding mode
		\~french
		\brief Mode d'encodage des fichiers texte
		*/
		typedef enum eENCODING_MODE CASTOR_TYPE( uint8_t )
		{	eENCODING_MODE_AUTO		//!<\~english Auto select text encoding	\~french Encodage de texte en sélection automatique
		,	eENCODING_MODE_ASCII	//!<\~english ASCII text encoding		\~french Encodage de texte en ASCII
		,	eENCODING_MODE_UNICODE	//!<\~english UNICODE text encoding		\~french Encodage de texte en UNICODE
		,	eENCODING_MODE_UTF8		//!<\~english UTF8 text encoding		\~french Encodage de texte en UTF8
		,	eENCODING_MODE_UTF16	//!<\~english UTF16 text encoding		\~french Encodage de texte en UTF16
		}	eENCODING_MODE;

	protected:
		//!\~english The opening mode							\~french Le mode d'ouverture
		int						m_iMode;
		//!\~english The encoding mode							\~french Le mode d'encodage
		eENCODING_MODE			m_eEncoding;
		//!\~english The full file path							\~french Le chemin d'accès au fichier
		Path					m_strFileFullPath;
		//!\~english The file									\~french Le fichier
		FILE				*	m_pFile;
		//!\~english Tells the FILE is owned by this instance	\~french Dit si le fichier est géré par cette instance
		bool					m_bOwnFile;
		//!\~english The current cursor position in file		\~french La position actuelle du curseur dans le fichier
		unsigned long long		m_ullCursor;
		//!\~english The total file length						\~french La taille totale du fichier
		unsigned long long		m_ullLength;

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
		File( Path const & p_fileName, int p_iMode, eENCODING_MODE p_eEncoding=eENCODING_MODE_ASCII );
	public:
		/**
		 *\~english
		 *\brief		Copy constructor
		 *\~french
		 *\brief		Constructeur par copie
		 */
		File( File const & p_file );
		/**
		 *\~english
		 *\brief		Move constructor
		 *\~french
		 *\brief		Constructeur par déplacement
		 */
		File( File && p_file );
		/**
		 *\~english
		 *\brief		Destructor, closes the file
		 *\~french
		 *\brief		Destructeur, ferme le fichier
		 */
		virtual ~File();
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
		int Seek( long long p_i64Offset, eOFFSET_MODE p_eOrigin=eOFFSET_MODE_BEGINNING );
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_file	The file to copy
		 *\return		A reference to this file
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_file	Le fichier à copier
		 *\return		Une référence sur ce fichier
		 */
		File & operator =( File const & p_file );
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_file	The file to move
		 *\return		A reference to this file
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement
		 *\param[in]	p_file	Le fichier à déplacer
		 *\return		Une référence sur ce fichier
		 */
		File & operator =( File && p_file );
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
		static bool ListDirectoryFiles( Path const & p_folderPath, StringArray & p_files, bool p_recursive = false );
		/**
		 *\~english
		 *\brief		Gives the current directory (id est where the execution is)
		 *\return		The directory
		 *\~french
		 *\brief		Donne le répertoire d'exécution
		 *\return		Le répertoire
		 */
		static Path	DirectoryGetCurrent();
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
		static bool	FileExists( Path const & p_pathFile );
				long long			GetLength		();
				bool 				IsOk			()const;
				long long			Tell			();
		inline 	Path const &		GetFileFullPath	()const { return m_strFileFullPath; }
		inline 	Path				GetFilePath		()const { return m_strFileFullPath.GetPath(); }
		inline 	Path				GetFileName		()const { return m_strFileFullPath.GetFullFileName(); }

	protected:
		DECLARE_INVARIANT_BLOCK()
		uint64_t DoWrite( uint8_t const * p_pBuffer, uint64_t p_uiSize );
		uint64_t DoRead( uint8_t * p_pBuffer, uint64_t p_uiSize );
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\~english
	\brief		Text file class
	\~french
	\brief		Classe de gestion de fichier texte
	*/
	class TextFile : public File
	{
	public:
		/**
		 *\~english
		 *\brief		Opens the file at the given path with the given mode and encoding
		 *\param[in]	p_fileName	The file path
		 *\param[in]	p_iMode		The opening mode, combination of one or more eOPEN_MODE
		 *\param[in]	p_eEncoding	The text encoding mode
		 *\~french
		 *\brief		Ouvre le fichier situé au chemin donné, avec le mode et l'encodage donnés
		 *\param[in]	p_fileName	Le chemin du fichier
		 *\param[in]	p_iMode		Le mode d'ouverture, combinaison d'un ou plusieurs eOPEN_MODE
		 *\param[in]	p_eEncoding	L'encodage pour un fichier en mode texte
		 */
		TextFile( Path const & p_fileName, int p_iMode, eENCODING_MODE p_eEncoding=eENCODING_MODE_ASCII );
		/**
		 *\~english
		 *\brief		Copy constructor
		 *\~french
		 *\brief		Constructeur par copie
		 */
		TextFile( TextFile const & p_file );
		/**
		 *\~english
		 *\brief		Move constructor
		 *\~french
		 *\brief		Constructeur par déplacement
		 */
		TextFile( TextFile && p_file );
		/**
		 *\~english
		 *\brief		Destructor, closes the file
		 *\~french
		 *\brief		Destructeur, ferme le fichier
		 */
		virtual ~TextFile();
		/**
		 *\~english
		 *\brief		Open mode dependant write function
		 *\param[in]	p_toWrite	The data to write in this file
		 *\return		A reference to this file
		 *\~french
		 *\brief		Fonction d'écriture dépendant du mode d'ouverture
		 *\param[in]	p_toWrite	La donnée à écrire dans le fichier
		 *\return		Une référence sur ce fichier
		 */
		template< typename T > TextFile & operator <<( T const & p_toWrite );
		/**
		 *\~english
		 *\brief		Open mode dependant read function
		 *\param[out]	p_toRead	The data to read from this file
		 *\return		A reference to this file
		 *\~french
		 *\brief		Fonction de lecture dépendant du mode d'ouverture
		 *\param[out]	p_toRead	La donnée à lire à partir du fichier
		 *\return		Une référence sur ce fichier
		 */
		template< typename T > TextFile & operator >>( T & p_toRead);
		/**
		 *\~english
		 *\brief		Reads one line from the file (stops at first separator met or when size is read)
		 *\param[out]	p_toRead		Receives the read line
		 *\param[in]	p_size			The maximum line size
		 *\param[in]	p_strSeparators	The list of line separators
		 *\return		The bytes read
		 *\~french
		 *\brief		Lit une ligne à partir du fichier (s'arrête au premier séparateur rencontré ou quand la taille est atteinte)
		 *\param[out]	p_toRead		Reçoit la ligne lue
		 *\param[in]	p_size			La taille maximale de la ligne
		 *\param[in]	p_strSeparators	La liste des séparateurs de ligne
		 *\return		Le nombre d'octets lus
		 */
		uint64_t ReadLine( String & p_toRead, uint64_t p_size, String p_strSeparators=cuT( "\r\n" ) );
		/**
		 *\~english
		 *\brief		Reads one word from the file (stops at first ' ' met)
		 *\param[out]	p_toRead	Receives the read word
		 *\return		The bytes read
		 *\~french
		 *\brief		Lit un mot à partir du fichier (s'arrête au premier ' ' rencontré)
		 *\param[out]	p_toRead	Reçoit le mot lu
		 *\return		Le nombre d'octets lus
		 */
		uint64_t ReadWord( String & p_toRead );
		/**
		 *\~english
		 *\brief		Reads one char from the file
		 *\param[out]	p_toRead	Receives the read char
		 *\return		The bytes read
		 *\~french
		 *\brief		Lit un caractère à partir du fichier
		 *\param[out]	p_toRead	Reçoit le caractère lu
		 *\return		Le nombre d'octets lus
		 */
		uint64_t ReadChar( xchar & p_toRead );
		/**
		 *\~english
		 *\brief		Copies all this file's content into a string
		 *\param[out]	p_strOut	Receives thge content
		 *\return		The bytes read
		 *\~french
		 *\brief		Copie tout le contenu de ce fichier dans une chaîne de caractères
		 *\param[out]	p_strOut	Reçoit le contenu
		 *\return		Le nombre d'octets lus
		 */
		uint64_t CopyToString( String & p_strOut );
		/**
		 *\~english
		 *\brief		Writes a text into the file
		 *\param[in]	p_strText	The text
		 *\return		The bytes written
		 *\~french
		 *\brief		Ecrit une texte dans le fichier
		 *\param[in]	p_strText	Le texte
		 *\return		Le nombre d'octets écrits
		 */
		uint64_t WriteText( String const & p_strText );
		/**
		 *\~english
		 *\brief		Writes a formatted text into the file
		 *\param[in]	p_uiMaxSize	The maximum text size
		 *\param[in]	p_pFormat	The text format
		 *\param[in]	...			POD arguments (cf. printf)
		 *\return		The bytes written
		 *\~french
		 *\brief		Ecrit un texte formaté dans le fichier
		 *\param[in]	p_uiMaxSize	La taille maximale du texte
		 *\param[in]	p_pFormat	Le format du texte
		 *\param[in]	...			Paramètres POD (cf. printf)
		 *\return		Le nombre d'octets écrits
		 */
		uint64_t Print( uint64_t p_uiMaxSize, xchar const * p_pFormat, ... );
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_file	The file to copy
		 *\return		A reference to this file
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_file	Le fichier à copier
		 *\return		Une référence sur ce fichier
		 */
		TextFile & operator =( TextFile const & p_file );
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_file	The file to move
		 *\return		A reference to this file
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement
		 *\param[in]	p_file	Le fichier à déplacer
		 *\return		Une référence sur ce fichier
		 */
		TextFile & operator =( TextFile && p_file );
	};
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
	class BinaryFile : public File
	{
	public:
		/**
		 *\~english
		 *\brief		Opens the file at the given path with the given mode and encoding
		 *\param[in]	p_fileName	The file path
		 *\param[in]	p_iMode		The opening mode, combination of one or more eOPEN_MODE
		 *\~french
		 *\brief		Ouvre le fichier situé au chemin donné, avec le mode et l'encodage donnés
		 *\param[in]	p_fileName	Le chemin du fichier
		 *\param[in]	p_iMode		Le mode d'ouverture, combinaison d'un ou plusieurs eOPEN_MODE
		 */
		BinaryFile( Path const & p_fileName, int p_iMode, eENCODING_MODE p_eEncodingMode=eENCODING_MODE_ASCII );
		/**
		 *\~english
		 *\brief		Copy constructor
		 *\~french
		 *\brief		Constructeur par copie
		 */
		BinaryFile( BinaryFile const & p_file );
		/**
		 *\~english
		 *\brief		Move constructor
		 *\~french
		 *\brief		Constructeur par déplacement
		 */
		BinaryFile( BinaryFile && p_file );
		/**
		 *\~english
		 *\brief		Destructor, closes the file
		 *\~french
		 *\brief		Destructeur, ferme le fichier
		 */
		virtual ~BinaryFile();
		/**
		 *\~english
		 *\brief		Open mode dependant write function
		 *\param[in]	p_toWrite	The data to write in this file
		 *\return		A reference to this file
		 *\~french
		 *\brief		Fonction d'écriture dépendant du mode d'ouverture
		 *\param[in]	p_toWrite	La donnée à écrire dans le fichier
		 *\return		Une référence sur ce fichier
		 */
		template< typename T > BinaryFile & operator <<( T const & p_toWrite );
		/**
		 *\~english
		 *\brief		Open mode dependant read function
		 *\param[out]	p_toRead	The data to read from this file
		 *\return		A reference to this file
		 *\~french
		 *\brief		Fonction de lecture dépendant du mode d'ouverture
		 *\param[out]	p_toRead	La donnée à lire à partir du fichier
		 *\return		Une référence sur ce fichier
		 */
		template< typename T > BinaryFile & operator >>( T & p_toRead);
		/**
		 *\~english
		 *\brief		Writes a string into the file (binary, so writes size then content, without '\0')
		 *\param[in]	p_strToWrite	The string
		 *\return		\p true if written correctly
		 *\~french
		 *\brief		Ecrit une chaîne de caractères dans le fichier (en binaire, donc écrit d'abord la taille puis le contenu, sans le '\0')
		 *\param[in]	p_strToWrite	La chaîne
		 *\return		\p true si l'écriture s'est bien déroulée
		 */
		bool Write( String const & p_strToWrite );
		/**
		 *\~english
		 *\brief		Reads a string from the file (binary, so reads size then content)
		 *\param[out]	p_strToRead	The string
		 *\return		\p true if read correctly
		 *\~french
		 *\brief		Lit une chaîne de caractères à partir du fichier (en binaire, donc lit d'abord la taille puis le contenu)
		 *\param[out]	p_strToRead	La chaîne
		 *\return		\p true si la lecture s'est bien déroulée
		 */
		bool Read( String & p_strToRead );
		/**
		 *\~english
		 *\brief		Writes an object into the file
		 *\param[in]	p_toWrite	The object
		 *\return		The bytes written
		 *\~french
		 *\brief		Ecrit un objet dans le fichier
		 *\param[in]	p_toWrite	L'objet
		 *\return		Le nombre d'octets écrits
		 */
		template< typename T > uint64_t Write( T const & p_toWrite );
		/**
		 *\~english
		 *\brief		Reads an object from the file
		 *\param[out]	p_toRead	The object
		 *\return		The bytes read
		 *\~french
		 *\brief		Lit un objet à partir du fichier
		 *\param[out]	p_toRead	L'objet
		 *\return		Le nombre d'octets lus
		 */
		template< typename T > uint64_t Read( T & p_toRead );
		/**
		 *\~english
		 *\brief		Writes a dynamic array of objects into the file
		 *\param[in]	p_toWrite	The object array
		 *\param[in]	p_count		The array size
		 *\return		The bytes written
		 *\~french
		 *\brief		Ecrit un tableau dynamique d'objets dans du fichier
		 *\param[in]	p_toWrite	Le tableau d'objets
		 *\param[in]	p_count		La taille du tableau
		 *\return		Le nombre d'octets écrits
		 */
		template< typename T > uint64_t WriteArray( T const * p_toWrite, uint64_t p_count );
		/**
		 *\~english
		 *\brief		Writes a static array of objects into the file
		 *\remark		Array size is deduced from parameter
		 *\param[in]	p_toWrite	The object array
		 *\return		The bytes written
		 *\~french
		 *\brief		Ecrit un tableau statique d'objets dans du fichier
		 *\remark		La taille du tableau est déduite à partir du paramètre
		 *\param[in]	p_toWrite	Le tableau d'objets
		 *\return		Le nombre d'octets écrits
		 */
		template< typename T, uint64_t N > uint64_t WriteArray( T const (& p_toWrite)[N] );
		/**
		 *\~english
		 *\brief		Reads a dynamic array of objects from the file
		 *\param[out]	p_toRead	The objects array (must be allocated before call)
		 *\param[in]	p_count		The array size
		 *\return		The bytes read
		 *\~french
		 *\brief		Lit un tableau dynamique d'objets à partir du fichier
		 *\param[out]	p_toRead	Le tableau d'objets (doit être alloué avant l'appel)
		 *\param[in]	p_count		La taille du tableau
		 *\return		Le nombre d'octets lus
		 */
		template< typename T > uint64_t ReadArray( T * p_toRead, uint64_t p_count );
		/**
		 *\~english
		 *\brief		Reads a static array of objects from the file
		 *\param[out]	p_toRead	The objects array
		 *\remark		Array size is deduced from parameter
		 *\return		The bytes read
		 *\~french
		 *\brief		Lit un tableau statique d'objets à partir du fichier
		 *\param[out]	p_toRead	Le tableau d'objets
		 *\remark		La taille du tableau est déduite à partir du paramètre
		 *\return		Le nombre d'octets lus
		 */
		template< typename T, uint64_t N > uint64_t ReadArray( T (& p_toRead)[N] );
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_file	The file to copy
		 *\return		A reference to this file
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_file	Le fichier à copier
		 *\return		Une référence sur ce fichier
		 */
		BinaryFile & operator =( BinaryFile const & p_file );
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_file	The file to move
		 *\return		A reference to this file
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement
		 *\param[in]	p_file	Le fichier à déplacer
		 *\return		Une référence sur ce fichier
		 */
		BinaryFile & operator =( BinaryFile && p_file );
	};
#	include "File.inl"
}

#pragma warning( pop)

#endif
