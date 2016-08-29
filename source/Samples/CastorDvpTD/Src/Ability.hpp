#pragma once

#include "CastorDvpTDPrerequisites.hpp"

#include <functional>

namespace castortd
{
	template< typename T >
	struct Ability
	{
		using ValueIncrementFunction = std::function< T( T const &, uint32_t ) >;

		inline void Initialise( T const & p_value, ValueIncrementFunction const & p_valueIncrement )
		{
			m_level = 0;
			m_value = p_value;
			m_valueIncrement = p_valueIncrement;
		}

		inline void Upgrade()
		{
			m_value = m_valueIncrement( m_value, m_level );
			++m_level;
		}

		inline T const & GetValue()const
		{
			return m_value;
		}

	protected:
		T m_value{};
		ValueIncrementFunction m_valueIncrement;
		uint32_t m_level{ 0u };
	};

	template< typename T >
	struct PaidAbility
		: private Ability< T >
	{
		using ValueIncrementFunction = typename Ability< T >::ValueIncrementFunction;
		using CostIncrementFunction = std::function< uint32_t( uint32_t, uint32_t ) >;

		inline void Initialise( T const & p_value, ValueIncrementFunction const & p_valueIncrement, uint32_t p_cost, CostIncrementFunction p_costIncrement )
		{
			Ability< T >::Initialise( p_value, p_valueIncrement );
			m_cost = p_cost;
			m_costIncrement = p_costIncrement;
		}

		inline void Upgrade()
		{
			if ( CanUpgrade() )
			{
				Ability< T >::Upgrade();
				m_cost = m_costIncrement( m_cost, m_level );
			}
		}

		inline bool CanUpgrade()
		{
			return m_level < 15;
		}

		inline uint32_t const & GetCost()const
		{
			return m_cost;
		}

		using Ability< T >::GetValue;

	protected:
		uint32_t m_cost{ 0u };
		CostIncrementFunction m_costIncrement;
	};
}
