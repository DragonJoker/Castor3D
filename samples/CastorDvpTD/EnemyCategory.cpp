#include "CastorDvpTD/EnemyCategory.hpp"

using namespace castor;
using namespace castor3d;

namespace castortd
{
	BaseEnemy::BaseEnemy()
		: Enemy::Category{}
	{
		m_speed = 24.0f;
		m_flying = false;

		m_life.initialise( 1u, []( uint32_t p_value, uint32_t p_level )
		{
			return p_value + std::max( p_level, uint32_t( p_value * 5.0 / 100.0 ) );
		} );

		m_bounty.initialise( 9u, []( uint32_t p_value, uint32_t p_level )
		{
			return p_value + std::max( 2u, ( p_value * 4 ) / 100 );
		} );
	}
}
