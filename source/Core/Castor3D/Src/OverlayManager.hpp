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
#ifndef ___C3D_OVERLAY_MANAGER_H___
#define ___C3D_OVERLAY_MANAGER_H___

#include "Overlay.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		09/02/2010
	\version	0.1
	\~english
	\brief		Overlay collection, with additional add and remove functions to manage Z-Index
	\~french
	\brief		Collection d'incrustations, avec des fonctions additionnelles d'ajout et de suppression pour gérer les Z-Index
	*/
	class C3D_API OverlayManager
		: private OverlayCollection
	{
	public:
		typedef OverlayCollectionIt iterator;
		typedef OverlayCollectionConstIt const_iterator;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		OverlayManager( Engine * p_pEngine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~OverlayManager();
		/**
		 *\~english
		 *\brief		Clears all overlays lists
		 *\~french
		 *\brief		Vide les listes d'incrustations
		 */
		void ClearOverlays();
		/**
		 *\~english
		 *\brief		Add an overlay to the lists, given it's name
		 *\param[in]	p_strName	The overlay name
		 *\param[in]	p_pOverlay	The overlay
		 *\param[in]	p_pParent	The parent overlay
		 *\~french
		 *\brief		Ajoute une incrustation aux listes, selon son nom
		 *\param[in]	p_strName	Le nom
		 *\param[in]	p_pOverlay	L'incrustation
		 *\param[in]	p_pParent	L'incrustation parente
		 */
		void AddOverlay( Castor::String const & p_strName, OverlaySPtr p_pOverlay, OverlaySPtr p_pParent );
		/**
		 *\~english
		 *\brief		Retrieves the overlay with the given name
		 *\param[in]	p_strName	The name
		 *\return		The overlay, \p nullptr if not found
		 *\~french
		 *\brief		Récupère l'incrustation avec le nom donné
		 *\param[in]	p_strName	Le nom
		 *\return		L'incrustation, \p nullptr si non trouvée
		 */
		OverlaySPtr GetOverlay( Castor::String const & p_strName );
		/**
		 *\~english
		 *\brief		Checks if an overlay with the given name exists
		 *\param[in]	p_strName	The name
		 *\return		\p true if an overlay is defined with given name
		 *\~french
		 *\brief		Vérifie si une incrustation avec le nom donné existe
		 *\param[in]	p_strName	Le nom
		 *\return		\p true Si une incrustation est défini avec le nom donné
		 */
		bool HasOverlay( Castor::String const & p_strName );
		/**
		 *\~english
		 *\brief		Writes overlays in a text file
		 *\param[out]	p_file	The file
		 *\return		\p true if ok
		 *\~french
		 *\brief		Ecrit les overlays dans un fichier texte
		 *\param[out]	p_file	Le fichier
		 *\return		\p true si tout s'est bien passé
		 */
		bool WriteOverlays( Castor::TextFile & p_file )const;
		/**
		 *\~english
		 *\brief		Reads overlays from a text file
		 *\param[in]	p_file	The file
		 *\return		\p true if ok
		 *\~french
		 *\brief		Lit les overlays à partir d'un fichier texte
		 *\param[in]	p_file	Le fichier
		 *\return		\p true si tout s'est bien passé
		 */
		bool ReadOverlays( Castor::TextFile & p_file );
		/**
		 *\~english
		 *\brief		Writes overlays in a binary file
		 *\param[out]	p_file	The file
		 *\return		\p true if ok
		 *\~french
		 *\brief		Ecrit les overlays dans un fichier binaire
		 *\param[out]	p_file	Le fichier
		 *\return		\p true si tout s'est bien passé
		 */
		bool SaveOverlays( Castor::BinaryFile & p_file )const;
		/**
		 *\~english
		 *\brief		Reads overlays from a binary file
		 *\param[in]	p_file	The file
		 *\return		\p true if ok
		 *\~french
		 *\brief		Lit les overlays à partir d'un fichier binaire
		 *\param[in]	p_file	Le fichier
		 *\return		\p true si tout s'est bien passé
		 */
		bool LoadOverlays( Castor::BinaryFile & p_file );
		/**
		 *\~english
		 *\brief		Initialises or cleans up the OverlayRenderer, according to engine rendering status
		 *\~french
		 *\brief		Initialise ou nettoie l'OverlayRenderer, selon le statut du rendu
		 */
		void Update();
		/**
		 *\~english
		 *\brief		Retrieves the overlay renderer
		 *\return		The overlay renderer
		 *\~french
		 *\brief		Récupère le renderer d'incrustation
		 *\return		Le renderer d'incrustation
		 */
		OverlayRendererSPtr GetRenderer()const
		{
			return m_pRenderer;
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the first overlay
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur la première incrustation
		 *\return		La valeur
		 */
		inline OverlayPtrArray::iterator begin()
		{
			return m_overlays.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the first overlay
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur la première incrustation
		 *\return		La valeur
		 */
		inline OverlayPtrArray::const_iterator begin()const
		{
			return m_overlays.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the first overlay
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur la première incrustation
		 *\return		La valeur
		 */
		inline OverlayPtrArray::reverse_iterator rbegin()
		{
			return m_overlays.rbegin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the first overlay
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur la première incrustation
		 *\return		La valeur
		 */
		inline OverlayPtrArray::const_reverse_iterator rbegin()const
		{
			return m_overlays.rbegin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to after the last overlay
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur après la dernière incrustation
		 *\return		La valeur
		 */
		inline OverlayPtrArray::iterator end()
		{
			return m_overlays.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to after the last overlay
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur après la dernière incrustation
		 *\return		La valeur
		 */
		inline OverlayPtrArray::const_iterator end()const
		{
			return m_overlays.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to after the last overlay
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur après la dernière incrustation
		 *\return		La valeur
		 */
		inline OverlayPtrArray::reverse_iterator rend()
		{
			return m_overlays.rend();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to after the last overlay
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur après la dernière incrustation
		 *\return		La valeur
		 */
		inline OverlayPtrArray::const_reverse_iterator rend()const
		{
			return m_overlays.rend();
		}
		/**
		 *\~english
		 *\brief		Renders the overlays
		 *\param[in]	p_scene	The currently rendered scene
		 *\param[in]	p_size	The render window size
		 *\~french
		 *\brief		Dessine les incrustations
		 *\param[in]	p_scene	La scène en cours de rendu
		 *\param[in]	p_size	La taille de la fenêtre de rendu
		 */
		void RenderOverlays( Scene const & p_scene, Castor::Size const & p_size );

		using OverlayCollection::lock;
		using OverlayCollection::unlock;

	private:
		//!\~english The overlays, in rendering order	\~french Les incrustations, dans l'ordre de rendu
		OverlayPtrArray m_overlays;
		//!\~english The engine	\~french Le moteur
		Engine * m_pEngine;
		//!\~english The overlay renderer	\~french le renderer d'incrustation
		OverlayRendererSPtr m_pRenderer;
		//!\~english The overlay count, per level	\~french Le nombre d'incrustations par niveau
		std::vector< int > m_overlayCountPerLevel;
	};
	typedef OverlayManager::iterator OverlayManagerIt;
	typedef OverlayManager::const_iterator OverlayManagerConstIt;
}

#pragma warning( pop )

#endif
