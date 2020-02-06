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
#ifndef ___CASTOR_BINARY_FILE___
#define ___CASTOR_BINARY_FILE___

#include "File.hpp"

namespace Castor
{
	/*!
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 03/01/2011
	\~english
	\brief User friendly File class
	\remark Adds some static functions to check file/directory validity, file/directory creation...
	\~french
	\brief Classe de gestion de fichier
	\remark Ajoute quelques fonctions statiques de v�rification d'existence, suppression, ...
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
		 *\brief		Ouvre le fichier situ� au chemin donn�, avec le mode et l'encodage donn�s
		 *\param[in]	p_fileName	Le chemin du fichier
		 *\param[in]	p_iMode		Le mode d'ouverture, combinaison d'un ou plusieurs eOPEN_MODE
		 */
		BinaryFile( Path const & p_fileName, int p_iMode, eENCODING_MODE p_eEncodingMode = eENCODING_MODE_ASCII );
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
		 *\brief		Constructeur par d�placement
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
		 *\brief		Fonction d'�criture d�pendant du mode d'ouverture
		 *\param[in]	p_toWrite	La donn�e � �crire dans le fichier
		 *\return		Une r�f�rence sur ce fichier
		 */
		template< typename T > BinaryFile & operator <<( T const & p_toWrite );
		/**
		 *\~english
		 *\brief		Open mode dependant read function
		 *\param[out]	p_toRead	The data to read from this file
		 *\return		A reference to this file
		 *\~french
		 *\brief		Fonction de lecture d�pendant du mode d'ouverture
		 *\param[out]	p_toRead	La donn�e � lire � partir du fichier
		 *\return		Une r�f�rence sur ce fichier
		 */
		template< typename T > BinaryFile & operator >>( T & p_toRead );
		/**
		 *\~english
		 *\brief		Writes a string into the file (binary, so writes size then content, without '\0')
		 *\param[in]	p_strToWrite	The string
		 *\return		\p true if written correctly
		 *\~french
		 *\brief		Ecrit une cha�ne de caract�res dans le fichier (en binaire, donc �crit d'abord la taille puis le contenu, sans le '\0')
		 *\param[in]	p_strToWrite	La cha�ne
		 *\return		\p true si l'�criture s'est bien d�roul�e
		 */
		bool Write( String const & p_strToWrite );
		/**
		 *\~english
		 *\brief		Reads a string from the file (binary, so reads size then content)
		 *\param[out]	p_strToRead	The string
		 *\return		\p true if read correctly
		 *\~french
		 *\brief		Lit une cha�ne de caract�res � partir du fichier (en binaire, donc lit d'abord la taille puis le contenu)
		 *\param[out]	p_strToRead	La cha�ne
		 *\return		\p true si la lecture s'est bien d�roul�e
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
		 *\return		Le nombre d'octets �crits
		 */
		template< typename T > uint64_t Write( T const & p_toWrite );
		/**
		 *\~english
		 *\brief		Reads an object from the file
		 *\param[out]	p_toRead	The object
		 *\return		The bytes read
		 *\~french
		 *\brief		Lit un objet � partir du fichier
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
		 *\return		Le nombre d'octets �crits
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
		 *\remark		La taille du tableau est d�duite � partir du param�tre
		 *\param[in]	p_toWrite	Le tableau d'objets
		 *\return		Le nombre d'octets �crits
		 */
		template< typename T, uint64_t N > uint64_t WriteArray( T const( & p_toWrite )[N] );
		/**
		 *\~english
		 *\brief		Reads a dynamic array of objects from the file
		 *\param[out]	p_toRead	The objects array (must be allocated before call)
		 *\param[in]	p_count		The array size
		 *\return		The bytes read
		 *\~french
		 *\brief		Lit un tableau dynamique d'objets � partir du fichier
		 *\param[out]	p_toRead	Le tableau d'objets (doit �tre allou� avant l'appel)
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
		 *\brief		Lit un tableau statique d'objets � partir du fichier
		 *\param[out]	p_toRead	Le tableau d'objets
		 *\remark		La taille du tableau est d�duite � partir du param�tre
		 *\return		Le nombre d'octets lus
		 */
		template< typename T, uint64_t N > uint64_t ReadArray( T( & p_toRead )[N] );
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_file	The file to copy
		 *\return		A reference to this file
		 *\~french
		 *\brief		Op�rateur d'affectation par copie
		 *\param[in]	p_file	Le fichier � copier
		 *\return		Une r�f�rence sur ce fichier
		 */
		BinaryFile & operator =( BinaryFile const & p_file );
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_file	The file to move
		 *\return		A reference to this file
		 *\~french
		 *\brief		Op�rateur d'affectation par d�placement
		 *\param[in]	p_file	Le fichier � d�placer
		 *\return		Une r�f�rence sur ce fichier
		 */
		BinaryFile & operator =( BinaryFile && p_file );
	};
}

#include "BinaryFile.inl"

#endif
