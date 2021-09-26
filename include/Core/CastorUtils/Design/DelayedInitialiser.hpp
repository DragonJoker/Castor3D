/*
See LICENSE file in root folder
*/
#ifndef ___CU_DelayedInitialiser_H___
#define ___CU_DelayedInitialiser_H___

#include "DesignModule.hpp"

#pragma warning( push )
#pragma warning( disable:4365 )
#include <atomic>
#include <memory>
#pragma warning( pop )

namespace castor
{
#if defined( CU_NoDelayedInit )
	template< typename TypeT >
	class DelayedInitialiserT
	{
	public:
		template< typename RhsT >
		friend class DelayedInitialiserT;

		explicit DelayedInitialiserT( std::unique_ptr< TypeT > ptr = nullptr )
			: m_ptr{ std::move( ptr ) }
		{
		}

		template< typename RhsT, typename ... ParamsT >
		static DelayedInitialiserT make( ParamsT && ... params )
		{
			static_assert( std::is_same_v< TypeT, RhsT > || std::is_base_of_v< TypeT, RhsT > );
			return DelayedInitialiserT{ std::make_unique< RhsT >( std::forward< ParamsT >( params )... ) };
		}

		DelayedInitialiserT( DelayedInitialiserT const & rhs ) = delete;
		DelayedInitialiserT & operator=( DelayedInitialiserT const & rhs ) = delete;

		DelayedInitialiserT( DelayedInitialiserT && rhs )
			: m_ptr{ std::move( rhs.m_ptr ) }
		{
		}

		template< typename RhsT >
		DelayedInitialiserT & operator=( DelayedInitialiserT< RhsT > && rhs )
		{
			static_assert( std::is_same_v< TypeT, RhsT > || std::is_base_of_v< TypeT, RhsT > );
			m_ptr = std::move( rhs.m_ptr );
			return *this;
		}

		DelayedInitialiserT & operator=( std::unique_ptr< TypeT > ptr )
		{
			m_ptr = std::move( ptr );
			return *this;
		}

		TypeT & operator*()
		{
			return *m_ptr;
		}

		TypeT const & operator*()const
		{
			return *m_ptr;
		}

		TypeT * operator->()
		{
			return m_ptr.get();
		}

		TypeT const * operator->()const
		{
			return m_ptr.get();
		}

		TypeT * get()const
		{
			return m_ptr.get();
		}

		TypeT * raw()const
		{
			return m_ptr.get();
		}

		void reset()
		{
			m_ptr.reset();
		}

		operator bool()const
		{
			return bool( m_ptr );
		}

		template< typename ... ParamsT >
		void cleanup( ParamsT && ... params )
		{
			CU_Require( m_ptr );
			m_ptr->cleanup( std::forward< ParamsT >( params )... );
		}

		template< typename ... ParamsT >
		void initialise( ParamsT && ... params )
		{
			CU_Require( m_ptr );
			m_ptr->initialise( std::forward< ParamsT >( params )... );
		}

	private:
		std::unique_ptr< TypeT > m_ptr;
	};
#else
	template< typename TypeT >
	class DelayedInitialiserT
	{
	public:
		template< typename RhsT >
		friend class DelayedInitialiserT;

		explicit DelayedInitialiserT( std::unique_ptr< TypeT > ptr = nullptr )
			: m_ptr{ std::move( ptr ) }
		{
		}

		template< typename RhsT, typename ... ParamsT >
		static DelayedInitialiserT make( ParamsT && ... params )
		{
			static_assert( std::is_same_v< TypeT, RhsT > || std::is_base_of_v< TypeT, RhsT > );
			return DelayedInitialiserT{ std::make_unique< RhsT >( std::forward< ParamsT && >( params )... ) };
		}

		DelayedInitialiserT( DelayedInitialiserT const & rhs ) = delete;
		DelayedInitialiserT & operator=( DelayedInitialiserT const & rhs ) = delete;

		DelayedInitialiserT( DelayedInitialiserT && rhs )
			: m_ptr{ std::move( rhs.m_ptr ) }
			, m_initialised{ rhs.m_initialised.exchange( false ) }
			, m_initialiseExecuted{ rhs.m_initialiseExecuted.exchange( false ) }
			, m_initialise{ std::move( rhs.m_initialise ) }
		{
		}

		template< typename RhsT >
		DelayedInitialiserT & operator=( DelayedInitialiserT< RhsT > && rhs )
		{
			static_assert( std::is_same_v< TypeT, RhsT > || std::is_base_of_v< TypeT, RhsT > );
			m_ptr = std::move( rhs.m_ptr );
			m_initialised = rhs.m_initialised.exchange( false );
			m_initialiseExecuted = rhs.m_initialiseExecuted.exchange( false );
			m_initialise = std::move( rhs.m_initialise );
			return *this;
		}

		DelayedInitialiserT & operator=( std::unique_ptr< TypeT > ptr )
		{
			m_ptr = std::move( ptr );

			if ( m_initialised.exchange( false ) )
			{
				m_initialise = [](){};
				m_initialiseExecuted = false;
			}

			return *this;
		}

		TypeT & operator*()
		{
			checkInitialise();
			return *m_ptr;
		}

		TypeT const & operator*()const
		{
			checkInitialise();
			return *m_ptr;
		}
			
		TypeT * operator->()
		{
			checkInitialise();
			return m_ptr.get();
		}

		TypeT const * operator->()const
		{
			checkInitialise();
			return m_ptr.get();
		}

		TypeT * get()const
		{
			checkInitialise();
			return m_ptr.get();
		}

		TypeT * raw()const
		{
			return m_ptr.get();
		}

		void reset()
		{
			m_ptr.reset();
		}

		operator bool()const
		{
			return bool( m_ptr );
		}

		template< typename ... ParamsT >
		void cleanup( ParamsT && ... params )
		{
			CU_Require( m_ptr );
			if ( m_initialised.exchange( false ) )
			{
				if ( m_initialiseExecuted.exchange( false ) )
				{
					m_ptr->cleanup( std::forward< ParamsT >( params )... );
				}

				m_initialise = [](){};
			}
		}

		template< typename ... ParamsT >
		void initialise( ParamsT && ... params )
		{
			CU_Require( m_ptr );
			if ( !m_initialised.exchange( true ) )
			{
				m_initialise = [this, &params...]()
				{
					m_ptr->initialise( std::forward< ParamsT >( params )... );
					m_initialiseExecuted = true;
					m_initialise = []()
					{};
				};
			}
		}

	private:
		void checkInitialise()const
		{
			if ( m_initialised )
			{
				m_initialise();
			}
		}

	private:
		std::unique_ptr< TypeT > m_ptr;
		std::atomic_bool m_initialised{ false };
		std::atomic_bool m_initialiseExecuted{ false };
		std::function< void() > m_initialise{ [](){} };
	};
#endif

	template< typename LhsT, typename RhsT, typename ... ParamsT >
	inline DelayedInitialiserT< LhsT > makeDerivedDelayedInitialiser( ParamsT && ... params )
	{
		return DelayedInitialiserT< LhsT >::template make< RhsT >( std::forward< ParamsT >( params )... );
	}

	template< typename LhsT, typename ... ParamsT >
	inline DelayedInitialiserT< LhsT > makeDelayedInitialiser( ParamsT && ... params )
	{
		return DelayedInitialiserT< LhsT >::template make< LhsT >( std::forward< ParamsT >( params )... );
	}
}

#endif
