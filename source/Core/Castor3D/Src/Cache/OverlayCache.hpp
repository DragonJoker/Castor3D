/*
See LICENSE file in root folder
*/
#ifndef ___C3D_OVERLAY_CACHE_H___
#define ___C3D_OVERLAY_CACHE_H___

#include "Overlay/Overlay.hpp"
#include "Cache/Cache.hpp"
#include "Overlay/OverlayFactory.hpp"
#include "Render/Viewport.hpp"

#include <Design/OwnedBy.hpp>

namespace castor3d
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
		C3D_API static const castor::String Name;
		using Producer = std::function< std::shared_ptr< Overlay >( KeyType const &, OverlayType, SceneSPtr, OverlaySPtr ) >;
		using Merger = std::function< void( CacheBase< Overlay, KeyType > const &
											, castor::Collection< Overlay, KeyType > &
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
		inline bool operator()( OverlayCategorySPtr p_a, OverlayCategorySPtr p_b )const
		{
			return p_a->getLevel() < p_b->getLevel() || ( p_a->getLevel() == p_b->getLevel() && p_a->getIndex() < p_b->getIndex() );
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
	class Cache< Overlay, castor::String >
		: public CacheBase< Overlay, castor::String >
	{
	public:
		using MyCacheType = CacheBase< Overlay, castor::String >;
		using Element = typename MyCacheType::Element;
		using Key = typename MyCacheType::Key;
		using Collection = typename MyCacheType::Collection;
		using ElementPtr = typename MyCacheType::ElementPtr;
		using Producer = typename MyCacheType::Producer;
		using Merger = typename MyCacheType::Merger;

		typedef castor::Collection< Overlay, castor::String >::TObjPtrMapIt iterator;
		typedef castor::Collection< Overlay, castor::String >::TObjPtrMapConstIt const_iterator;
		DECLARE_MAP( castor::String, FontTextureSPtr, FontTextureStr );

		struct OverlayInitialiser
		{
			explicit OverlayInitialiser( Cache< Overlay, castor::String > & cache );
			void operator()( OverlaySPtr p_element );

		private:
			OverlayCategorySet & m_overlays;
			std::vector< int > & m_overlayCountPerLevel;
		};

		struct OverlayCleaner
		{
			explicit OverlayCleaner( Cache< Overlay, castor::String > & cache );
			void operator()( OverlaySPtr p_element );

		private:
			OverlayCategorySet & m_overlays;
			std::vector< int > & m_overlayCountPerLevel;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine		The engine.
		 *\param[in]	p_produce		The element producer.
		 *\param[in]	p_initialise	The element initialiser.
		 *\param[in]	p_clean			The element cleaner.
		 *\param[in]	p_merge			The element collection merger.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	p_produce		Le créateur d'objet.
		 *\param[in]	p_initialise	L'initialiseur d'objet.
		 *\param[in]	p_clean			Le nettoyeur d'objet.
		 *\param[in]	p_merge			Le fusionneur de collection d'objets.
		 */
		C3D_API Cache( Engine & engine
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
		C3D_API void clear();
		/**
		 *\~english
		 *\brief		Cleans all overlays up.
		 *\~french
		 *\brief		Nettoie les incrustations.
		 */
		C3D_API void cleanup();
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
		C3D_API FontTextureSPtr getFontTexture( castor::String const & p_name );
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
		C3D_API FontTextureSPtr createFontTexture( castor::FontSPtr p_font );
		/**
		 *\~english
		 *\brief		Creates an object.
		 *\param[in]	p_name		The object name.
		 *\param[in]	p_type		The overlay type.
		 *\param[in]	p_scene		The scene.
		 *\param[in]	p_parent	The parent overlay, if any.
		 *\return		The created object.
		 *\~french
		 *\brief		Crée un objet.
		 *\param[in]	p_name		Le nom d'objet.
		 *\param[in]	p_type		Le type d'incrustation.
		 *\param[in]	p_scene		La scène.
		 *\param[in]	p_parent	L'incrustation parente, si elle existe.
		 *\return		L'objet créé.
		 */
		inline ElementPtr add( Key const & p_name, OverlayType p_type, SceneSPtr p_scene, OverlaySPtr p_parent )
		{
			return MyCacheType::add( p_name, p_type, p_scene, p_parent );
		}
		/**
		 *\~english
		 *\brief		adds an already created object.
		 *\param[in]	p_name		The object name.
		 *\param[in]	p_element	The object.
		 *\return		The object.
		 *\~french
		 *\brief		Ajoute un objet déjà créé.
		 *\param[in]	p_name		Le nom d'objet.
		 *\param[in]	p_element	L'objet'.
		 *\return		L'objet.
		 */
		inline ElementPtr add( Key const & p_name, ElementPtr p_element )
		{
			return MyCacheType::add( p_name, p_element );
		}
		/**
		 *\~english
		 *\brief		Removes an element, given a name.
		 *\param[in]	name		The element name.
		 *\~french
		 *\brief		Retire un élément à partir d'un nom.
		 *\param[in]	name		Le nom d'élément.
		 */
		inline void remove( Key const & name )
		{
			if ( m_elements.has( name ) )
			{
				auto element = m_elements.find( name );
				m_clean( element );
				m_elements.erase( name );
			}
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
		inline OverlayFactory const & getOverlayFactory()const
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
		inline OverlayFactory & getFactory()
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
		//!\~english	The rendering viewport.
		//!\~french		Le viewport de rendu.
		Viewport m_viewport;
		//!\~english	The overlay count, per level.
		//!\~french		Le nombre d'incrustations par niveau.
		std::vector< int > m_overlayCountPerLevel;
		//!\~english	The pojection matrix.
		//!\~french		La matrice de projection.
		castor::Matrix4x4r m_projection;
		//!\~english	The FontTextures, sorted by font name.
		//!\~french		Les FontTexutrs, triées par nom de police.
		FontTextureStrMap m_fontTextures;
	};
	using OverlayCache = Cache< Overlay, castor::String >;
	DECLARE_SMART_PTR( OverlayCache );
}

#endif
