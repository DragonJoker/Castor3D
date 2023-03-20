/*
See LICENSE file in root folder
*/
#ifndef ___C3D_CacheView_H___
#define ___C3D_CacheView_H___

#include "CacheModule.hpp"

#include "Castor3D/Event/Frame/FrameEventModule.hpp"

#include <CastorUtils/Design/Named.hpp>
#include <CastorUtils/Design/ResourceCache.hpp>

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
		using ElementObsT = typename ElementCacheT::ElementObsT;
		using ElementContT = typename ElementCacheT::ElementContT;
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
			, ElementInitialiserT initialise = []( ElementObsT const & ){}
			, ElementCleanerT clean = []( ElementObsT const & ){} );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~CacheViewT() = default;
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
		inline ElementObsT add( ElementKeyT const & name
			, ParametersT && ... params );
		/**
		 *\~english
		 *\brief		Creates or retrieves an element with the given informations.
		 *\param[in]	name		The element name.
		 *\param[in]	initialise	Tells if the element is to be initialised after creation.
		 *\param[out]	created		Receives the created element if ti was created.
		 *\param[in]	params		The parameters forwarded to the viewed cache.
		 *\return		The created or existing element.
		 *\~french.=
		 *\brief		Crée ou récupère un élément avec les informations données.
		 *\param[in]	name		Le nom de l'élément.
		 *\param[out]	created		Reçoit l'élement créé s'il l'a été.
		 *\param[in]	initialise	Dit si l'élément doit être initialisé après sa création.
		 *\param[in]	params		Les paramètres transmis au cache vu.
		 *\return		L'élément créé ou existant.
		 */
		template< typename ... ParametersT >
		inline ElementObsT tryAdd( ElementKeyT const & name
			, bool initialise
			, ElementObsT & created
			, ParametersT && ... params );
		/**
		 *\~english
		 *\brief		adds an already created an element.
		 *\param[in]	name		The element name.
		 *\param[in]	element		The element.
		 *\param[in]	initialise	Tells if the element is to be initialised after creation.
		 *\~french
		 *\brief		Ajoute un élément déjà créé.
		 *\param[in]	name		Le nom d'élément.
		 *\param[in]	element		L'élément.
		 *\param[in]	initialise	Dit si l'élément doit être initialisé après sa création.
		 */
		inline bool tryAdd( ElementKeyT const & name
			, ElementPtrT & element
			, bool initialise = false );
		/**
		 *\~english
		 *\brief		adds an already created an element.
		 *\param[in]	name		The element name.
		 *\param[in]	element		The element.
		 *\param[in]	initialise	Tells if the element is to be initialised after creation.
		 *\~french
		 *\brief		Ajoute un élément déjà créé.
		 *\param[in]	name		Le nom d'élément.
		 *\param[in]	element		L'élément.
		 *\param[in]	initialise	Dit si l'élément doit être initialisé après sa création.
		 */
		inline ElementObsT add( ElementKeyT const & name
			, ElementPtrT & element
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
		inline ElementObsT tryFind( ElementKeyT const & name )const;
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
		inline ElementObsT find( ElementKeyT const & name )const;
		/**
		 *\~english
		 *\brief		Removes an element, given a name.
		 *\param[in]	name	The element name.
		 *\~french
		 *\brief		Retire un élément à partir d'un nom.
		 *\param[in]	name	Le nom d'élément.
		 */
		inline ElementObsT tryRemove( ElementKeyT const & name );
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

		auto & getCache()const
		{
			return m_cache;
		}

	private:
		ElementCacheT & m_cache;
		mutable std::mutex m_elementsMutex;
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
