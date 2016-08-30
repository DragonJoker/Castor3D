#include "EnemyCategory.hpp"

using namespace Castor;
using namespace Castor3D;

namespace castortd
{
	BaseEnemy::BaseEnemy()
		: Enemy::Category{}
	{
		m_speed = 24.0f;
		m_flying = false;

		m_life.Initialise( 1u, []( uint32_t p_value, uint32_t p_level )
		{
			return p_value + std::max( p_level, uint32_t( p_value * 5.0 / 100.0 ) );
		} );

		m_bounty.Initialise( 9u, []( uint32_t p_value, uint32_t p_level )
		{
			return p_value + 2u;
		} );
	}
}
