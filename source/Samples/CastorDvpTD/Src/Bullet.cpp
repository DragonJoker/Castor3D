#include "Bullet.hpp"

#include "Game.hpp"

using namespace Castor;
using namespace Castor3D;

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

	void Bullet::Load( float p_speed, uint32_t p_damage, Castor::Point3r const & p_origin, Enemy & p_enemy )
	{
		m_damage = p_damage;
		m_node.get().SetPosition( p_origin );
		m_target = p_enemy;
		m_speed = p_speed;
	}

	bool Bullet::Accept( Game & p_game )
	{
		bool l_reachDst{ true };

		if ( m_target.get().IsAlive() )
		{
			auto l_speed = p_game.GetElapsed().count() * m_speed / 1000;
			Point3r l_result = m_target.get().GetNode().GetPosition();
			Point3r l_position{ m_node.get().GetPosition() };
			Point3r l_direction{ l_result - l_position };
			auto l_distanceToDst{ point::distance( l_direction ) };
			l_direction[0] *= l_speed / l_distanceToDst;
			l_direction[2] *= l_speed / l_distanceToDst;
			l_reachDst = l_distanceToDst <= l_speed;

			if ( !l_reachDst )
			{
				l_result = l_position + l_direction;
				m_node.get().SetPosition( l_result );
			}
			else
			{
				m_target.get().TakeDamage( m_damage );
			}
		}

		if ( l_reachDst )
		{
			m_node.get().SetPosition( Point3r{ 0, -10, 0 } );
		}

		return l_reachDst;
	}
}
