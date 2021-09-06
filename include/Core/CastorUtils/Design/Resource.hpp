/*
See LICENSE file in root folder
*/
#ifndef ___CU_Resource_HPP___
#define ___CU_Resource_HPP___

#include "CastorUtils/Design/DesignModule.hpp"

#include "CastorUtils/Config/SmartPtr.hpp"
#include "CastorUtils/Design/Named.hpp"
#include "CastorUtils/Design/Signal.hpp"

#include <atomic>
#include <functional>

namespace castor
{
	template< typename ResT, typename KeyT >
	class ResourceT final
		: public ResT
	{
	public:
		using ElementT = ResT;
		using ElementKeyT = KeyT;

	public:
		/**
		*name
		*	Construction/Desctruction.
		**/
		/**@{*/
		ResourceT( ResourceT && rhs ) = default;
		ResourceT( ResourceT const & ) = delete;
		ResourceT & operator=( ResourceT && rhs ) = default;
		ResourceT & operator=( ResourceT const & ) = delete;
		~ResourceT() = default;

		template< typename ... ParametersT >
		explicit ResourceT( ParametersT && ... params );
		explicit ResourceT( ElementT && rhs );
		ResourceT & operator=( ElementT && rhs );
		/**@}*/
		/**
		*\~english
		*name
		*	Initialisation/Cleanup.
		*\~french
		*name
		*	Initialisation/Nettoyage.
		**/
		/**@{*/
		template< typename ... ParametersT >
		void initialise( ParametersT && ... params );
		template< typename ... ParametersT >
		void cleanup( ParametersT && ... params );
		template< typename ... ParametersT, typename ... ParametersU >
		void reinitialise( ParametersT && ... paramsT
			, ParametersU && ... paramsU );
		/**@}*/
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		**/
		/**@{*/
		bool isInitialised()const noexcept
		{
			return m_initialised;
		}

		ElementT & operator*()
		{
			return *this;
		}
		/**@}*/
		/**
		*name
		*	Signal.
		**/
		/**@{*/
		using OnResourceFunc = std::function< void( ElementT const & ) >;
		using OnResourceEvent = Signal< OnResourceFunc >;
		using Connection = castor::Connection< OnResourceEvent >;

		OnResourceEvent onInitialising;
		OnResourceEvent onInitialised;
		OnResourceEvent onCleaning;
		OnResourceEvent onCleaned;
		/**@}*/

	private:
		std::atomic_bool m_initialised;
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
	struct ResourceInitialiserT
	{
		using ElementT = typename CacheT::ElementT;

		void operator()( ElementT & res )const
		{
			res.initialise();
		}
	};

	template< typename CacheT >
	struct ResourceCleanerT
	{
		using ElementT = typename CacheT::ElementT;

		void operator()( ElementT & res )const
		{
			res.cleanup();
		}
	};

	template< typename CacheT >
	struct ResourceMergerT
		: public castor::Named
	{
		using ElementCacheT = typename CacheT::ElementCacheT;
		using ElementContT = typename CacheT::ElementContT;
		using ElementPtrT = typename CacheT::ElementPtrT;

		ResourceMergerT( castor::String const & name )
			: castor::Named{ name }
		{
		}

		void operator()( ElementCacheT const & source
			, ElementContT & destination
			, ElementPtrT element )const
		{
			auto name = element->getName();
			auto ires = destination.emplace( name, ElementPtrT{} );

			while ( !ires.second )
			{
				name = getName() + cuT( "_" ) + name;
				ires = destination.emplace( name, ElementPtrT{} );
			}

			ires.first->second = std::move( element );
			ires.first->second->rename( name );
		}
	};
	/**@}*/
	/**
	*\~english
	*	Resource creation helper.
	*\~french
	*	Fonction d'aide à la création de ressource.
	**/
	template< typename ResT
		, typename KeyT
		, typename ... ParametersT >
	inline ResourceSPtrT< ResT, KeyT > makeResource( ParametersT && ... params )
	{
		return std::make_shared< ResourceT< ResT, KeyT > >( std::forward< ParametersT >( params )... );
	}
}

#include "CastorUtils/Design/Resource.inl"

#endif
