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
#ifndef ___C3D_OVERLAY_CACHE_H___
#define ___C3D_OVERLAY_CACHE_H___

#include "Cache/ResourceCache.hpp"
#include "Overlay/Overlay.hpp"
#include "Overlay/OverlayFactory.hpp"
#include "Render/Viewport.hpp"

#include <OwnedBy.hpp>

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		04/02/2016
	\version	0.8.0
	\~english
	\brief		Helper structure to get an object type name.
	\~french
	\brief		Structure permettant de récupérer le nom du type d'un objet.
	*/
	template<> struct CachedObjectNamer< Overlay >
	{
		C3D_API static const Castor::String Name;
	};
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
		inline bool operator()( OverlayCategorySPtr p_a, OverlayCategorySPtr p_b )
		{
			return p_a->GetLevel() < p_b->GetLevel() || ( p_a->GetLevel() == p_b->GetLevel() && p_a->GetIndex() < p_b->GetIndex() );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		04/07/2016
	\version	0.9.0
	\~english
	\brief		Helper structure to create an element.
	\~french
	\brief		Structure permettant de créer un élément.
	*/
	template<>
	struct ElementProducer< Overlay, Castor::String, eOVERLAY_TYPE, OverlaySPtr, SceneSPtr >
	{
		using ElemPtr = std::shared_ptr< Overlay >;

		inline ElementProducer( Engine & p_engine )
			: m_engine{ p_engine }
		{
		}

		inline ElemPtr operator()( Castor::String const & p_key, eOVERLAY_TYPE p_type, SceneSPtr p_scene, OverlaySPtr p_parent )
		{
			auto l_return = std::make_shared< Overlay >( m_engine, p_type, p_scene, p_parent );
			l_return->SetName( p_key );
			return l_return;
		}

		Engine & m_engine;
	};
	using OverlayProducer = ElementProducer< Overlay, Castor::String, eOVERLAY_TYPE, OverlaySPtr, SceneSPtr >;
	/*!
	\author 	Sylvain DOREMUS
	\date 		09/02/2010
	\version	0.1
	\~english
	\brief		Overlay collection, with additional add and remove functions to manage Z-Index
	\~french
	\brief		Collection d'incrustations, avec des fonctions additionnelles d'ajout et de suppression pour gérer les Z-Index
	*/
	class OverlayCache
		: public Cache< Overlay, Castor::String, OverlayProducer >
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
		C3D_API OverlayCache( EngineGetter && p_get, OverlayProducer && p_produce );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~OverlayCache();
		/**
		 *\~english
		 *\brief		Clears all overlays lists
		 *\~french
		 *\brief		Vide les listes d'incrustations
		 */
		C3D_API void Clear();
		/**
		 *\~english
		 *\brief		Cleans all overlays up.
		 *\~french
		 *\brief		Nettoie les incrustations.
		 */
		C3D_API void Cleanup();
		/**
		 *\~english
		 *\brief		Creates an overlay, given a type and the overlay definitions
		 *\remarks		If an overlay with the given name already exists, no creation is done, the return is the existing overlay
		 *\param[in]	p_type		The overlay type (panel, text ...)
		 *\param[in]	p_name	The overlay name
		 *\param[in]	p_parent	The parent overlay, nullptr if none
		 *\param[in]	p_scene	The scene that holds the overlay
		 *\return		The created overlay
		 *\~french
		 *\brief		Crée un overlay
		 *\remarks		Si un overlay avec le même nom existe déjà, aucune création n'est faite, l'existant est retourné
		 *\param[in]	p_type		Le type d'overlay
		 *\param[in]	p_name	Le nom voulu pour l'overlay
		 *\param[in]	p_parent	L'overlay parent, nullptr si aucun
		 *\param[in]	p_scene	La scène contenant l'overlay
		 *\return		L'overlay
		 */
		C3D_API OverlaySPtr Add( Castor::String const & p_name, eOVERLAY_TYPE p_type, OverlaySPtr p_parent, SceneSPtr p_scene );
		/**
		 *\~english
		 *\brief		Removes an overlay from the lists.
		 *\param[in]	p_name		The overlay name.
		 *\~french
		 *\brief		Enlève une incrustation des listes.
		 *\param[in]	p_name		Le nom de l'incrustation.
		 */
		C3D_API void Remove( Castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Initialises or cleans up the OverlayRenderer, according to engine rendering status
		 *\~french
		 *\brief		Initialise ou nettoie l'OverlayRenderer, selon le statut du rendu
		 */
		C3D_API void UpdateRenderer();
		/**
		 *\~english
		 *\brief		Updates overlays.
		 *\~french
		 *\brief		Met à jour les incrustations.
		 */
		C3D_API void Update();
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
		C3D_API void Render( Scene const & p_scene, Castor::Size const & p_size );
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
		C3D_API bool Write( Castor::TextFile & p_file )const;
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
		C3D_API bool Read( Castor::TextFile & p_file );
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
		C3D_API FontTextureSPtr GetFontTexture( Castor::String const & p_name );
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
		C3D_API FontTextureSPtr CreateFontTexture( Castor::FontSPtr p_font );
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
		/**
		 *\~english
		 *\brief		Retrieves the Overlay factory
		 *\return		The factory
		 *\~french
		 *\brief		Récupère la fabrique d'Overlay
		 *\return		La fabrique
		 */
		inline OverlayFactory const & GetOverlayFactory()const
		{
			return m_overlayFactory;
		}
		/**
		 *\~english
		 *\brief		Retrieves the Overlay factory
		 *\return		The factory
		 *\~french
		 *\brief		Récupère la fabrique d'Overlay
		 *\return		La fabrique
		 */
		inline OverlayFactory & GetFactory()
		{
			return m_overlayFactory;
		}

	private:
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
		C3D_API void DoAddOverlay( Castor::String const & p_name, OverlaySPtr p_overlay, OverlaySPtr p_parent );

	private:
		//!\~english	The OverlayCategory factory.
		//!\~french		La fabrique de OverlayCategory.
		OverlayFactory m_overlayFactory;
		//!\~english	The overlays, in rendering order.
		//!\~french		Les incrustations, dans l'ordre de rendu.
		OverlayCategorySet m_overlays;
		//!\~english	The overlay renderer.
		//!\~french		le renderer d'incrustation.
		OverlayRendererSPtr m_pRenderer;
		//!\~english	The rendering viewport.
		//!\~french		Le viewport de rendu.
		Viewport m_viewport;
		//!\~english	The overlay count, per level.
		//!\~french		Le nombre d'incrustations par niveau.
		std::vector< int > m_overlayCountPerLevel;
		//!\~english	The pojection matrix.
		//!\~french		La matrice de projection.
		Castor::Matrix4x4r m_projection;
		//!\~english	The FontTextures, sorted by font name.
		//!\~french		Les FontTexutrs, triées par nom de police.
		FontTextureStrMap m_fontTextures;
	};
	typedef OverlayCache::iterator OverlayCacheIt;
	typedef OverlayCache::const_iterator OverlayCacheConstIt;
	/**
	 *\~english
	 *\brief		Creates a Scene cache.
	 *\param[in]	p_get		The engine getter.
	 *\param[in]	p_produce	The element producer.
	 *\~french
	 *\brief		Crée un cache de Scene.
	 *\param[in]	p_get		Le récupérteur de moteur.
	 *\param[in]	p_produce	Le créateur d'objet.
	 */
	inline std::unique_ptr< OverlayCache >
	MakeCache( EngineGetter && p_get, OverlayProducer && p_produce )
	{
		return std::make_unique< OverlayCache >( std::move( p_get ), std::move( p_produce ) );
	}
}

#endif
