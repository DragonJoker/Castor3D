#include "Boulder.hpp"

#include "Game.hpp"

namespace castortd
{
	Boulder::Boulder( float speed
		, uint32_t damage
		, c3d::SceneNode & node
		, c3d::Point3f target )
		: m_node{ node }
		, m_target{ target }
		, m_speed{ speed }
		, m_damage{ damage }
	{
	}

	void Boulder::load( float speed
		, uint32_t damage
		, c3d::Point3f const & origin
		, c3d::Point3f target )
	{
		m_damage = damage;
		m_node.get().setPosition( origin );
		m_target = target;
		m_speed = speed;
	}

	bool Boulder::accept( Game & game )
	{
		auto speed = float( game.getElapsed().count() ) * m_speed / 1000;
		c3d::Point3f position{ m_node.get().getPosition() };
		c3d::Point3f direction{ m_target - position };
		auto distanceToDst = c3d::point::length( direction );
		direction[0] *= float( speed / distanceToDst );
		direction[2] *= float( speed / distanceToDst );
		auto reachDst = distanceToDst <= speed;

		if ( !reachDst )
		{
			m_node.get().setPosition( position + direction );
		}
		else
		{
			game.areaDamage( m_target, m_damage );
			m_node.get().setPosition( c3d::Point3f{ 0, -10, 0 } );
		}

		return reachDst;
	}
}
