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
#ifndef ___CASTOR_FONT_CACHE_H___
#define ___CASTOR_FONT_CACHE_H___

#include "Design/Collection.hpp"

#if defined( CreateFont )
#	undef CreateFont
#endif

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		29/09/2015
	\~english
	\brief		Font manager.
	\remark		Holds the loaded fonts, and also the paths to font files.
	\~french
	\brief		Gestionnaire de polices.
	\remark		Détient les polices, et aussi les chemins d'accès aux fichiers des polices.
	*/
	class FontCache
		: private Collection< Font, String >
	{
	public:
		DECLARE_MAP( Castor::String, Castor::Path, PathName );

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\~french
		 *\brief		Constructeur.
		 */
		CU_API FontCache();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		CU_API ~FontCache();
		/**
		 *\~english
		 *\brief		Creates a font.
		 *\remarks		If the font already exists, it is returned.
		 *\param[in]	p_path		The full access path to the file.
		 *\param[in]	p_name		The font name.
		 *\param[in]	p_height	The font precision.
		 *\return		The created (or retrieved) font.
		 *\~french
		 *\brief		Crée une police.
		 *\remarks		Si la police existe déjà, elle est retournée.
		 *\param[in]	p_path		Le chemin complet d'accès au fichier.
		 *\param[in]	p_name		Le nom de la police.
		 *\param[in]	p_height	La précision de la police.
		 *\return		La police créée (ou récupérée).
		 */
		CU_API FontSPtr Add( Castor::String const & p_name, uint32_t p_height, Castor::Path const & p_path );
		/**
		 *\~english
		 *\brief		Retrieves a font.
		 *\param[in]	p_name	The font name.
		 *\return		The font, nullptr if not found.
		 *\~french
		 *\brief		Récupère une police.
		 *\param[in]	p_name	Le nom de la police.
		 *\return		La police, nullptr si non trouvée.
		 */
		CU_API FontSPtr Find( Castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Retrieves a font.
		 *\param[in]	p_name	The font name.
		 *\return		The font, nullptr if not found.
		 *\~french
		 *\brief		Récupère une police.
		 *\param[in]	p_name	Le nom de la police.
		 *\return		La police, nullptr si non trouvée.
		 */
		CU_API void Remove( Castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Clears the collection and file paths.
		 *\~french
		 *\brief		Nettoie la collection et les chemins d'accès aux fichiers.
		 */
		CU_API void Clear();

	public:
		using Collection< Font, String >::begin;
		using Collection< Font, String >::end;
		using Collection< Font, String >::lock;
		using Collection< Font, String >::unlock;

	protected:
		//!\~english The font files paths sorted by file_name.file_extension	\~french Les fichiers des polices, triés par file_name.file_extension
		PathNameMap m_paths;
	};
}

#endif
