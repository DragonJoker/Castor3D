#include "TowerCategory.hpp"

namespace castortd
{
	namespace
	{
		template< uint32_t IncT >
		uint32_t costIncrement( uint32_t value
			, uint32_t level )
		{
			return value + IncT + IncT * ( level / 5 );
		}

		template< uint32_t IncT >
		uint32_t uintIncrement( uint32_t value
			, [[maybe_unused]] uint32_t level )
		{
			return value + IncT;
		}

		template< float IncT >
		float floatIncrement( float value
			, [[maybe_unused]] uint32_t level )
		{
			return value + IncT;
		}

		template< float IncT >
		float speedIncrement( float value
			, [[maybe_unused]] uint32_t level )
		{
			return value * IncT;
		}
	}

	LongRangeTower::LongRangeTower()
		: Category{ Tower::Category::Kind::eLongRange
			, cuT( "armature_short_range.1|attack" ) }
	{
		m_damage.initialise( 5u, uintIncrement< 9u >
			, 400u, costIncrement< 30u > );

		m_speed.initialise( 1.0f, speedIncrement< 1.4142f >
			, 200u, costIncrement< 20u > );

		m_range.initialise( 100.0f, floatIncrement< 20.0f >
			, 150u, costIncrement< 10u > );

		m_bulletSpeed = 96.0f;
		m_towerCost = 250u;
	}

	ShortRangeTower::ShortRangeTower()
		: Category{ Tower::Category::Kind::eShortRange
			, cuT( "armature_short_range.1|attack" ) }
	{
		m_damage.initialise( 3u, uintIncrement< 5u >
			, 400u, costIncrement< 30u > );

		m_speed.initialise( 1.0f, speedIncrement< 1.4142f >
			, 150u, costIncrement< 10u > );

		m_range.initialise( 40.0f, floatIncrement< 4.0f >
			, 200u, costIncrement< 20u > );

		m_bulletSpeed = 120.0f;
		m_towerCost = 170u;
	}
}
