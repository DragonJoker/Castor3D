/*
See LICENSE file in root folder
*/
#ifndef ___C3D_CacheView_H___
#define ___C3D_CacheView_H___

#include "CacheModule.hpp"

#include "Castor3D/Event/Frame/FrameEventModule.hpp"

#include <CastorUtils/Design/Named.hpp>

namespace castor3d
{
	template< typename CacheT
		, EventType EventT >
	class CacheViewT
		: public castor::Named
	{
	public:
		using ElementCacheT = CacheT;
		using ElementT = typename ElementCacheT::ElementT;
		using ElementKeyT = typename ElementCacheT::ElementKeyT;
		using ElementCacheTraitsT = typename ElementCacheT::ElementCacheTraitsT;
		using ElementPtrT = typename ElementCacheT::ElementPtrT;
		using ElementContT = typename ElementCacheT::ElementContT;
		using ElementProducerT = typename ElementCacheT::ElementProducerT;
		using ElementInitialiserT = typename ElementCacheT::ElementInitialiserT;
		using ElementCleanerT = typename ElementCacheT::ElementCleanerT;
		using ElementMergerT = typename ElementCacheT::ElementMergerT;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	name		The view name.
		 *\param[in]	initialise	The element initialiser.
		 *\param[in]	clean		The element cleaner.
		 *\param[in]	cache		The viewed cache.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	name		The view name.
		 *\param[in]	initialise	L'initialiseur d'objet.
		 *\param[in]	clean		Le nettoyeur d'objet.
		 *\param[in]	cache		The viewed cache.
		 */
		inline CacheViewT( castor::String const & name
			, ElementCacheT & cache
			, ElementInitialiserT initialise = []( ElementPtrT ){}
			, ElementCleanerT clean = []( ElementPtrT ){} );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		inline ~CacheViewT();
		/**
		 *\~english
		 *\brief		Removes from the cache the elements created through this view.
		 *\~french
		 *\brief		Supprime du cache les éléments créés via cette vue.
		 */
		inline void clear();
		/**
		 *\~english
		 *\brief		Creates an element with the given informations.
		 *\param[in]	name	The element name.
		 *\param[in]	params	The parameters forwarded to the viewed cache.
		 *\return		The created element.
		 *\~french.=
		 *\brief		Crée un élément avec les informations données.
		 *\param[in]	name	Le nom de l'élément.
		 *\param[in]	params	Les paramètres transmis au cache vu.
		 *\return		L'élément créé.
		 */
		template< typename ... ParametersT >
		inline ElementPtrT add( ElementKeyT const & name
			, ParametersT && ... params );
		/**
		 *\~english
		 *\brief		adds an already created an element.
		 *\param[in]	name	The element name.
		 *\param[in]	element	The element.
		 *\~french
		 *\brief		Ajoute un élément déjà créé.
		 *\param[in]	name	Le nom d'élément.
		 *\param[in]	element	L'élément.
		 */
		inline bool tryAdd( ElementKeyT const & name
			, ElementPtrT element
			, bool initialise = false );
		/**
		 *\~english
		 *\brief		adds an already created an element.
		 *\param[in]	name	The element name.
		 *\param[in]	element	The element.
		 *\~french
		 *\brief		Ajoute un élément déjà créé.
		 *\param[in]	name	Le nom d'élément.
		 *\param[in]	element	L'élément.
		 */
		inline ElementPtrT add( ElementKeyT const & name
			, ElementPtrT element
			, bool initialise = false );
		/**
		 *\~english
		 *\return		\p true if the view is empty.
		 *\~french
		 *\return		\p true si la vue est vide.
		 */
		inline bool isEmpty()const;
		/**
		 *\~english
		 *\param[in]	name	The object name.
		 *\return		\p true if an element with given name exists.
		 *\~french
		 *\param[in]	name	Le nom d'objet.
		 *\return		\p true Si un élément avec le nom donné existe.
		 */
		inline bool has( ElementKeyT const & name )const;
		/**
		 *\~english
		 *\brief		Looks for an element with given name.
		 *\param[in]	name	The object name.
		 *\return		The found element, nullptr if not found.
		 *\~french
		 *\brief		Cherche un élément par son nom.
		 *\param[in]	name	Le nom d'objet.
		 *\return		L'élément trouvé, nullptr si non trouvé.
		 */
		inline ElementPtrT tryFind( ElementKeyT const & name )const;
		/**
		 *\~english
		 *\brief		Looks for an element with given name.
		 *\param[in]	name	The object name.
		 *\return		The found element, nullptr if not found.
		 *\~french
		 *\brief		Cherche un élément par son nom.
		 *\param[in]	name	Le nom d'objet.
		 *\return		L'élément trouvé, nullptr si non trouvé.
		 */
		inline ElementPtrT find( ElementKeyT const & name )const;
		/**
		 *\~english
		 *\brief		Removes an element, given a name.
		 *\param[in]	name	The element name.
		 *\~french
		 *\brief		Retire un élément à partir d'un nom.
		 *\param[in]	name	Le nom d'élément.
		 */
		inline ElementPtrT tryRemove( ElementKeyT const & name );
		/**
		 *\~english
		 *\brief		Removes an object, given a name.
		 *\param[in]	name	The object name.
		 *\~french
		 *\brief		Retire un objet à partir d'un nom.
		 *\param[in]	name	Le nom d'objet.
		 */
		inline void remove( ElementKeyT const & name );
		/**
		*\~english
		*\name Iteration.
		*\~french
		*\name Itération.
		**/
		/**@{*/
		inline auto begin()
		{
			return m_createdElements.begin();
		}

		inline auto begin()const
		{
			return m_createdElements.begin();
		}

		inline auto end()
		{
			return m_createdElements.end();
		}

		inline auto end()const
		{
			return m_createdElements.end();
		}
		/**@}*/

	private:
		ElementCacheT & m_cache;
		std::set< ElementKeyT > m_createdElements;
		ElementInitialiserT m_initialise;
		ElementCleanerT m_clean;
		std::vector< ElementPtrT > m_cleaning;
	};

	template< typename CacheT >
	using CacheViewTraitsT = typename CacheT::ElementCacheTraitsT;

	template< EventType EventT
		, typename CacheT >
	CacheViewPtrT< CacheT, EventT > makeCacheView( castor::String const & name
		, CacheT & cache
		, typename CacheViewTraitsT< CacheT >::ElementInitialiserT initialiser = {}
		, typename CacheViewTraitsT< CacheT >::ElementCleanerT cleaner = {} )
	{
		return std::make_shared< CacheViewT< CacheT, EventT > >( name
			, cache
			, std::move( initialiser )
			, std::move( cleaner ) );
	}
}

#include "Castor3D/Cache/CacheView.inl"

#endif
