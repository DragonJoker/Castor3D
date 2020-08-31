/*
See LICENSE file in root folder
*/
#ifndef ___CU_DelayedInitialiser_H___
#define ___CU_DelayedInitialiser_H___

#include "DesignModule.hpp"

#include <atomic>
#include <memory>

namespace castor
{
	template< typename TypeT >
	class DelayedInitialiserT
	{
	public:
		template< typename RhsT >
		friend class DelayedInitialiserT;

		DelayedInitialiserT( std::unique_ptr< TypeT > ptr = nullptr )
			: m_ptr{ std::move( ptr ) }
		{
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

		void cleanup()
		{
			CU_Require( m_ptr );
			if ( m_initialised.exchange( false ) )
			{
				if ( m_initialiseExecuted.exchange( false ) )
				{
					m_ptr->cleanup();
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
	template< typename TypeT >
	DelayedInitialiserT< TypeT > makeDelayedInitialiser( std::unique_ptr< TypeT > ptr = nullptr )
	{
		return DelayedInitialiserT< TypeT >( std::move( ptr ) );
	}
}

#endif
