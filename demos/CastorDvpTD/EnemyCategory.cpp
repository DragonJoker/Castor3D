#include "EnemyCategory.hpp"

namespace castortd
{
	BaseEnemy::BaseEnemy()
		: Enemy::Category{}
	{
		m_speed = 40.0f;
		m_flying = false;

		m_life.initialise( 1u, []( uint32_t value, uint32_t level )
		{
			return value + std::max( level, uint32_t( value * 5.0 / 100.0 ) );
		} );

		m_bounty.initialise( 9u, []( uint32_t value, uint32_t level )
		{
			return value + std::max( 2u, ( value * 4 ) / 100 );
		} );
	}
}
