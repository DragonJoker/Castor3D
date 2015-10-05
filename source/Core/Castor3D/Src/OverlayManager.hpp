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
	\date 		03/10/2015
	\version	0.8.0
	\~english
	\brief		Structure used to sort overlays by order.
	\~french
	\brief		Structure utilisée pour trier les incrustations par ordre.
	*/
	struct OverlayCategorySort
	{
		/**
		 *\~english
		 *\brief		Comparison operator.
		 *\~french
		 *\brief		Opérateur de comparaison.
		 */
		bool operator()( OverlayCategorySPtr p_a, OverlayCategorySPtr p_b )
		{
			return p_a->GetLevel() < p_b->GetLevel() || ( p_a->GetLevel() == p_b->GetLevel() && p_a->GetIndex() < p_b->GetIndex() );
		}
	};
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
		: private Castor::Collection< Overlay, Castor::String >
	{
	public:
		typedef Castor::Collection< Overlay, Castor::String >::TObjPtrMapIt iterator;
		typedef Castor::Collection< Overlay, Castor::String >::TObjPtrMapConstIt const_iterator;
		typedef std::set< OverlayCategorySPtr, OverlayCategorySort > OverlayCategorySet;
		DECLARE_MAP( Castor::String, FontTextureSPtr, FontTextureStr );

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		OverlayManager( Engine * p_engine );
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
		void Clear();
		/**
		 *\~english
		 *\brief		Cleans all overlays up.
		 *\~french
		 *\brief		Nettoie les incrustations.
		 */
		void Cleanup();
		/**
		 *\~english
		 *\brief		Creates a panel overlay.
		 *\remarks		Posts the intialisation event to the engine.
		 *\param[in]	p_name		The overlay name.
		 *\param[in]	p_position	The position, relative to parent, or screen, if no parent.
		 *\param[in]	p_size		The size, relative to parent, or screen, if no parent.
		 *\param[in]	p_material	The overlay material.
		 *\param[in]	p_parent	The parent overlay.
		 *\return		The created overlay.
		 *\~french
		 *\brief		Crée une incrustation panneau.
		 *\remarks		Poste l'évènement d'initialisation au moteur.
		 *\param[in]	p_name		Le nom de l'incrustation.
		 *\param[in]	p_position	La position relative au parent, ou à l'écran, si pas de parent.
		 *\param[in]	p_size		La taille relative au parent, ou à l'écran, si pas de parent.
		 *\param[in]	p_material	Le matériau de l'incrustation.
		 *\param[in]	p_parent	L'incrustation parente.
		 *\return		L'incrustation ainsi créée.
		 */
		PanelOverlaySPtr CreatePanel( Castor::String const & p_name, Castor::Point2d const & p_position, Castor::Point2d const & p_size, MaterialSPtr p_material, OverlaySPtr p_parent = nullptr );
		/**
		 *\~english
		 *\brief		Creates a panel overlay.
		 *\remarks		Posts the intialisation event to the engine.
		 *\param[in]	p_name		The overlay name.
		 *\param[in]	p_position	The position in pixels, inside the parent, or screen if no parent.
		 *\param[in]	p_size		The absolute size in pixels.
		 *\param[in]	p_material	The overlay material.
		 *\param[in]	p_parent	The parent overlay.
		 *\return		The created overlay.
		 *\~french
		 *\brief		Crée une incrustation panneau.
		 *\remarks		Poste l'évènement d'initialisation au moteur.
		 *\param[in]	p_name		Le nom de l'incrustation.
		 *\param[in]	p_position	La position en pixels, dans le parent, ou l'écran, si pas de parent.
		 *\param[in]	p_size		La taille absolue, en pixels.
		 *\param[in]	p_material	Le matériau de l'incrustation.
		 *\param[in]	p_parent	L'incrustation parente.
		 *\return		L'incrustation ainsi créée.
		 */
		PanelOverlaySPtr CreatePanel( Castor::String const & p_name, Castor::Position const & p_position, Castor::Size const & p_size, MaterialSPtr p_material, OverlaySPtr p_parent = nullptr );
		/**
		 *\~english
		 *\brief		Creates a border panel overlay.
		 *\remarks		Posts the intialisation event to the engine.
		 *\param[in]	p_name				The overlay name.
		 *\param[in]	p_position			The position, relative to parent, or screen, if no parent.
		 *\param[in]	p_size				The size, relative to parent, or screen, if no parent.
		 *\param[in]	p_material			The overlay material.
		 *\param[in]	p_bordersSize		The overlay borders size.
		 *\param[in]	p_bordersMaterial	The overlay borders material.
		 *\param[in]	p_parent			The parent overlay.
		 *\return		The created overlay.
		 *\~french
		 *\brief		Crée une incrustation panneau borduré.
		 *\remarks		Poste l'évènement d'initialisation au moteur.
		 *\param[in]	p_name				Le nom de l'incrustation.
		 *\param[in]	p_position			La position relative au parent, ou à l'écran, si pas de parent.
		 *\param[in]	p_size				La taille relative au parent, ou à l'écran, si pas de parent.
		 *\param[in]	p_material			Le matériau de l'incrustation.
		 *\param[in]	p_bordersSize		Les dimensions des bords de l'incrustation.
		 *\param[in]	p_bordersMaterial	Le matériau des bordures de l'incrustation.
		 *\param[in]	p_parent			L'incrustation parente.
		 *\return		L'incrustation ainsi créée.
		 */
		BorderPanelOverlaySPtr CreateBorderPanel( Castor::String const & p_name, Castor::Point2d const & p_position, Castor::Point2d const & p_size, MaterialSPtr p_material, Castor::Point4d const & p_bordersSize, MaterialSPtr p_bordersMaterial, OverlaySPtr p_parent = nullptr );
		/**
		 *\~english
		 *\brief		Creates a panel overlay.
		 *\remarks		Posts the intialisation event to the engine.
		 *\param[in]	p_name				The overlay name.
		 *\param[in]	p_position			The position in pixels, inside the parent, or screen if no parent.
		 *\param[in]	p_size				The absolute size in pixels.
		 *\param[in]	p_material			The overlay material.
		 *\param[in]	p_bordersSize		The overlay borders pixel size.
		 *\param[in]	p_bordersMaterial	The overlay borders material.
		 *\param[in]	p_parent			The parent overlay.
		 *\return		The created overlay.
		 *\brief		Crée une incrustation panneau borduré.
		 *\remarks		Poste l'évènement d'initialisation au moteur.
		 *\param[in]	p_name				Le nom de l'incrustation.
		 *\param[in]	p_position			La position en pixels, dans le parent, ou l'écran, si pas de parent.
		 *\param[in]	p_size				La taille absolue, en pixels.
		 *\param[in]	p_material			Le matériau de l'incrustation.
		 *\param[in]	p_bordersSize		Les dimensions des bords de l'incrustation.
		 *\param[in]	p_bordersMaterial	Le matériau des bordures de l'incrustation.
		 *\param[in]	p_parent			L'incrustation parente.
		 *\return		L'incrustation ainsi créée.
		 */
		BorderPanelOverlaySPtr CreateBorderPanel( Castor::String const & p_name, Castor::Position const & p_position, Castor::Size const & p_size, MaterialSPtr p_material, Castor::Rectangle const & p_bordersSize, MaterialSPtr p_bordersMaterial, OverlaySPtr p_parent = nullptr );
		/**
		 *\~english
		 *\brief		Creates a text overlay.
		 *\remarks		Posts the intialisation event to the engine.
		 *\param[in]	p_name		The overlay name.
		 *\param[in]	p_position	The position, relative to parent, or screen, if no parent.
		 *\param[in]	p_size		The size, relative to parent, or screen, if no parent.
		 *\param[in]	p_material	The overlay material.
		 *\param[in]	p_font		The font used to display the text.
		 *\param[in]	p_parent	The parent overlay.
		 *\return		The created overlay.
		 *\~french
		 *\brief		Crée une incrustation texte.
		 *\remarks		Poste l'évènement d'initialisation au moteur.
		 *\param[in]	p_name		Le nom de l'incrustation.
		 *\param[in]	p_position	La position relative au parent, ou à l'écran, si pas de parent.
		 *\param[in]	p_size		La taille relative au parent, ou à l'écran, si pas de parent.
		 *\param[in]	p_material	Le matériau de l'incrustation.
		 *\param[in]	p_font		La police utilisée pour afficher le texte.
		 *\param[in]	p_parent	L'incrustation parente.
		 *\return		L'incrustation ainsi créée.
		 */
		TextOverlaySPtr CreateText( Castor::String const & p_name, Castor::Point2d const & p_position, Castor::Point2d const & p_size, MaterialSPtr p_material, Castor::FontSPtr p_font, OverlaySPtr p_parent = nullptr );
		/**
		 *\~english
		 *\brief		Creates a text overlay.
		 *\remarks		Posts the intialisation event to the engine.
		 *\param[in]	p_name		The overlay name.
		 *\param[in]	p_position	The position in pixels, inside the parent, or screen if no parent.
		 *\param[in]	p_size		The absolute size in pixels.
		 *\param[in]	p_material	The overlay material.
		 *\param[in]	p_font		The font used to display the text.
		 *\param[in]	p_parent	The parent overlay.
		 *\return		The created overlay.
		 *\~french
		 *\brief		Crée une incrustation panneau.
		 *\remarks		Poste l'évènement d'initialisation au moteur.
		 *\param[in]	p_name		Le nom de l'incrustation.
		 *\param[in]	p_position	La position en pixels, dans le parent, ou l'écran, si pas de parent.
		 *\param[in]	p_size		La taille absolue, en pixels.
		 *\param[in]	p_material	Le matériau de l'incrustation.
		 *\param[in]	p_font		La police utilisée pour afficher le texte.
		 *\param[in]	p_parent	L'incrustation parente.
		 *\return		L'incrustation ainsi créée.
		 */
		TextOverlaySPtr CreateText( Castor::String const & p_name, Castor::Position const & p_position, Castor::Size const & p_size, MaterialSPtr p_material, Castor::FontSPtr p_font, OverlaySPtr p_parent = nullptr );
		/**
		 *\~english
		 *\brief		Creates an overlay, given a type and the overlay definitions
		 *\remark		If an overlay with the given name already exists, no creation is done, the return is the existing overlay
		 *\param[in]	p_type		The overlay type (panel, text ...)
		 *\param[in]	p_name	The overlay name
		 *\param[in]	p_parent	The parent overlay, NULL if none
		 *\param[in]	p_scene	The scene that holds the overlay
		 *\return		The created overlay
		 *\~french
		 *\brief		Crée un overlay
		 *\remark		Si un overlay avec le même nom existe déjà, aucune création n'est faite, l'existant est retourné
		 *\param[in]	p_type		Le type d'overlay
		 *\param[in]	p_name	Le nom voulu pour l'overlay
		 *\param[in]	p_parent	L'overlay parent, nullptr si aucun
		 *\param[in]	p_scene	La scène contenant l'overlay
		 *\return		L'overlay
		 */
		OverlaySPtr CreateOverlay( eOVERLAY_TYPE p_type, Castor::String const & p_name, OverlaySPtr p_parent, SceneSPtr p_scene );
		/**
		 *\~english
		 *\brief		Add an overlay to the lists, given it's name
		 *\param[in]	p_name		The overlay name
		 *\param[in]	p_overlay	The overlay
		 *\param[in]	p_parent	The parent overlay
		 *\~french
		 *\brief		Ajoute une incrustation aux listes, selon son nom
		 *\param[in]	p_name		Le nom
		 *\param[in]	p_overlay	L'incrustation
		 *\param[in]	p_parent	L'incrustation parente
		 */
		void AddOverlay( Castor::String const & p_name, OverlaySPtr p_overlay, OverlaySPtr p_parent );
		/**
		 *\~english
		 *\brief		Removes an overlay from the lists.
		 *\param[in]	p_name		The overlay name.
		 *\~french
		 *\brief		Enlève une incrustation des listes.
		 *\param[in]	p_name		Le nom de l'incrustation.
		 */
		void RemoveOverlay( Castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Retrieves the overlay with the given name
		 *\param[in]	p_name	The name
		 *\return		The overlay, \p nullptr if not found
		 *\~french
		 *\brief		Récupère l'incrustation avec le nom donné
		 *\param[in]	p_name	Le nom
		 *\return		L'incrustation, \p nullptr si non trouvée
		 */
		OverlaySPtr GetOverlay( Castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Checks if an overlay with the given name exists
		 *\param[in]	p_name	The name
		 *\return		\p true if an overlay is defined with given name
		 *\~french
		 *\brief		Vérifie si une incrustation avec le nom donné existe
		 *\param[in]	p_name	Le nom
		 *\return		\p true Si une incrustation est défini avec le nom donné
		 */
		bool HasOverlay( Castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Initialises or cleans up the OverlayRenderer, according to engine rendering status
		 *\~french
		 *\brief		Initialise ou nettoie l'OverlayRenderer, selon le statut du rendu
		 */
		void Update();
		/**
		 *\~english
		 *\brief		Renders all visible overlays
		 *\param[in]	p_scene	The scene displayed, to display its overlays and the global ones
		 *\param[in]	p_size	The render target size
		 *\~french
		 *\brief		Fonction de rendu des overlays visibles
		 *\param[in]	p_scene	La scène rendue, pour afficher ses overlays en plus des globaux
		 *\param[in]	p_size	Les dimensions de la cible du rendu
		 */
		void RenderOverlays( Scene const & p_scene, Castor::Size const & p_size );
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
		*\brief		Retrieves a FontTexture given a font name.
		*\param[in]	p_name	The font name.
		*\return		The FontTexture if it exist, nullptr if not.
		*\~french
		*\brief		Récupère une FontTexture, à partir d'un nom de police.
		*\param[in]	p_name	Le nom de la police.
		*\return		La FontTexture si elle exite, nullptr sinon.
		*/
		FontTextureSPtr GetFontTexture( Castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Creates a FontTexture from a font.
		 *\param[in]	p_font	The font.
		 *\return		The created FontTexture.
		 *\~french
		 *\brief		Crée une FontTexture, à partir d'une police.
		 *\param[in]	p_font	La police.
		 *\return		La FontTexture créée.
		 */
		FontTextureSPtr CreateFontTexture( Castor::FontSPtr p_font );
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
		inline OverlayCategorySet::iterator begin()
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
		inline OverlayCategorySet::const_iterator begin()const
		{
			return m_overlays.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to after the last overlay
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur après la dernière incrustation
		 *\return		La valeur
		 */
		inline OverlayCategorySet::iterator end()
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
		inline OverlayCategorySet::const_iterator end()const
		{
			return m_overlays.end();
		}

	private:
		//!\~english The overlays, in rendering order.	\~french Les incrustations, dans l'ordre de rendu.
		OverlayCategorySet m_overlays;
		//!\~english The engine	\~french Le moteur
		Engine * m_engine;
		//!\~english The overlay renderer	\~french le renderer d'incrustation
		OverlayRendererSPtr m_pRenderer;
		//!\~english The overlay count, per level	\~french Le nombre d'incrustations par niveau
		std::vector< int > m_overlayCountPerLevel;
		//!\~english The pojection matrix.	\~french La matrice de projection.
		Castor::Matrix4x4r m_projection;
		//!\~english The last display size.	\~french Les dimensions du dernier affichage.
		Castor::Size m_size;
		//!\~english The FontTextures, sorted by font name.	\~french Les FontTexutrs, triées par nom de policce.
		FontTextureStrMap m_fontTextures;
	};
	typedef OverlayManager::iterator OverlayManagerIt;
	typedef OverlayManager::const_iterator OverlayManagerConstIt;
}

#pragma warning( pop )

#endif
