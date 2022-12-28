#include "Bullet.hpp"

#include "Game.hpp"

namespace castortd
{
	Bullet::Bullet( float speed, uint32_t damage, castor3d::SceneNode & node, Enemy & enemy )
		: m_node{ node }
		, m_target{ enemy }
		, m_speed{ speed }
		, m_damage{ damage }
	{
	}

	void Bullet::load( float speed, uint32_t damage, castor::Point3f const & origin, Enemy & enemy )
	{
		m_damage = damage;
		m_node.get().setPosition( origin );
		m_target = enemy;
		m_speed = speed;
	}

	bool Bullet::accept( Game & game )
	{
		bool reachDst{ true };

		if ( m_target.get().isAlive() )
		{
			auto speed = float( game.getElapsed().count() ) * m_speed / 1000;
			castor::Point3f result = m_target.get().getNode().getPosition();
			castor::Point3f position{ m_node.get().getPosition() };
			castor::Point3f direction{ result - position };
			auto distanceToDst = castor::point::length( direction );
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
				m_target.get().takeDamage( m_damage );
			}
		}

		if ( reachDst )
		{
			m_node.get().setPosition( castor::Point3f{ 0, -10, 0 } );
		}

		return reachDst;
	}
}
