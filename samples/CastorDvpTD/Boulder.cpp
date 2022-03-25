#include "Boulder.hpp"

#include "Game.hpp"

namespace castortd
{
	Boulder::Boulder( float speed
		, uint32_t damage
		, castor3d::SceneNode & node
		, castor::Point3f target )
		: m_node{ node }
		, m_target{ target }
		, m_speed{ speed }
		, m_damage{ damage }
	{
	}

	void Boulder::load( float speed
		, uint32_t damage
		, castor::Point3f const & origin
		, castor::Point3f target )
	{
		m_damage = damage;
		m_node.get().setPosition( origin );
		m_target = target;
		m_speed = speed;
	}

	bool Boulder::accept( Game & game )
	{
		auto speed = float( game.getElapsed().count() ) * m_speed / 1000;
		castor::Point3f position{ m_node.get().getPosition() };
		castor::Point3f direction{ m_target - position };
		auto distanceToDst = castor::point::length( direction );
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
			m_node.get().setPosition( castor::Point3f{ 0, -10, 0 } );
		}

		return reachDst;
	}
}
