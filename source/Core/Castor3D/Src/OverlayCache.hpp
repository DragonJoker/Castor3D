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
#ifndef ___C3D_OVERLAY_CACHE_H___
#define ___C3D_OVERLAY_CACHE_H___

#include "Overlay/Overlay.hpp"
#include "Cache/Cache.hpp"
#include "Overlay/OverlayFactory.hpp"
#include "Render/Viewport.hpp"

#include <Design/OwnedBy.hpp>

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		04/02/2016
	\version	0.8.0
	\~english
	\brief		Helper structure to specialise a cache behaviour.
	\remarks	Specialisation for Overlay.
	\~french
	\brief		Structure permettant de spécialiser le comportement d'un cache.
	\remarks	Spécialisation pour Overlay.
	*/
	template< typename KeyType >
	struct CacheTraits< Overlay, KeyType >
	{
		C3D_API static const Castor::String Name;
		using Producer = std::function< std::shared_ptr< Overlay >( KeyType const &, eOVERLAY_TYPE, SceneSPtr, OverlaySPtr ) >;
		using Merger = std::function< void( CacheBase< Overlay, KeyType > const &
											, Castor::Collection< Overlay, KeyType > &
											, std::shared_ptr< Overlay > ) >;
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
	using OverlayCategorySet = std::set< OverlayCategorySPtr, OverlayCategorySort >;
	/*!
	\author 	Sylvain DOREMUS
	\date 		09/02/2010
	\version	0.1
	\~english
	\brief		Overlay collection, with additional add and remove functions to manage Z-Index
	\~french
	\brief		Collection d'incrustations, avec des fonctions additionnelles d'ajout et de suppression pour gérer les Z-Index
	*/
	template<>
	class Cache< Overlay, Castor::String >
		: public CacheBase< Overlay, Castor::String >
	{
	public:
		using MyCacheType = CacheBase< Overlay, Castor::String >;
		using Element = typename MyCacheType::Element;
		using Key = typename MyCacheType::Key;
		using Collection = typename MyCacheType::Collection;
		using ElementPtr = typename MyCacheType::ElementPtr;
		using Producer = typename MyCacheType::Producer;
		using Merger = typename MyCacheType::Merger;

		typedef Castor::Collection< Overlay, Castor::String >::TObjPtrMapIt iterator;
		typedef Castor::Collection< Overlay, Castor::String >::TObjPtrMapConstIt const_iterator;
		DECLARE_MAP( Castor::String, FontTextureSPtr, FontTextureStr );

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		C3D_API Cache( Engine & p_engine
					   , Producer && p_produce
					   , Initialiser && p_initialise = Initialiser{}
					   , Cleaner && p_clean = Cleaner{}
					   , Merger && p_merge = Merger{} );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~Cache();
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
	using OverlayCache = Cache< Overlay, Castor::String >;
	DECLARE_SMART_PTR( OverlayCache );
}

#endif
