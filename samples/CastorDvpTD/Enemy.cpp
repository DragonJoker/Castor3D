#include "Enemy.hpp"

#include "Game.hpp"

using namespace castor;
using namespace castor3d;

namespace castortd
{
	Enemy::Enemy( SceneNode & p_node, Game const & p_game, Path const & p_path, Category const & p_category )
		: m_path{ p_path }
		, m_node{ p_node }
		, m_category{ p_category }
		, m_speed{ p_category.m_speed }
		, m_life{ p_category.m_life.getValue() }
		, m_cur{ p_path.begin() }
	{
		m_destination = p_game.convert( Point2i{ m_cur->m_x, m_cur->m_y } ) + castor::Point3f{ 0, p_game.getCellHeight(), 0 };
	}

	void Enemy::load( Game const & p_game )
	{
		m_state = State::Walking;
		m_life = m_category.get().m_life.getValue();
		m_speed = m_category.get().m_speed;
		m_cur = m_path.get().begin();
		m_destination = p_game.convert( Point2i{ m_cur->m_x, m_cur->m_y } ) + castor::Point3f{ 0, p_game.getCellHeight(), 0 };
	}

	bool Enemy::accept( Game const & p_game )
	{
		auto speed = float( p_game.getElapsed().count() ) * m_speed / 1000;
		castor::Point3f destination = m_destination;
		castor::Point3f position{ m_node.get().getPosition() };
		castor::Point3f direction{ destination - position };
		auto distanceToDst = point::length( direction );
		direction[0] *= float( speed / distanceToDst );
		direction[2] *= float( speed / distanceToDst );
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
				m_destination = p_game.convert( Point2i{ m_cur->m_x, m_cur->m_y } ) + castor::Point3f{ 0, p_game.getCellHeight(), 0 };
				auto save = float( speed - distanceToDst );
				std::swap( speed, save );
				accept( p_game );
				std::swap( speed, save );
			}
			else
			{
				m_state = State::Arrived;
				m_node.get().setPosition( castor::Point3f{ 0, -10, 0 } );
			}
		}
		else
		{
			// Right on the destination node.
			++m_cur;
			m_destination = p_game.convert( Point2i{ m_cur->m_x, m_cur->m_y } ) + castor::Point3f{ 0, p_game.getCellHeight() / 2, 0 };
		}

		bool result = m_state == State::Arrived;

		if ( !result )
		{
			m_node.get().setPosition( destination );
		}

		return result;
	}
}
