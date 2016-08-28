#include "Tower.hpp"

#include "Enemy.hpp"
#include "Game.hpp"

using namespace Castor;
using namespace Castor3D;

namespace castortd
{
	Tower::Tower( SceneNode & p_node, Cell const & p_cell )
		: m_node{ p_node }
		, m_cell{ p_cell }
	{
	}

	Tower::~Tower()
	{
	}

	void Tower::Update( std::chrono::milliseconds const & p_elapsed, EnemyArray & p_enemies )
	{
		m_remaining -= p_elapsed;

		if ( m_remaining < std::chrono::milliseconds{} )
		{
			m_remaining = std::chrono::milliseconds{};
		}

		if ( m_state == Tower::State::Idle )
		{
			LookForEnemy( p_enemies );
		}
	}

	bool Tower::LookForEnemy( EnemyArray & p_enemies )
	{
		for ( auto & l_enemy : p_enemies )
		{
			if ( m_state != State::Spotted )
			{
				if ( DoIsInRange( *l_enemy ) )
				{
					m_target = l_enemy;
					m_state = State::Spotted;
				}
			}
		}

		return m_state == State::Spotted;
	}

	bool Tower::CanShoot()
	{
		if ( !m_target
			 || !m_target->IsAlive()
			 || !DoIsInRange( *m_target ) )
		{
			m_target = nullptr;
			m_state = State::Idle;
		}

		return m_state != State::Idle && m_remaining == std::chrono::milliseconds{};
	}

	void Tower::Shoot( Game & p_game )
	{
		m_remaining = m_category.m_cooldown;
		p_game.EmitBullet( m_category.m_damage, m_node.GetPosition(), *m_target );
	}

	bool Tower::DoIsInRange( Enemy const & p_enemy )const
	{
		return point::distance( m_node.GetPosition() - p_enemy.GetNode().GetPosition() ) <= m_category.m_range;
	}
}
