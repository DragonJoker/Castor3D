/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_BINARY_FILE___
#define ___CASTOR_BINARY_FILE___

#include "CastorUtils/Data/File.hpp"

namespace castor
{
	template<>
	struct FileTyper< FileType::eBinary >
	{
		using Type = BinaryFile;
	};
	/**
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 03/01/2011
	\~english
	\brief User friendly File class
	\remark adds some static functions to check file/directory validity, file/directory creation...
	\~french
	\brief Classe de gestion de fichier
	\remark Ajoute quelques fonctions statiques de vérification d'existence, suppression, ...
	*/
	class BinaryFile
		: public File
	{
	public:
		/**
		 *\~english
		 *\brief		Opens the file at the given path with the given mode and encoding
		 *\param[in]	fileName		The file path
		 *\param[in]	mode			The opening mode, combination of one or more OpenMode
		 *\param[in]	encodingMode	The encoding mode
		 *\~french
		 *\brief		Ouvre le fichier situé au chemin donné, avec le mode et l'encodage donnés
		 *\param[in]	fileName		Le chemin du fichier
		 *\param[in]	mode			Le mode d'ouverture, combinaison d'un ou plusieurs OpenMode
		 *\param[in]	encodingMode	Le mode d'encodage
		 */
		CU_API BinaryFile( Path const & fileName
			, FlagCombination< OpenMode > const & mode
			, EncodingMode encodingMode = EncodingMode::eASCII );
		/**
		 *\~english
		 *\brief		Destructor, closes the file
		 *\~french
		 *\brief		Destructeur, ferme le fichier
		 */
		CU_API ~BinaryFile()override;
		/**
		 *\~english
		 *\brief		Writes a string into the file (binary, so writes size then content, without '\0')
		 *\param[in]	toWrite	The string
		 *\return		\p true if written correctly
		 *\~french
		 *\brief		Ecrit une chaîne de caractères dans le fichier (en binaire, donc écrit d'abord la taille puis le contenu, sans le '\0')
		 *\param[in]	toWrite	La chaîne
		 *\return		\p true si l'écriture s'est bien déroulée
		 */
		CU_API bool write( String const & toWrite );
		/**
		 *\~english
		 *\brief		Reads a string from the file (binary, so reads size then content)
		 *\param[out]	toRead	The string
		 *\return		\p true if read correctly
		 *\~french
		 *\brief		Lit une chaîne de caractères à partir du fichier (en binaire, donc lit d'abord la taille puis le contenu)
		 *\param[out]	toRead	La chaîne
		 *\return		\p true si la lecture s'est bien déroulée
		 */
		CU_API bool read( String & toRead );
		/**
		 *\~english
		 *\brief		Writes an object into the file
		 *\param[in]	toWrite	The object
		 *\return		The bytes written
		 *\~french
		 *\brief		Ecrit un objet dans le fichier
		 *\param[in]	toWrite	L'objet
		 *\return		Le nombre d'octets écrits
		 */
		template< typename T > uint64_t write( T const & toWrite );
		/**
		 *\~english
		 *\brief		Reads an object from the file
		 *\param[out]	toRead	The object
		 *\return		The bytes read
		 *\~french
		 *\brief		Lit un objet à partir du fichier
		 *\param[out]	toRead	L'objet
		 *\return		Le nombre d'octets lus
		 */
		template< typename T > uint64_t read( T & toRead );
		/**
		 *\~english
		 *\brief		Writes a dynamic array of objects into the file
		 *\param[in]	toWrite	The object array
		 *\param[in]	count	The array size
		 *\return		The bytes written
		 *\~french
		 *\brief		Ecrit un tableau dynamique d'objets dans du fichier
		 *\param[in]	toWrite	Le tableau d'objets
		 *\param[in]	count	La taille du tableau
		 *\return		Le nombre d'octets écrits
		 */
		template< typename T > uint64_t writeArray( T const * toWrite, uint64_t count );
		/**
		 *\~english
		 *\brief		Writes a static array of objects into the file
		 *\remarks		Array size is deduced from parameter
		 *\param[in]	toWrite	The object array
		 *\return		The bytes written
		 *\~french
		 *\brief		Ecrit un tableau statique d'objets dans du fichier
		 *\remarks		La taille du tableau est déduite à partir du paramètre
		 *\param[in]	toWrite	Le tableau d'objets
		 *\return		Le nombre d'octets écrits
		 */
		template< typename T, uint64_t N > uint64_t writeArray( T const( & toWrite )[N] );
		/**
		 *\~english
		 *\brief		Reads a dynamic array of objects from the file
		 *\param[out]	toRead	The objects array (must be allocated before call)
		 *\param[in]	count	The array size
		 *\return		The bytes read
		 *\~french
		 *\brief		Lit un tableau dynamique d'objets à partir du fichier
		 *\param[out]	toRead	Le tableau d'objets (doit être alloué avant l'appel)
		 *\param[in]	count	La taille du tableau
		 *\return		Le nombre d'octets lus
		 */
		template< typename T > uint64_t readArray( T * toRead, uint64_t count );
		/**
		 *\~english
		 *\brief		Reads a static array of objects from the file
		 *\param[out]	toRead	The objects array
		 *\remarks		Array size is deduced from parameter
		 *\return		The bytes read
		 *\~french
		 *\brief		Lit un tableau statique d'objets à partir du fichier
		 *\param[out]	toRead	Le tableau d'objets
		 *\remarks		La taille du tableau est déduite à partir du paramètre
		 *\return		Le nombre d'octets lus
		 */
		template< typename T, uint64_t N > uint64_t readArray( T( & toRead )[N] );
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
	template< typename T > BinaryFile & operator<<( BinaryFile & file, T const & toWrite );
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
	template< typename T > BinaryFile & operator>>( BinaryFile & file, T & toRead );
}

#include "BinaryFile.inl"

#endif
