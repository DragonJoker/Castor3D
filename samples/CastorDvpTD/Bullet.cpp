#include "Bullet.hpp"

#include "Game.hpp"

using namespace castor;
using namespace castor3d;

namespace castortd
{
	Bullet::Bullet( float p_speed, uint32_t p_damage, SceneNode & p_node, Enemy & p_enemy )
		: m_node{ p_node }
		, m_target{ p_enemy }
		, m_speed{ p_speed }
		, m_damage{ p_damage }
	{
	}

	void Bullet::load( float p_speed, uint32_t p_damage, castor::Point3f const & p_origin, Enemy & p_enemy )
	{
		m_damage = p_damage;
		m_node.get().setPosition( p_origin );
		m_target = p_enemy;
		m_speed = p_speed;
	}

	bool Bullet::accept( Game & p_game )
	{
		bool reachDst{ true };

		if ( m_target.get().IsAlive() )
		{
			auto speed = float( p_game.getElapsed().count() ) * m_speed / 1000;
			castor::Point3f result = m_target.get().getNode().getPosition();
			castor::Point3f position{ m_node.get().getPosition() };
			castor::Point3f direction{ result - position };
			auto distanceToDst = point::length( direction );
			direction[0] *= float( speed / distanceToDst );
			direction[2] *= float( speed / distanceToDst );
			reachDst = distanceToDst <= speed;

			if ( !reachDst )
			{
				result = position + direction;
				m_node.get().setPosition( result );
			}
			else
			{
				m_target.get().TakeDamage( m_damage );
			}
		}

		if ( reachDst )
		{
			m_node.get().setPosition( castor::Point3f{ 0, -10, 0 } );
		}

		return reachDst;
	}
}
