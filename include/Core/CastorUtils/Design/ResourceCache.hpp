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
		using ElementCacheTraitsT = TraitsT;
		using ElementCacheT = ResourceCacheBaseT< ElementT, ElementKeyT, ElementCacheTraitsT >;
		using ElementPtrT = typename ElementCacheT::ElementPtrT;
		using ElementObsT = typename ElementCacheT::ElementObsT;
		using ElementContT = typename ElementCacheT::ElementContT;
		using ElementInitialiserT = typename ElementCacheT::ElementInitialiserT;
		using ElementCleanerT = typename ElementCacheT::ElementCleanerT;
		using ElementMergerT = typename ElementCacheT::ElementMergerT;
		/**
		*name
		*	Construction/Desctruction.
		**/
		/**@{*/
		explicit ResourceCacheT( LoggerInstance & logger
			, ElementInitialiserT initialise = ElementInitialiserT{}
			, ElementCleanerT clean = ElementCleanerT{}
			, ElementMergerT merge = ElementMergerT{} )
			: ElementCacheT{ logger
				, std::move( initialise )
				, std::move( clean )
				, std::move( merge ) }
		{
		}

		~ResourceCacheT() = default;
		/**@}*/
	};
	/**
	*\~english
	*name
	*	Cache functors.
	*\~french
	*name
	*	Foncteurs de cache.
	**/
	/**@{*/
	template< typename CacheT >
	struct DummyFunctorT
	{
		using ElementT = typename CacheT::ElementT;

		void operator()( ElementT & res )const
		{
		}
	};
	/**@}*/
	/**
	 *\~english
	 *\brief		Creates a cache.
	 *\param[in]	parameters	The cache ctor parameters.
	 *\~french
	 *\brief		Crée un cache.
	 *\param[in]	parameters	Les paramètres de construction du cache.
	 */
	template< typename ResT
		, typename KeyT
		, typename TraitsT
		, typename ... ParametersT >
	ResourceCachePtrT< ResT, KeyT, TraitsT > makeCache( ParametersT && ... parameters )
	{
		return std::make_shared< ResourceCacheT< ResT, KeyT, TraitsT > >( std::forward< ParametersT >( parameters )... );
	}
}

#endif
