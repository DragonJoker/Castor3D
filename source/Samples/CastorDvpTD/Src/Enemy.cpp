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
		auto speed = p_game.GetElapsed().count() * m_speed / 1000;
		Point3r destination = m_destination;
		Point3r position{ m_node.get().GetPosition() };
		Point3r direction{ destination - position };
		auto distanceToDst = point::length( direction );
		direction[0] *= speed / distanceToDst;
		direction[2] *= speed / distanceToDst;
		bool reachDst{ distanceToDst <= speed };

		if ( !reachDst )
		{
			destination = position + direction;
		}
		else if ( distanceToDst < speed )
		{
			++m_cur;

			if ( m_cur != m_path.get().end() )
			{
				m_destination = p_game.Convert( Point2i{ m_cur->m_x, m_cur->m_y } ) + Point3r{ 0, p_game.GetCellHeight(), 0 };
				auto save = float( speed - distanceToDst );
				std::swap( speed, save );
				Accept( p_game );
				std::swap( speed, save );
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

		bool result = m_state == State::Arrived;

		if ( !result )
		{
			m_node.get().SetPosition( destination );
		}

		return result;
	}
}
