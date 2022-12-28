#pragma once

#include "CastorDvpTDPrerequisites.hpp"

#include <functional>

namespace castortd
{
	template< typename T >
	struct Ability
	{
		using ValueIncrementFunction = std::function< T( T const &, uint32_t ) >;

		void initialise( T const & value
			, ValueIncrementFunction const & increment )
		{
			m_level = 0;
			m_value = value;
			m_increment = increment;
		}

		void upgrade()
		{
			m_value = m_increment( m_value, m_level++ );
		}

		T const & getValue()const
		{
			return m_value;
		}

	protected:
		T m_value{};
		ValueIncrementFunction m_increment;
		uint32_t m_level{ 0u };
	};

	template< typename T >
	struct PaidAbility
		: private Ability< T >
	{
		using ValueIncrementFunction = typename Ability< T >::ValueIncrementFunction;
		using CostIncrementFunction = std::function< uint32_t( uint32_t, uint32_t ) >;

		void initialise( T const & value
			, ValueIncrementFunction const & valueIncrement
			, uint32_t cost
			, CostIncrementFunction costIncrement
			, uint32_t maxLevel = 15u )
		{
			Ability< T >::initialise( value, valueIncrement );
			m_cost = cost;
			m_costIncrement = costIncrement;
			m_maxLevel = maxLevel;
		}

		void upgrade()
		{
			if ( canUpgrade() )
			{
				m_cost = m_costIncrement( m_cost, Ability< T >::m_level );
				Ability< T >::upgrade();
			}
		}

		bool canUpgrade()
		{
			return Ability< T >::m_level < m_maxLevel;
		}

		uint32_t const & getCost()const
		{
			return m_cost;
		}

		using Ability< T >::getValue;

	protected:
		uint32_t m_cost{ 0u };
		CostIncrementFunction m_costIncrement;
		uint32_t m_maxLevel{ 15u };
	};
}
