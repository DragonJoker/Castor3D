/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_FONT_CACHE_H___
#define ___CASTOR_FONT_CACHE_H___

#include "Design/Collection.hpp"

#if defined( CreateFont )
#	undef CreateFont
#endif

namespace castor
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
		DECLARE_MAP( castor::String, castor::Path, PathName );

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
		CU_API FontSPtr create( castor::String const & p_name, uint32_t p_height, castor::Path const & p_path );
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
		CU_API FontSPtr add( castor::String const & p_name, uint32_t p_height, castor::Path const & p_path );
		/**
		 *\~english
		 *\brief		adds an already created font.
		 *\param[in]	p_name	The font name.
		 *\param[in]	p_font	The font.
		 *\return		The font.
		 *\~french
		 *\brief		Ajoute une police déjà créée.
		 *\remarks		Si la police existe déjà, elle est retournée.
		 *\param[in]	p_name	Le nom de la police.
		 *\param[in]	p_font	La police.
		 *\return		La police.
		 */
		CU_API FontSPtr add( castor::String const & p_name, FontSPtr p_font );
		/**
		 *\~english
		 *\brief		Tells if a font exists.
		 *\param[in]	p_name	The font name.
		 *\return		\p false if not found.
		 *\~french
		 *\brief		Dit si une police existe.
		 *\param[in]	p_name	Le nom de la police.
		 *\return		\p false si non trouvée.
		 */
		CU_API bool has( castor::String const & p_name );
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
		CU_API FontSPtr find( castor::String const & p_name );
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
		CU_API void remove( castor::String const & p_name );
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
		//!\~english The font files paths sorted by file_name.file_extension	\~french Les fichiers des polices, triés par file_name.file_extension
		PathNameMap m_paths;
	};
}

#endif
