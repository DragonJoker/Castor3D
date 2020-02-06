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
#ifndef ___CASTOR_PATH_H___
#define ___CASTOR_PATH_H___

#include "Miscellaneous/StringUtils.hpp"

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\date		09/02/2010
	\~english
	\brief		Path management class
	\remark		Defines platform dependant paths.
	\~french
	\brief		Classe de gestion de chemin
	\remark		Définit un chemin de fichier.
	*/
	class Path
		: public String
	{
	public:
		//!\~english Path separator (OS dependant)	\~french Séparateur de chemin (dépendant de l'OS)
		static const xchar Separator;

	public:
		CU_API Path();
		/**
		 *\~english
		 *\brief		Constructor from a char array
		 *\param[in]	p_data	The char array
		 *\~french
		 *\brief		Constructeur à partir d'un tableau de char
		 *\param[in]	p_data	Le tableau de char
		 */
		explicit CU_API Path( char const * p_data );
		/**
		 *\~english
		 *\brief		Constructor from a wchar_t array
		 *\param[in]	p_data	The wchar_t array
		 *\~french
		 *\brief		Constructeur à partir d'un tableau de wchar_t
		 *\param[in]	p_data	Le tableau de wchar_t
		 */
		explicit CU_API Path( wchar_t const * p_data );
		/**
		 *\~english
		 *\brief		Constructor from a String
		 *\param[in]	p_data	The String
		 *\~french
		 *\brief		Constructeur à partir d'un String
		 *\param[in]	p_data	Le String
		 */
		explicit CU_API Path( String const & p_data );
		/**
		 *\~english
		 *\brief		Copy Constructor
		 *\param[in]	p_data	The Path object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_data	L'objet Path à copier
		 */
		CU_API Path( Path const & p_data );
		/**
		 *\~english
		 *\brief		Move Constructor
		 *\param[in]	p_data	The Path object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_data	L'objet Path à déplacer
		 */
		CU_API Path( Path && p_data );
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_data	The Path object to copy
		 *\return		A reference to this Path object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_data	L'objet Path à copier
		 *\return		Une référence sur cet objet Path
		 */
		CU_API Path & operator =( Path const & p_data );
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_data	The Path object to move
		 *\return		A reference to this Path object
		 *\~french
		 *\brief		Opérateur d'affectation  par déplacement
		 *\param[in]	p_data	L'objet Path à déplacer
		 *\return		Une référence sur cet objet Path
		 */
		CU_API Path & operator=( Path && p_data );
		/**
		 *\~english
		 *\brief		Appends a path to the current path. Adds the separator if needed
		 *\param[in]	p_path	The path to add
		 *\~french
		 *\brief		Concatène un chemin à celui-ci. Ajoute le séparateur si besoin est
		 *\param[in]	p_path	Le chemin à concaténer
		 */
		CU_API Path & operator/=( Path const & p_path );
		/**
		 *\~english
		 *\brief		Adds a string to the current path. Adds the separator if needed
		 *\param[in]	p_string	The path to add
		 *\~french
		 *\brief		Concatène une chaîne de caractères à ce chemin. Ajoute le séparateur si besoin est
		 *\param[in]	p_string	La chaîne de caractères à concaténer
		 */
		CU_API Path & operator/=( String const & p_string );
		/**
		 *\~english
		 *\brief		Adds a MBCS C string to the current path. Adds the separator if needed
		 *\param[in]	p_buffer	The path to add
		 *\~french
		 *\brief		Concatène une chaîne de caractères C MBCS à ce chemin. Ajoute le séparateur si besoin est
		 *\param[in]	p_buffer	La chaîne de caractères C à concaténer
		 */
		CU_API Path & operator/=( char const * p_buffer );
		/**
		 *\~english
		 *\brief		Adds a Unicode C string to the current path. Adds the separator if needed
		 *\param[in]	p_buffer	The path to add
		 *\~french
		 *\brief		Concatène une chaîne de caractères C Unicode à ce chemin. Ajoute le séparateur si besoin est
		 *\param[in]	p_buffer	La chaîne de caractères C à concaténer
		 */
		CU_API Path & operator/=( wchar_t const * p_buffer );
		/**
		 *\~english
		 *\brief		Appends a path to the current path.
		 *\param[in]	p_path	The path to add
		 *\~french
		 *\brief		Concatène un chemin à celui-ci.
		 *\param[in]	p_path	Le chemin à concaténer
		 */
		CU_API Path & operator+=( Path const & p_path );
		/**
		 *\~english
		 *\brief		Adds a string to the current path.
		 *\param[in]	p_string	The path to add
		 *\~french
		 *\brief		Concatène une chaîne de caractères à ce chemin.
		 *\param[in]	p_string	La chaîne de caractères à concaténer
		 */
		CU_API Path & operator+=( String const & p_string );
		/**
		 *\~english
		 *\brief		Adds a MBCS C string to the current path.
		 *\param[in]	p_buffer	The path to add
		 *\~french
		 *\brief		Concatène une chaîne de caractères C MBCS à ce chemin.
		 *\param[in]	p_buffer	La chaîne de caractères C à concaténer
		 */
		CU_API Path & operator+=( char const * p_buffer );
		/**
		 *\~english
		 *\brief		Adds a Unicode C string to the current path.
		 *\param[in]	p_buffer	The path to add
		 *\~french
		 *\brief		Concatène une chaîne de caractères C Unicode à ce chemin.
		 *\param[in]	p_buffer	La chaîne de caractères C à concaténer
		 */
		CU_API Path & operator+=( wchar_t const * p_buffer );
		/**
		 *\~english
		 *\brief		Retrieves the parent path
		 *\return		The parent path
		 *\~french
		 *\brief		Récupère le dossier parent
		 *\return		Le dossier parent
		 */
		CU_API Path GetPath()const;
		/**
		 *\~english
		 *\brief		Retrieves the leaf of this path (filename and extension for files).
		 *\param[in]	p_withExtension	Tells if we want extension or not, for files.
		 *\return		The leaf.
		 *\~french
		 *\brief		Récupère la feuille de ce chemin (nom du fichier et extension, pour les fichiers).
		 *\param[in]	p_withExtension	Dit si on veur l'extension ou pas, pour les fichiers.
		 *\return		La feuille.
		 */
		CU_API Path GetFileName( bool p_withExtension = false )const;
		/**
		 *\~english
		 *\brief		Retrieves the full path and leaf
		 *\return		The full path and leaf
		 *\~french
		 *\brief		Récupère le chemin complet
		 *\return		Le chemin complet
		 */
		CU_API Path GetFullFileName()const;
		/**
		 *\~english
		 *\brief		Retrieves the extension of the file name, if any
		 *\return		The extension
		 *\~french
		 *\brief		Récupère l'extension du nom de fichier, s'il y en a
		 *\return		L'extension
		 */
		CU_API String GetExtension()const;

	private:
		void DoNormalise();
	};
	/**
	 *\~english
	 *\brief		Appends 2 paths. Adds the separator if needed
	 *\param[in]	p_lhs	The first path to add
	 *\param[in]	p_rhs	The second path to add
	 *\~french
	 *\brief		Concatène 2 chemins. Ajoute le séparateur si besoin est
	 *\param[in]	p_lhs	Le premier chemin à concaténer
	 *\param[in]	p_rhs	Le second chemin à concaténer
	 */
	CU_API Path operator/( Path const & p_lhs, Path const & p_rhs );
	/**
	 *\~english
	 *\brief		Adds a string to a path. Adds the separator if needed
	 *\param[in]	p_lhs	The first path to add
	 *\param[in]	p_rhs	The path to add
	 *\~french
	 *\brief		Concatène une chaîne de caractères à un chemin. Ajoute le séparateur si besoin est
	 *\param[in]	p_lhs	The first path to add
	 *\param[in]	p_rhs	La chaîne de caractères à concaténer
	 */
	CU_API Path operator/( Path const & p_lhs, String const & p_rhs );
	/**
	 *\~english
	 *\brief		Adds a string to a path. Adds the separator if needed
	 *\param[in]	p_lhs	The first path to add
	 *\param[in]	p_rhs	The path to add
	 *\~french
	 *\brief		Concatène une chaîne de caractères à un chemin. Ajoute le séparateur si besoin est
	 *\param[in]	p_lhs	The first path to add
	 *\param[in]	p_rhs	La chaîne de caractères à concaténer
	 */
	CU_API Path operator/( Path const & p_lhs, char const * p_rhs );
	/**
	 *\~english
	 *\brief		Adds a string to a path. Adds the separator if needed
	 *\param[in]	p_lhs	The first path to add
	 *\param[in]	p_rhs	The path to add
	 *\~french
	 *\brief		Concatène une chaîne de caractères à un chemin. Ajoute le séparateur si besoin est
	 *\param[in]	p_lhs	The first path to add
	 *\param[in]	p_rhs	La chaîne de caractères à concaténer
	 */
	CU_API Path operator/( Path const & p_lhs, wchar_t const * p_rhs );
	/**
	 *\~english
	 *\brief		Adds a string to a path. Adds the separator if needed
	 *\param[in]	p_lhs	The path to add
	 *\param[in]	p_rhs	The first path to add
	 *\~french
	 *\brief		Concatène une chaîne de caractères à un chemin. Ajoute le séparateur si besoin est
	 *\param[in]	p_lhs	La chaîne de caractères à concaténer
	 *\param[in]	p_rhs	The first path to add
	 */
	CU_API Path operator/( String const & p_lhs, Path const & p_rhs );
	/**
	 *\~english
	 *\brief		Adds a string to a path. Adds the separator if needed
	 *\param[in]	p_lhs	The path to add
	 *\param[in]	p_rhs	The first path to add
	 *\~french
	 *\brief		Concatène une chaîne de caractères à un chemin. Ajoute le séparateur si besoin est
	 *\param[in]	p_lhs	La chaîne de caractères à concaténer
	 *\param[in]	p_rhs	The first path to add
	 */
	CU_API Path operator/( char const * p_lhs, Path const & p_rhs );
	/**
	 *\~english
	 *\brief		Adds a string to a path. Adds the separator if needed
	 *\param[in]	p_lhs	The path to add
	 *\param[in]	p_rhs	The first path to add
	 *\~french
	 *\brief		Concatène une chaîne de caractères à un chemin. Ajoute le séparateur si besoin est
	 *\param[in]	p_lhs	La chaîne de caractères à concaténer
	 *\param[in]	p_rhs	The first path to add
	 */
	CU_API Path operator/( wchar_t const * p_lhs, Path const & p_rhs );
}

#endif
