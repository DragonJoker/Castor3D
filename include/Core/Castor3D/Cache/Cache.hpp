/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Cache_H___
#define ___C3D_Cache_H___

#include "Castor3D/Cache/CacheBase.hpp"

namespace castor3d
{
	template< typename ElementType, typename KeyType >
	class Cache
		: public CacheBase< ElementType, KeyType >
	{
	public:
		using MyCacheType = CacheBase< ElementType, KeyType >;
		using MyCacheTraits = typename MyCacheType::MyCacheTraits;
		using Element = typename MyCacheType::Element;
		using Key = typename MyCacheType::Key;
		using Collection = typename MyCacheType::Collection;
		using LockType = typename MyCacheType::LockType;
		using ElementPtr = typename MyCacheType::ElementPtr;
		using Producer = typename MyCacheType::Producer;
		using Initialiser = typename MyCacheType::Initialiser;
		using Cleaner = typename MyCacheType::Cleaner;
		using Merger = typename MyCacheType::Merger;
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine		The engine.
		 *\param[in]	produce		The element producer.
		 *\param[in]	initialise	The element initialiser.
		 *\param[in]	clean		The element cleaner.
		 *\param[in]	merge		The element collection merger.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	produce		Le créateur d'objet.
		 *\param[in]	initialise	L'initialiseur d'objet.
		 *\param[in]	clean		Le nettoyeur d'objet.
		 *\param[in]	merge		Le fusionneur de collection d'objets.
		 */
		Cache( Engine & engine
			, Producer && produce
			, Initialiser && initialise = Initialiser{}
			, Cleaner && clean = Cleaner{}
			, Merger && merge = Merger{} )
			: MyCacheType( engine
				, std::move( produce )
				, std::move( initialise )
				, std::move( clean )
				, std::move( merge ) )
		{
		}
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~Cache()
		{
		}
	};
	/**
	 *\~english
	 *\brief		Creates a cache.
	 *\param[in]	engine		The engine.
	 *\param[in]	produce		The element producer.
	 *\param[in]	initialise	The element initialiser.
	 *\param[in]	clean		The element cleaner.
	 *\param[in]	merge		The element collection merger.
	 *\~french
	 *\brief		Crée un cache.
	 *\param[in]	engine		Le moteur.
	 *\param[in]	produce		Le créateur d'objet.
	 *\param[in]	initialise	L'initialiseur d'objet.
	 *\param[in]	clean		Le nettoyeur d'objet.
	 *\param[in]	merge		Le fusionneur de collection d'objets.
	 */
	template< typename ElementType, typename KeyType >
	inline std::unique_ptr< Cache< ElementType, KeyType > >
	makeCache( Engine & engine
		, typename CacheTraits< ElementType, KeyType >::Producer && produce
		, ElementInitialiser< ElementType > && initialise = []( std::shared_ptr< ElementType > ){}
		, ElementCleaner< ElementType > && clean = []( std::shared_ptr< ElementType > ){}
		, typename CacheTraits< ElementType, KeyType >::Merger && merge = []( CacheBase< ElementType, KeyType > const &
			, castor::Collection< ElementType, KeyType > &
			, std::shared_ptr< ElementType > ){} )
	{
		return std::make_unique< Cache< ElementType, KeyType > >( engine
			, std::move( produce )
			, std::move( initialise )
			, std::move( clean )
			, std::move( merge ) );
	}
}

#endif
