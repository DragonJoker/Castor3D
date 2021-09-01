/*
See LICENSE file in root folder
*/
#ifndef ___CU_ResourceCache_H___
#define ___CU_ResourceCache_H___

#include "CastorUtils/Design/ResourceCacheBase.hpp"
#include "CastorUtils/Log/LogModule.hpp"

#include <memory>

namespace castor
{
	template< typename ResT, typename KeyT, typename TraitsT >
	class ResourceCacheT final
		: public ResourceCacheBaseT< ResT, KeyT, TraitsT >
	{
	public:
		using ElementT = ResT;
		using ElementKeyT = KeyT;
		using ElementCacheT = ResourceCacheBaseT< ElementT, ElementKeyT, TraitsT >;
		using ElementCacheTraitsT = typename ElementCacheT::ElementCacheTraitsT;
		using ElementPtrT = typename ElementCacheT::ElementPtrT;
		using ElementContT = typename ElementCacheT::ElementContT;
		using ElementProducerT = typename ElementCacheT::ElementProducerT;
		using ElementInitialiserT = typename ElementCacheT::ElementInitialiserT;
		using ElementCleanerT = typename ElementCacheT::ElementCleanerT;
		using ElementMergerT = typename ElementCacheT::ElementMergerT;
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	clean		Le nettoyeur d'objet.
		 *\param[in]	merge		Le fusionneur de collection d'objets.
		 */
		ResourceCacheT( LoggerInstance & logger
			, ElementProducerT produce
			, ElementInitialiserT initialise = ElementInitialiserT{}
			, ElementCleanerT clean = ElementCleanerT{}
			, ElementMergerT merge = ElementMergerT{} )
			: ElementCacheT{ logger
				, std::move( produce )
				, std::move( initialise )
				, std::move( clean )
				, std::move( merge ) }
		{
		}
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~ResourceCacheT()
		{
		}
	};
	/**
	 *\~english
	 *\brief		Creates a cache.
	 *\param[in]	parameters	The cache ctor parameters.
	 *\~french
	 *\brief		Crée un cache.
	 *\param[in]	parameters	Les paramètres de construction du cache.
	 */
	template< typename ResT, typename KeyT, typename ... ParametersT >
	ResourceCachePtrT< ResT, KeyT > makeCache( ParametersT && ... parameters )
	{
		return std::make_shared< ResourceCacheT< ResT, KeyT > >( std::forward< ParametersT >( parameters )... );
	}
}

#endif
