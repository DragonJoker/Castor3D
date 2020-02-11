/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_TEXT_FILE_H___
#define ___CASTOR_TEXT_FILE_H___

#include "CastorUtils/Data/File.hpp"

namespace castor
{
	/**
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		24/05/2016
	\~english
	\brief		Helper structure to find File type (BinaryFile or TextFile) from FileType.
	\remarks	Specialisation for FileType::eText.
	\~french
	\brief		Structure d'aide pour trouver le type de File (BinaryFile ou TextFile) à partir d'un FileType.
	\remarks	Spécialisation pour FileType::eText.
	*/
	template<>
	struct FileTyper< FileType::eText >
	{
		using Type = TextFile;
	};
	/**
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\~english
	\brief		Text file class
	\~french
	\brief		Classe de gestion de fichier texte
	*/
	class TextFile
		: public File
	{
	public:
		/**
		 *\~english
		 *\brief		Opens the file at the given path with the given mode and encoding
		 *\param[in]	fileName	The file path
		 *\param[in]	mode		The opening mode, combination of one or more OpenMode
		 *\param[in]	encoding	The text encoding mode
		 *\~french
		 *\brief		Ouvre le fichier situé au chemin donné, avec le mode et l'encodage donnés
		 *\param[in]	fileName	Le chemin du fichier
		 *\param[in]	mode		Le mode d'ouverture, combinaison d'un ou plusieurs OpenMode
		 *\param[in]	encoding	L'encodage pour un fichier en mode texte
		 */
		CU_API TextFile( Path const & fileName
			, FlagCombination< OpenMode > const & mode
			, EncodingMode encoding = EncodingMode::eASCII );
		/**
		 *\~english
		 *\brief		Destructor, closes the file
		 *\~french
		 *\brief		Destructeur, ferme le fichier
		 */
		CU_API virtual ~TextFile();
		/**
		 *\~english
		 *\brief		Reads one line from the file (stops at first separator met or when size is read)
		 *\param[out]	toRead		Receives the read line
		 *\param[in]	size		The maximum line size
		 *\param[in]	separators	The list of line separators
		 *\return		The bytes read
		 *\~french
		 *\brief		Lit une ligne à partir du fichier (s'arrête au premier séparateur rencontré ou quand la taille est atteinte)
		 *\param[out]	toRead		Reçoit la ligne lue
		 *\param[in]	size		La taille maximale de la ligne
		 *\param[in]	separators	La liste des séparateurs de ligne
		 *\return		Le nombre d'octets lus
		 */
		CU_API uint64_t readLine( String & toRead, uint64_t size, String separators = cuT( "\r\n" ) );
		/**
		 *\~english
		 *\brief		Reads one word from the file (stops at first ' ' met)
		 *\param[out]	toRead	Receives the read word
		 *\return		The bytes read
		 *\~french
		 *\brief		Lit un mot à partir du fichier (s'arrête au premier ' ' rencontré)
		 *\param[out]	toRead	Reçoit le mot lu
		 *\return		Le nombre d'octets lus
		 */
		CU_API uint64_t readWord( String & toRead );
		/**
		 *\~english
		 *\brief		Reads one char from the file
		 *\param[out]	toRead	Receives the read char
		 *\return		The bytes read
		 *\~french
		 *\brief		Lit un caractère à partir du fichier
		 *\param[out]	toRead	Reçoit le caractère lu
		 *\return		Le nombre d'octets lus
		 */
		CU_API uint64_t readChar( xchar & toRead );
		/**
		 *\~english
		 *\brief		Copies all this file's content into a string
		 *\param[out]	out	Receives the content
		 *\return		The bytes read
		 *\~french
		 *\brief		Copie tout le contenu de ce fichier dans une chaîne de caractères
		 *\param[out]	out	Reçoit le contenu
		 *\return		Le nombre d'octets lus
		 */
		CU_API uint64_t copyToString( String & out );
		/**
		 *\~english
		 *\brief		Writes a text into the file
		 *\param[in]	text	The text
		 *\return		The bytes written
		 *\~french
		 *\brief		Ecrit une texte dans le fichier
		 *\param[in]	text	Le texte
		 *\return		Le nombre d'octets écrits
		 */
		CU_API uint64_t writeText( String const & text );
		/**
		 *\~english
		 *\brief		Writes a formatted text into the file
		 *\param[in]	maxSize	The maximum text size
		 *\param[in]	format	The text format
		 *\param[in]	...		POD arguments (cf. printf)
		 *\return		The bytes written
		 *\~french
		 *\brief		Ecrit un texte formaté dans le fichier
		 *\param[in]	maxSize	La taille maximale du texte
		 *\param[in]	format	Le format du texte
		 *\param[in]	...		Paramètres POD (cf. printf)
		 *\return		Le nombre d'octets écrits
		 */
		CU_API uint64_t print( uint64_t maxSize, xchar const * format, ... );
	};
	/**
	 *\~english
	 *\brief		open mode dependant write function
	 *\param[in]	file	The file
	 *\param[in]	toWrite	The data to write in the file
	 *\return		A reference to this file
	 *\~french
	 *\brief		Fonction d'écriture dépendant du mode d'ouverture
	 *\param[in]	file	Le fichier
	 *\param[in]	toWrite	La donnée à écrire dans le fichier
	 *\return		Une référence sur ce fichier
	 */
	template< typename T > TextFile & operator<<( TextFile & file, T const & toWrite );
	/**
	 *\~english
	 *\brief		open mode dependant read function
	 *\param[in]	file	The file
	 *\param[out]	toRead	The data to read from the file
	 *\return		A reference to this file
	 *\~french
	 *\brief		Fonction de lecture dépendant du mode d'ouverture
	 *\param[in]	file	Le fichier
	 *\param[out]	toRead	La donnée à lire à partir du fichier
	 *\return		Une référence sur ce fichier
	 */
	template< typename T > TextFile & operator>>( TextFile & file, T & toRead );
}

#include "TextFile.inl"

#endif
