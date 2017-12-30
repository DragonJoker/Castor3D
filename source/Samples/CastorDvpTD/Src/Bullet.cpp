#include "Bullet.hpp"

#include "Game.hpp"

using namespace castor;
using namespace castor3d;

namespace castortd
{
	Bullet::Bullet( float p_speed, uint32_t p_damage, SceneNode & p_node, Enemy & p_enemy )
		: m_node{ p_node }
		, m_target{ p_enemy }
		, m_damage{ p_damage }
		, m_speed{ p_speed }
	{
	}

	Bullet::~Bullet()
	{
	}

	void Bullet::load( float p_speed, uint32_t p_damage, castor::Point3r const & p_origin, Enemy & p_enemy )
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
			auto speed = p_game.getElapsed().count() * m_speed / 1000;
			Point3r result = m_target.get().getNode().getPosition();
			Point3r position{ m_node.get().getPosition() };
			Point3r direction{ result - position };
			auto distanceToDst = point::length( direction );
			direction[0] *= speed / distanceToDst;
			direction[2] *= speed / distanceToDst;
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
			m_node.get().setPosition( Point3r{ 0, -10, 0 } );
		}

		return reachDst;
	}
}
