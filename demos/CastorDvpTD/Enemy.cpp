#include "Enemy.hpp"

#include "Game.hpp"

namespace castortd
{
	Enemy::Enemy( castor3d::SceneNode & node, Game const & game, Path const & path, Category const & category )
		: m_path{ path }
		, m_node{ node }
		, m_category{ category }
		, m_speed{ category.m_speed }
		, m_life{ category.m_life.getValue() }
		, m_cur{ path.begin() }
	{
		m_destination = game.convert( castor::Point2i{ m_cur->m_x, m_cur->m_y } ) + castor::Point3f{ 0, game.getCellHeight(), 0 };
	}

	void Enemy::load( Game const & game )
	{
		m_state = State::Walking;
		m_life = m_category.get().m_life.getValue();
		m_speed = m_category.get().m_speed;
		m_cur = m_path.get().begin();
		m_destination = game.convert( castor::Point2i{ m_cur->m_x, m_cur->m_y } ) + castor::Point3f{ 0, game.getCellHeight(), 0 };
	}

	bool Enemy::accept( Game const & game )
	{
		auto speed = float( game.getElapsed().count() ) * m_speed / 1000;
		castor::Point3f destination = m_destination;
		castor::Point3f position{ m_node.get().getPosition() };
		castor::Point3f direction{ destination - position };
		auto distanceToDst = castor::point::length( direction );
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
				m_destination = game.convert( castor::Point2i{ m_cur->m_x, m_cur->m_y } ) + castor::Point3f{ 0, game.getCellHeight(), 0 };
				auto save = float( speed - distanceToDst );
				castor::swap( speed, save );
				accept( game );
				castor::swap( speed, save );
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
			m_destination = game.convert( castor::Point2i{ m_cur->m_x, m_cur->m_y } ) + castor::Point3f{ 0, game.getCellHeight() / 2, 0 };
		}

		bool result = m_state == State::Arrived;

		if ( !result )
		{
			m_node.get().setPosition( destination );
		}

		return result;
	}
}
