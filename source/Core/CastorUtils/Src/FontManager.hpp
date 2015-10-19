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
#ifndef ___CASTOR_FONT_MANAGER_H___
#define ___CASTOR_FONT_MANAGER_H___

#include "Collection.hpp"
#include "Loader.hpp"

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
	class FontManager
		: private Collection< Font, String >
	{
	public:
		/*!
		\author		Sylvain DOREMUS
		\version	0.8.0
		\date		29/09/2015
		\~english
		\brief		FontManager loader.
		\~french
		\brief		Loader de FontManager.
		*/
		class BinaryLoader
			: public Loader< FontManager, eFILE_TYPE_BINARY, BinaryFile >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor.
			 *\~french
			 *\brief		Constructeur.
			 */
			CU_API BinaryLoader();
			/**
			 *\~english
			 *\brief			Loads the font manager.
			 *\param[in,out]	p_manager	The manager to load.
			 *\param[in]		p_file		The source file.
			 *\~french
			 *\brief			Charge le gestionnaire de polices.
			 *\param[in,out]	p_font		Le gestionnaire à charger.
			 *\param[in]		p_file		Le fichier source.
			 */
			CU_API virtual bool operator()( FontManager & p_manager, BinaryFile & p_file );
		};

		DECLARE_MAP( Castor::String, Castor::Path, PathName );

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\~french
		 *\brief		Constructeur.
		 */
		CU_API FontManager();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		CU_API ~FontManager();
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
		CU_API FontSPtr create( Castor::Path const & p_path, Castor::String const & p_name, uint32_t p_height );
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
		CU_API FontSPtr get( Castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Clears the collection and file paths.
		 *\~french
		 *\brief		Nettoie la collection et les chemins d'accès aux fichiers.
		 */
		CU_API void clear();

	public:
		using Collection< Font, String >::begin;
		using Collection< Font, String >::end;
		using Collection< Font, String >::lock;
		using Collection< Font, String >::unlock;

	protected:
		//!\~english The font files paths sorted by <file_name>.<file_extension>	\~french Les fichiers des polices, triés par <file_name>.<file_extension>
		PathNameMap m_paths;
	};
}

#endif
 
