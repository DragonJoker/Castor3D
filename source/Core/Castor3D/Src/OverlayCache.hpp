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
	using OverlayCategorySet = std::set< OverlayCategorySPtr, OverlayCategorySort >;
	/*!
	\author 	Sylvain DOREMUS
	\date 		13/10/2015
	\version	0.8.0
	\~english
	\brief		Helper structure to enable initialisation if a type supports it.
	\remarks	Specialisation for Light.
	\~french
	\brief		Structure permettant d'initialiser les éléments qui le supportent.
	\remarks	Spécialisation pour Light.
	*/
	template<>
	struct ElementInitialiser< Overlay >
	{
		ElementInitialiser( OverlayCategorySet & p_overlays, std::vector< int > & p_overlayCountPerLevel )
			: m_overlays{ p_overlays }
			, m_overlayCountPerLevel{ p_overlayCountPerLevel }
		{
		}

		inline void operator()( Engine & p_engine, OverlaySPtr p_element )
		{
			int l_level = 0;

			if ( p_element->GetParent() )
			{
				l_level = p_element->GetParent()->GetLevel() + 1;
				p_element->GetParent()->AddChild( p_element );
			}

			while ( l_level >= int( m_overlayCountPerLevel.size() ) )
			{
				m_overlayCountPerLevel.resize( m_overlayCountPerLevel.size() * 2 );
			}

			p_element->SetOrder( ++m_overlayCountPerLevel[l_level], l_level );
			m_overlays.insert( p_element->GetCategory() );
		}

		OverlayCategorySet & m_overlays;
		std::vector< int > & m_overlayCountPerLevel;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		13/10/2015
	\version	0.8.0
	\~english
	\brief		Helper structure to enable cleanup if a type supports it.
	\remarks	Specialisation for Light.
	\~french
	\brief		Structure permettant de nettoyer les éléments qui le supportent.
	\remarks	Spécialisation pour Light.
	*/
	template<>
	struct ElementCleaner< Overlay >
	{
		ElementCleaner( OverlayCategorySet & p_overlays, std::vector< int > & p_overlayCountPerLevel )
			: m_overlays{ p_overlays }
			, m_overlayCountPerLevel{ p_overlayCountPerLevel }
		{
		}

		inline void operator()( Engine & p_engine, OverlaySPtr p_element )
		{
			if ( p_element->GetChildrenCount() )
			{
				for ( auto l_child : *p_element )
				{
					l_child->SetPosition( l_child->GetAbsolutePosition() );
					l_child->SetSize( l_child->GetAbsoluteSize() );
				}
			}
		}

		OverlayCategorySet & m_overlays;
		std::vector< int > & m_overlayCountPerLevel;
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
	class OverlayCache
		: public Cache< Overlay, Castor::String, OverlayProducer >
	{
	public:
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
