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
#ifndef ___CASTOR_BINARY_FILE___
#define ___CASTOR_BINARY_FILE___

#include "File.hpp"

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		24/05/2016
	\~english
	\brief		Helper structure to find File type (BinaryFile or TextFile) from eFILE_TYPE.
	\remarks	Specialisation for eFILE_TYPE_BINARY.
	\~french
	\brief		Structure d'aide pour trouver le type de File (BinaryFile ou TextFile) à partir d'un eFILE_TYPE.
	\remarks	Spécialisation pour eFILE_TYPE_BINARY.
	*/
	template<>
	struct FileTyper< eFILE_TYPE_BINARY >
	{
		using FileType = BinaryFile;
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
	class BinaryFile
		: public File
	{
	public:
		/**
		 *\~english
		 *\brief		Opens the file at the given path with the given mode and encoding
		 *\param[in]	p_fileName		The file path
		 *\param[in]	p_mode			The opening mode, combination of one or more eOPEN_MODE
		 *\param[in]	p_encodingMode	The encoding mode
		 *\~french
		 *\brief		Ouvre le fichier situé au chemin donné, avec le mode et l'encodage donnés
		 *\param[in]	p_fileName		Le chemin du fichier
		 *\param[in]	p_mode			Le mode d'ouverture, combinaison d'un ou plusieurs eOPEN_MODE
		 *\param[in]	p_encodingMode	Le mode d'encodage
		 */
		CU_API BinaryFile( Path const & p_fileName, int p_mode, eENCODING_MODE p_encodingMode = eENCODING_MODE_ASCII );
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
		CU_API bool Write( String const & p_toWrite );
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
		CU_API bool Read( String & p_toRead );
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
		 *\remarks		Array size is deduced from parameter
		 *\param[in]	p_toWrite	The object array
		 *\return		The bytes written
		 *\~french
		 *\brief		Ecrit un tableau statique d'objets dans du fichier
		 *\remarks		La taille du tableau est déduite à partir du paramètre
		 *\param[in]	p_toWrite	Le tableau d'objets
		 *\return		Le nombre d'octets écrits
		 */
		template< typename T, uint64_t N > uint64_t WriteArray( T const( & p_toWrite )[N] );
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
		 *\remarks		Array size is deduced from parameter
		 *\return		The bytes read
		 *\~french
		 *\brief		Lit un tableau statique d'objets à partir du fichier
		 *\param[out]	p_toRead	Le tableau d'objets
		 *\remarks		La taille du tableau est déduite à partir du paramètre
		 *\return		Le nombre d'octets lus
		 */
		template< typename T, uint64_t N > uint64_t ReadArray( T( & p_toRead )[N] );
	};
	/**
	 *\~english
	 *\brief		Open mode dependant write function
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
	 *\brief		Open mode dependant read function
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
