#include "TowerCategory.hpp"

namespace castortd
{
	LongRangeTower::LongRangeTower()
		: Category{ Tower::Category::Kind::eLongRange
			, cuT( "armature_short_range.1|attack" ) }
	{
		auto costIncrement = []( uint32_t inc
			, uint32_t value
			, uint32_t level )
		{
			return value + inc + inc * ( level / 5 );
		};
		auto damageIncrement = []( uint32_t inc
			, uint32_t value
			, uint32_t level )
		{
			return value + inc;
		};
		auto floatIncrement = []( float inc
			, float value
			, uint32_t level )
		{
			return value + inc;
		};
		auto speedIncrement = []( float inc
			, float value
			, uint32_t level )
		{
			return value * inc;
		};
		m_damage.initialise( 5u
			, std::bind( damageIncrement, 9u, std::placeholders::_1, std::placeholders::_2 )
			, 400u
			, std::bind( costIncrement, 30u, std::placeholders::_1, std::placeholders::_2 ) );

		m_speed.initialise( 1.0f
			, std::bind( speedIncrement, float( sqrt( 2.0f ) ), std::placeholders::_1, std::placeholders::_2 )
			, 200u
			, std::bind( costIncrement, 20u, std::placeholders::_1, std::placeholders::_2 ) );

		m_range.initialise( 100.0f
			, std::bind( floatIncrement, 20.0f, std::placeholders::_1, std::placeholders::_2 )
			, 150u
			, std::bind( costIncrement, 10u, std::placeholders::_1, std::placeholders::_2 ) );

		m_bulletSpeed = 96.0f;
		m_towerCost = 250u;
	}

	ShortRangeTower::ShortRangeTower()
		: Category{ Tower::Category::Kind::eShortRange
			, cuT( "armature_short_range.1|attack" ) }
	{
		auto costIncrement = []( uint32_t inc
			, uint32_t value
			, uint32_t level )
		{
			return value + inc + inc * ( level / 5 );
		};
		auto uintIncrement = []( uint32_t inc
			, uint32_t value
			, uint32_t level )
		{
			return value + inc;
		};
		auto floatIncrement = []( float inc
			, float value
			, uint32_t level )
		{
			return value + inc;
		};
		auto speedIncrement = []( float inc
			, float value
			, uint32_t level )
		{
			return value * inc;
		};
		m_damage.initialise( 3u
			, std::bind( uintIncrement, 5u, std::placeholders::_1, std::placeholders::_2 )
			, 400u
			, std::bind( costIncrement, 30u, std::placeholders::_1, std::placeholders::_2 ) );

		m_speed.initialise( 1.0f
			, std::bind( speedIncrement, float( sqrt( 2.0f ) ), std::placeholders::_1, std::placeholders::_2 )
			, 150u
			, std::bind( costIncrement, 10u, std::placeholders::_1, std::placeholders::_2 ) );

		m_range.initialise( 40.0f
			, std::bind( floatIncrement, 4.0f, std::placeholders::_1, std::placeholders::_2 )
			, 200u
			, std::bind( costIncrement, 20u, std::placeholders::_1, std::placeholders::_2 ) );

		m_bulletSpeed = 120.0f;
		m_towerCost = 170u;
	}
}
