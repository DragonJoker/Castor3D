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
		:	private OverlayCollection
	{
	public:
		typedef OverlayCollectionIt			iterator;
		typedef OverlayCollectionConstIt	const_iterator;

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
		 *\brief		Add an overlay to the lists, given it's z-index
		 *\param[in]	p_iZIndex	The wanted z-index
		 *\param[in]	p_pOverlay	The overlay
		 *\~french
		 *\brief		Ajoute une incrustation aux listes, selon son z-index
		 *\param[in]	p_iZIndex	Le z-index voulu
		 *\param[in]	p_pOverlay	L'incrustation
		 */
		void AddOverlay( int p_iZIndex, OverlaySPtr p_pOverlay );
		/**
		 *\~english
		 *\brief		Checks if an overlay exists at given z-index
		 *\param[in]	p_iZIndex	The z-index
		 *\return		\p true if an overlay is defined at the given z-index
		 *\~french
		 *\brief		Vérifie si une incrustation existe au z-index donné
		 *\param[in]	p_iZIndex	Le z-index
		 *\return		\p true si un overlayest défini au z-index donné
		 */
		bool HasOverlay( int p_iZIndex );
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
		 *\brief		Retrieves the overlay at given z-index
		 *\param[in]	p_iZIndex	The z-index
		 *\return		The overlay, \p nullptr if not found
		 *\~french
		 *\brief		Récupère l'incrustation au z-index donné
		 *\param[in]	p_iZIndex	Le z-index
		 *\return		L'incrustation, \p nullptr si non trouvée
		 */
		OverlaySPtr GetOverlay( int p_iZIndex );
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
		inline OverlayPtrIntMap::iterator Begin()
		{
			return m_mapOverlaysByZIndex.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the first overlay
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur la première incrustation
		 *\return		La valeur
		 */
		inline OverlayPtrIntMap::const_iterator Begin()const
		{
			return m_mapOverlaysByZIndex.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the first overlay
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur la première incrustation
		 *\return		La valeur
		 */
		inline OverlayPtrIntMap::reverse_iterator RBegin()
		{
			return m_mapOverlaysByZIndex.rbegin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the first overlay
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur la première incrustation
		 *\return		La valeur
		 */
		inline OverlayPtrIntMap::const_reverse_iterator RBegin()const
		{
			return m_mapOverlaysByZIndex.rbegin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to after the last overlay
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur après la dernière incrustation
		 *\return		La valeur
		 */
		inline OverlayPtrIntMap::iterator End()
		{
			return m_mapOverlaysByZIndex.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to after the last overlay
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur après la dernière incrustation
		 *\return		La valeur
		 */
		inline OverlayPtrIntMap::const_iterator End()const
		{
			return m_mapOverlaysByZIndex.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to after the last overlay
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur après la dernière incrustation
		 *\return		La valeur
		 */
		inline OverlayPtrIntMap::reverse_iterator REnd()
		{
			return m_mapOverlaysByZIndex.rend();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to after the last overlay
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur après la dernière incrustation
		 *\return		La valeur
		 */
		inline OverlayPtrIntMap::const_reverse_iterator REnd()const
		{
			return m_mapOverlaysByZIndex.rend();
		}
		/**
		 *\~english
		 *\brief		Begins the overlays rendering
		 *\param[in]	p_size	The render window size
		 *\~french
		 *\brief		Commence le rendu des incrustations
		 *\param[in]	p_size	La taille de la fenêtre de rendu
		 */
		void BeginRendering( Castor::Size const & p_size );
		/**
		 *\~english
		 *\brief		Ends the overlays rendering
		 *\~french
		 *\brief		Termine le rendu des incrustations
		 */
		void EndRendering();

		using OverlayCollection::lock;
		using OverlayCollection::unlock;

	private:
		//!\~english Current available Z index	\~french Z index disponible
		int m_iCurrentZIndex;
		//!\~english The overlays, sorted by Z index	\~french Les incrustations, triées par Z index
		OverlayPtrIntMap m_mapOverlaysByZIndex;
		//!\~english The engine	\~french Le moteur
		Engine * m_pEngine;
		//!\~english The overlay renderer	\~french le renderer d'incrustation
		OverlayRendererSPtr m_pRenderer;
	};
	typedef OverlayManager::iterator		OverlayManagerIt;
	typedef OverlayManager::const_iterator	OverlayManagerConstIt;
}

#pragma warning( pop )

#endif
