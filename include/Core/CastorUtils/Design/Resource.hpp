/*
See LICENSE file in root folder
*/
#ifndef ___CU_Resource_HPP___
#define ___CU_Resource_HPP___

#include "CastorUtils/Design/DesignModule.hpp"

#include "CastorUtils/Config/SmartPtr.hpp"
#include "CastorUtils/Design/Named.hpp"
#include "CastorUtils/Design/Signal.hpp"

#include <functional>

#include "CastorUtils/Config/BeginExternHeaderGuard.hpp"
#include <atomic>
#include "CastorUtils/Config/EndExternHeaderGuard.hpp"

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
		ResourceT( ResourceT && rhs )noexcept = default;
		ResourceT( ResourceT const & ) = delete;
		ResourceT & operator=( ResourceT && rhs )noexcept = default;
		ResourceT & operator=( ResourceT const & ) = delete;
		~ResourceT()noexcept override = default;

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

		ElementT & operator*()noexcept
		{
			return *this;
		}
		/**@}*/
		/**
		*name
		*	Signal.
		**/
		/**@{*/
		using OnResourceFunc = castor::Function< void( ElementT const & ) >;
		using OnResourceEvent = SignalT< OnResourceFunc >;
		using Connection = ConnectionT< OnResourceEvent >;

		OnResourceEvent onInitialising;
		OnResourceEvent onInitialised;
		OnResourceEvent onCleaning;
		OnResourceEvent onCleaned;
		/**@}*/

	private:
		std::atomic_bool m_initialised{ false };
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
		: public Named
	{
		using ElementCacheT = typename CacheT::ElementCacheT;
		using ElementContT = typename CacheT::ElementContT;
		using ElementPtrT = typename CacheT::ElementPtrT;

		explicit ResourceMergerT( String name )
			: Named{ castor::move( name ) }
		{
		}

		void operator()( ElementCacheT const & /*source*/
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

			ires.first->second = castor::move( element );
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
	inline ResourcePtrT< ResT, KeyT > makeResource( ParametersT && ... params )
	{
		return castor::makeUnique< ResourceT< ResT, KeyT > >( castor::forward< ParametersT >( params )... );
	}
}

#include "CastorUtils/Design/Resource.inl"

#endif
