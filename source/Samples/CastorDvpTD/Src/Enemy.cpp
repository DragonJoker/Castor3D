#include "Enemy.hpp"

#include "Game.hpp"

using namespace Castor;
using namespace Castor3D;

namespace castortd
{
	Enemy::Enemy( SceneNode & p_node, Game const & p_game, Path const & p_path, Category const & p_category )
		: m_node{ p_node }
		, m_path{ p_path }
		, m_category{ p_category }
		, m_life{ p_category.m_life.GetValue() }
		, m_speed{ p_category.m_speed }
		, m_cur{ p_path.begin() }
	{
		m_destination = p_game.Convert( Point2i{ m_cur->m_x, m_cur->m_y } ) + Point3r{ 0, p_game.GetCellHeight(), 0 };
	}

	Enemy::~Enemy()
	{
	}

	void Enemy::Load( Game const & p_game )
	{
		m_state = State::Walking;
		m_life = m_category.get().m_life.GetValue();
		m_speed = m_category.get().m_speed;
		m_cur = m_path.get().begin();
		m_destination = p_game.Convert( Point2i{ m_cur->m_x, m_cur->m_y } ) + Point3r{ 0, p_game.GetCellHeight(), 0 };
	}

	bool Enemy::Accept( Game const & p_game )
	{
		auto l_speed = p_game.GetElapsed().count() * m_speed / 1000;
		Point3r l_result = m_destination;
		Point3r l_position{ m_node.get().GetPosition() };
		Point3r l_direction{ l_result - l_position };
		auto l_distanceToDst = point::distance( l_direction );
		l_direction[0] *= l_speed / l_distanceToDst;
		l_direction[2] *= l_speed / l_distanceToDst;
		bool l_reachDst{ l_distanceToDst <= l_speed };

		if ( !l_reachDst )
		{
			l_result = l_position + l_direction;
		}
		else if ( l_distanceToDst < l_speed )
		{
			++m_cur;

			if ( m_cur != m_path.get().end() )
			{
				m_destination = p_game.Convert( Point2i{ m_cur->m_x, m_cur->m_y } ) + Point3r{ 0, p_game.GetCellHeight(), 0 };
				auto l_save = float( l_speed - l_distanceToDst );
				std::swap( l_speed, l_save );
				Accept( p_game );
				std::swap( l_speed, l_save );
			}
			else
			{
				m_state = State::Arrived;
				m_node.get().SetPosition( Castor::Point3r{ 0, -10, 0 } );
			}
		}
		else
		{
			// Right on the destination node.
			++m_cur;
			m_destination = p_game.Convert( Point2i{ m_cur->m_x, m_cur->m_y } ) + Point3r{ 0, p_game.GetCellHeight() / 2, 0 };
		}

		bool l_return = m_state == State::Arrived;

		if ( !l_return )
		{
			m_node.get().SetPosition( l_result );
		}

		return l_return;
	}
}
