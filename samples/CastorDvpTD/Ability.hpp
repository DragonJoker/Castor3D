#pragma once

#include "CastorDvpTDPrerequisites.hpp"

#include <functional>

namespace castortd
{
	template< typename T >
	struct Ability
	{
		using ValueIncrementFunction = std::function< T( T const &, uint32_t ) >;

		inline void initialise( T const & p_value
								, ValueIncrementFunction const & p_valueIncrement )
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

		inline T const & getValue()const
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

		inline void initialise( T const & p_value
								, ValueIncrementFunction const & p_valueIncrement
								, uint32_t p_cost
								, CostIncrementFunction p_costIncrement
								, uint32_t p_maxLevel = 15u )
		{
			Ability< T >::initialise( p_value, p_valueIncrement );
			m_cost = p_cost;
			m_costIncrement = p_costIncrement;
			m_maxLevel = p_maxLevel;
		}

		inline void Upgrade()
		{
			if ( CanUpgrade() )
			{
				Ability< T >::Upgrade();
				m_cost = m_costIncrement( m_cost, Ability< T >::m_level );
			}
		}

		inline bool CanUpgrade()
		{
			return Ability< T >::m_level < m_maxLevel;
		}

		inline uint32_t const & getCost()const
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
