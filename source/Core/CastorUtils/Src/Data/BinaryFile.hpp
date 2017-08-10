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
#ifndef ___CASTOR_BINARY_FILE___
#define ___CASTOR_BINARY_FILE___

#include "File.hpp"

namespace castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		24/05/2016
	\~english
	\brief		Helper structure to find File type (BinaryFile or TextFile) from FileType.
	\remarks	Specialisation for FileType::eBinary.
	\~french
	\brief		Structure d'aide pour trouver le type de File (BinaryFile ou TextFile) à partir d'un FileType.
	\remarks	Spécialisation pour FileType::eBinary.
	*/
	template<>
	struct FileTyper< FileType::eBinary >
	{
		using Type = BinaryFile;
	};
	/*!
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
		 *\param[in]	p_fileName		The file path
		 *\param[in]	p_mode			The opening mode, combination of one or more OpenMode
		 *\param[in]	p_encodingMode	The encoding mode
		 *\~french
		 *\brief		Ouvre le fichier situé au chemin donné, avec le mode et l'encodage donnés
		 *\param[in]	p_fileName		Le chemin du fichier
		 *\param[in]	p_mode			Le mode d'ouverture, combinaison d'un ou plusieurs OpenMode
		 *\param[in]	p_encodingMode	Le mode d'encodage
		 */
		CU_API BinaryFile( Path const & p_fileName
			, FlagCombination< OpenMode > const & p_mode
			, EncodingMode p_encodingMode = EncodingMode::eASCII );
		/**
		 *\~english
		 *\brief		Destructor, closes the file
		 *\~french
		 *\brief		Destructeur, ferme le fichier
		 */
		CU_API virtual ~BinaryFile();
		/**
		 *\~english
		 *\brief		Writes a string into the file (binary, so writes size then content, without '\0')
		 *\param[in]	p_toWrite	The string
		 *\return		\p true if written correctly
		 *\~french
		 *\brief		Ecrit une chaîne de caractères dans le fichier (en binaire, donc écrit d'abord la taille puis le contenu, sans le '\0')
		 *\param[in]	p_toWrite	La chaîne
		 *\return		\p true si l'écriture s'est bien déroulée
		 */
		CU_API bool write( String const & p_toWrite );
		/**
		 *\~english
		 *\brief		Reads a string from the file (binary, so reads size then content)
		 *\param[out]	p_toRead	The string
		 *\return		\p true if read correctly
		 *\~french
		 *\brief		Lit une chaîne de caractères à partir du fichier (en binaire, donc lit d'abord la taille puis le contenu)
		 *\param[out]	p_toRead	La chaîne
		 *\return		\p true si la lecture s'est bien déroulée
		 */
		CU_API bool read( String & p_toRead );
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
		template< typename T > uint64_t write( T const & p_toWrite );
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
		template< typename T > uint64_t read( T & p_toRead );
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
		template< typename T > uint64_t writeArray( T const * p_toWrite, uint64_t p_count );
		/**
		 *\~english
		 *\brief		Writes a static array of objects into the file
		 *\remarks		Array size is deduced from parameter
		 *\param[in]	p_toWrite	The object array
		 *\return		The bytes written
		 *\~french
		 *\brief		Ecrit un tableau statique d'objets dans du fichier
		 *\remarks		La taille du tableau est déduite à partir du paramètre
		 *\param[in]	p_toWrite	Le tableau d'objets
		 *\return		Le nombre d'octets écrits
		 */
		template< typename T, uint64_t N > uint64_t writeArray( T const( & p_toWrite )[N] );
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
		template< typename T > uint64_t readArray( T * p_toRead, uint64_t p_count );
		/**
		 *\~english
		 *\brief		Reads a static array of objects from the file
		 *\param[out]	p_toRead	The objects array
		 *\remarks		Array size is deduced from parameter
		 *\return		The bytes read
		 *\~french
		 *\brief		Lit un tableau statique d'objets à partir du fichier
		 *\param[out]	p_toRead	Le tableau d'objets
		 *\remarks		La taille du tableau est déduite à partir du paramètre
		 *\return		Le nombre d'octets lus
		 */
		template< typename T, uint64_t N > uint64_t readArray( T( & p_toRead )[N] );
	};
	/**
	 *\~english
	 *\brief		open mode dependant write function
	 *\param[in]	p_file		The file
	 *\param[in]	p_toWrite	The data to write in the file
	 *\return		A reference to this file
	 *\~french
	 *\brief		Fonction d'écriture dépendant du mode d'ouverture
	 *\param[in]	p_file		Le fichier
	 *\param[in]	p_toWrite	La donnée à écrire dans le fichier
	 *\return		Une référence sur ce fichier
	 */
	template< typename T > BinaryFile & operator<<( BinaryFile & p_file, T const & p_toWrite );
	/**
	 *\~english
	 *\brief		open mode dependant read function
	 *\param[in]	p_file		The file
	 *\param[out]	p_toRead	The data to read from the file
	 *\return		A reference to this file
	 *\~french
	 *\brief		Fonction de lecture dépendant du mode d'ouverture
	 *\param[in]	p_file		Le fichier
	 *\param[out]	p_toRead	La donnée à lire à partir du fichier
	 *\return		Une référence sur ce fichier
	 */
	template< typename T > BinaryFile & operator>>( BinaryFile & p_file, T & p_toRead );
}

#include "BinaryFile.inl"

#endif
