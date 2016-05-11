﻿/*
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
#ifndef ___CASTOR_TEXT_FILE_H___
#define ___CASTOR_TEXT_FILE_H___

#include "File.hpp"

namespace Castor
{
	/*!
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
		 *\param[in]	p_fileName	The file path
		 *\param[in]	p_mode		The opening mode, combination of one or more eOPEN_MODE
		 *\param[in]	p_eEncoding	The text encoding mode
		 *\~french
		 *\brief		Ouvre le fichier situé au chemin donné, avec le mode et l'encodage donnés
		 *\param[in]	p_fileName	Le chemin du fichier
		 *\param[in]	p_mode		Le mode d'ouverture, combinaison d'un ou plusieurs eOPEN_MODE
		 *\param[in]	p_eEncoding	L'encodage pour un fichier en mode texte
		 */
		CU_API TextFile( Path const & p_fileName, int p_mode, eENCODING_MODE p_eEncoding = eENCODING_MODE_ASCII );
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
		CU_API uint64_t ReadLine( String & p_toRead, uint64_t p_size, String p_strSeparators = cuT( "\r\n" ) );
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
		CU_API uint64_t ReadWord( String & p_toRead );
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
		CU_API uint64_t ReadChar( xchar & p_toRead );
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
		CU_API uint64_t CopyToString( String & p_strOut );
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
		CU_API uint64_t WriteText( String const & p_strText );
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
		CU_API uint64_t Print( uint64_t p_uiMaxSize, xchar const * p_pFormat, ... );
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
	template< typename T > TextFile & operator<<( TextFile & p_file, T const & p_toWrite );
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
	template< typename T > TextFile & operator>>( TextFile & p_file, T & p_toRead );
}

#include "TextFile.inl"

#endif
